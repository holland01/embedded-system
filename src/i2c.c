#include "lpc1114.h"
#include "ssd1306.h"

#define START       1
#define RSTART      2
#define ADDR_W_ACK  3
#define ADDR_W_NACK 4
#define DATA_W_ACK  5
#define DATA_W_NACK 6
#define BUS_LOST    7
#define ADDR_R_ACK  8
#define ADDR_R_NACK 9
#define DATA_R_ACK  10
#define DATA_R_NACK 11
#define AA   (1<<2)
#define SI   (1<<3)
#define STO  (1<<4)
#define STA  (1<<5)
#define I2EN (1<<6)

#define I2C_CONSET_INIT(REG) ( ( (REG) & (~(AA | SI | STO | STA)) ) | I2EN )
#define I2C_BIT_FREQ_100K(REG) (((REG) & 0xFFFF0000) | 0xF0) 
#define I2C_MMCTRL_DISABLE(REG) ((REG) & (~0x1))


/*
*  STO             DATA
*  no IRQ15          IRQ15/ACK
*  +----------------------------------+-----------+ +--------+
*  v                                  |           | v        |
*  +------+         +------+         +------+         +------+    |
*  |0:    |   STA   |1,2:  | ADDR+R  |3:    | DATA    |5:    |    |
*  | IDLE |-------->|START |-------->|ADDR  |-------->|DATA  |----+
*  |      |  IRQ15  |      | IRQ15   |  ACK | IRQ15   |  ACK |
*  +------+         +------+ ACK     ++-----+ ACK     +------+
*  ^ ^             | ^            | |                   |
*  | |       ADDR+R| |   STA      | |DATA               |DATA
*  | |       IRQ15 | +---------+--+ |IRQ15              |IRQ15
*  | |       NACK  v    IRQ15  |    vNACK               |NACK
*  | |          +------+       | +------+               |
*  | | STO      |4:    |       +-|6:*   |               |
*  | +----------|ADDR  |         |DATA  |<--------------+
*  |  no IRQ15  |  NACK|         |  NACK|
*  |            +------+         +------+
*  |                       STO      |
*  +--------------------------------+
*  no IRQ
*
*
*  Note that NACK means different things at different times.  NACK from the
*  address means that the addressed device is unresponsive.  A NACK from
*  data means that the number of bytes being sent is controlled by the
*  receiver and no more data can be sent.
*
*  State 7: This is not drawn.  This state can be entered from any other
*  state except for 0.  This occurs when the data line is held low by
*  another device.  The transfer failed and a start condition must be sent
*  again.
*/

#define DATA_BUFFER_LENGTH 256

volatile struct {
  unsigned char data[DATA_BUFFER_LENGTH];
  unsigned data_length;
  unsigned data_ptr;
} static _I2CBUF = {
  { 0 },
  0,
  0
};
/*
 * See I2C_block()
 */ 
static volatile bool _I2C_busy = false;

/* Defined in vector.c */
extern volatile unsigned HARDFAULT_CODE;
extern void hardfault();

/*
 * I2C-Die
 *
 * Used for error conditions in state machine
 */

static void __I2C_die(unsigned state) {
  HARDFAULT_CODE = state;
  hardfault();
}

/* 
 * I2C-Data-Clear
 *
 * Resets the I2C buffer
 * for the next transmission
 */

static void __I2C_data_clear() {
  _I2CBUF.data_ptr = 0;
    
  for (unsigned i = 0; i < DATA_BUFFER_LENGTH; ++i) {
    _I2CBUF.data[i] = 0xCC;
  }

  _I2CBUF.data_length = 0;
}

/*
 * I2C-con-stop
 *
 * When the bus has finished the
 * entire transfer process, 
 * the synchronization can be completed
 */

static void __I2C_con_stop() {
  _I2C_busy = 0;
  I2C0.CONSET = STO;
  I2C0.CONCLR = STA | AA | SI;  
}


/*
 * I2C-concatenate
 *
 * Take an arbitrary amount of bytes
 * and push it to the data stream
 */

void I2C_cat(const unsigned char* d, unsigned len) {
  unsigned capacity = DATA_BUFFER_LENGTH - _I2CBUF.data_length;

  if (capacity > 0) {
    unsigned length = len < capacity ? len : capacity;
    unsigned i = 0;
    
    while (i < length) {
      _I2CBUF.data[_I2CBUF.data_length + i] = d[i];
      i++;
    }

    _I2CBUF.data_length += length;
  }
}

/*
 * I2C-Push
 *
 * Pushes an arbitrary byte onto the 
 * data stream that's sent over the I2C state machine
 */


void I2C_push(unsigned char b) {
  I2C_cat(&b, 1);
}

/*
 * I2C-console-set-start
 *
 * Initializes the state machine
 * transfer process
 */


void I2C_conset_start() {
  I2C0.CONSET |= STA;
  _I2C_busy = 1;
}

/*
 * I2C-Block
 *
 * Waits until the entire transfer process
 * is finished. 
 */


void I2C_block() {
  while (_I2C_busy) {
    asm("nop");
  }

  volatile unsigned x = 0;

  while (x < 10000) {
    x++;
  }
}

/* IRQ15: I2C state change
 *
 *  Note that the following code only handles Master Write.
 */

void IRQ15() {
  asm("cpsid i");
  
  switch(I2C0.STAT >> 3) {
  case START:
  case RSTART:
    I2C0.DAT = SSD1306_ADDR_WRITE_MASK /* Device Address*/;
    I2C0.CONCLR = STA | STO | SI;
    break;
    
  case ADDR_W_ACK:
  case DATA_W_ACK:
    /*Data to send*/
    if (_I2CBUF.data_ptr < _I2CBUF.data_length) {
      I2C0.DAT = (unsigned)(_I2CBUF.data[_I2CBUF.data_ptr]);
      I2C0.CONCLR = STA | AA | SI;
      
      _I2CBUF.data_ptr++;
    } else {      
      /* Signal success to the thread */
      __I2C_con_stop();
      __I2C_data_clear();
    }
    break;
    
  case ADDR_W_NACK:
    __I2C_die(ADDR_W_NACK);
    /* Signal the error to the thread */
    __I2C_con_stop();
    __I2C_data_clear();
    break;
    
  case DATA_W_NACK:
    __I2C_die(DATA_W_NACK);
    /* End of stream to device */
    __I2C_con_stop();
    __I2C_data_clear();

    HARDFAULT_CODE = DATA_W_NACK;
    break;
    
  case BUS_LOST:
    __I2C_die(BUS_LOST);
    
    /* Signal the falure to the thread */
    I2C0.CONCLR = STA | STO | AA | SI;
    _I2C_busy = 0;

    __I2C_data_clear();
    break;
  }

  asm("cpsie i");
}

/*
 * I2C-Initialize
 */

void I2C_init() {
    // (IC2 = Enable) | (IOCON = Enable)
  SYSCON.SYSAHBCLKCTRL |= (1 << 5) | (1 << 16);
  
  // FUNC = I2C SCL
  IOCON_PIO0_4 |= 0x1;
  // I2CMODE = Standard mode/ Fast-mode I2C
  IOCON_PIO0_4 &= ~((1 << 8) | (1 << 9));

  // FUNC = I2C SDA
  IOCON_PIO0_5 |= 0x1;
  // I2CMODE = Standard mode/ Fast-mode I2C
  IOCON_PIO0_5 &= ~((1 << 8) | (1 << 9));

  // Reset I2C
  SYSCON.PRESETCTRL |= 0x2;
  
  I2C0.SCLH = I2C_BIT_FREQ_100K(I2C0.SCLH);

  I2C0.SCLL = I2C_BIT_FREQ_100K(I2C0.SCLL);

  ISER = ISER_IRQ15_ENABLED;

  __I2C_data_clear();
  
  //I2C0.MMCTRL = I2C_MMCTRL_DISABLE(I2C0.MMCTRL);

  // 0x1 -> monitor mode enabled
  // ~0x2 -> force SDL output to high
  // 0x3 -> monitor all traffic on the bus
  //  I2C0.MMCTRL &= ~0x2;

  // Enable the I2C interface
  I2C0.CONSET |= I2EN;
}




