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

/* IRQ15: I2C state change
 *
 *  Note that the following code only handles Master Write.
 */

enum {
	CMD_STATE_INIT = 0,
	CMD_STATE_WRITE_TEXT,
	CMD_STATE_END
};

static unsigned __cmd_state[] = {
	CMD_STATE_INIT,
	CMD_STATE_WRITE_TEXT,
	CMD_STATE_END
};

#define TEXTBUFLEN 64

static char* text_buffer[TEXTBUFLEN] = { 0 };

struct cmd {
	char* data;
	unsigned data_length;
	unsigned data_ptr;
	unsigned state_length;
	unsigned state_ptr;
} __cmd = {
	SSD1306_INIT,
	SSD1306_INIT_cOUNT,
	0,
	sizeof(__cmd_state) / sizeof(__cmd_state[0]),
	0
};

void write_clear() {
	I2C0.DAT = (unsigned)(__cmd.data[__cmd.data_ptr]);
	I2C0.CONCLR = STA | AA | SI;
				
	__cmd.data_ptr++;			
}

void IRQ15() {
  switch(I2C0.STAT >> 3) {
  case START:
  case RSTART:
    I2C0.DAT = SSD1306_ADDR_WRITE_MASK /* Device Address*/;
    I2C0.CONCLR = STA | STO | SI;
    break;
    
  case ADDR_W_ACK:
  case DATA_W_ACK:
    /*Data to send*/
    if (__cmd.data_ptr < __cmd.data_length) {
			if (__cmd_state[__cmd.state_ptr] == CMD_STATE_WRITE_TEXT) {
				if (__cmd.data[__cmd.data_ptr] == 0) {
					__cmd.data_length = __cmd.data_ptr;
				} else {
					write_clear();
				}
			} else {
				write_clear();
			}
    } else {			
      /* Signal success to the thread */
      I2C0.CONSET = STO;
      I2C0.CONCLR = STA | AA | SI;
    }
    break;
    
  case ADDR_W_NACK:
    /* Signal the error to the thread */
    I2C0.CONSET = STO;
    I2C0.CONCLR = STA | AA | SI;
    break;
    
  case DATA_W_NACK:
		__cmd.data_ptr = 0;
		
		if (__cmd.state_ptr < __cmd.state_length) {
			__cmd.state_ptr++;
			
			switch (__cmd_state[__cmd.state_ptr]) {
			case CMD_STATE_WRITE_TEXT:
				__cmd.data_ptr = text_buffer;
				__cmd.data_length = TEXTBUFLEN;
				break;
				
			case CMD_STATE_END:				
				break;
			}
		} 
    /* End of stream to device */
    I2C0.CONSET = STO;
    I2C0.CONCLR = STA | AA | SI;
    break;
    
  case BUS_LOST:
    /* Signal the falure to the thread */
    I2C0.CONCLR = STA | STO | AA | SI;
    break;
  }
}

static void d(int d) {
  volatile int count = 0;
  volatile int duration = d * 1000000;

  while (count < duration) {
    count++;
  }
}

void I2C_init() {

  d(150);

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

  //I2C0.MMCTRL = I2C_MMCTRL_DISABLE(I2C0.MMCTRL);

  // 0x1 -> monitor mode enabled
  // ~0x2 -> force SDL output to high
  // 0x3 -> monitor all traffic on the bus
  //  I2C0.MMCTRL &= ~0x2;
  
  I2C0.CONSET |= I2EN;
  I2C0.CONSET |= STA;
}




