#include "lpc1114.h"

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
void IRQ15() {
	switch(I2C0.STAT>>3) {
	case START:
	case RSTART:
		I2C0.DAT = /* Device Address*/;
		I2C0.CONCLR = STA | STP | SI;
		break;
	case ADDR_W_ACK:
	case WRITE_W_ACK:
		if(/*Data to send*/) {
			I2C0.DAT = /* Data to device */;
			I2C0.CONCLR = STA | AA | SI;
		}
		else {
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

void I2C_init() {
	// FUNC = I2C
	IOCON_PIO0_4 |= 0x1;
	// I2CMODE = Standard mode/ Fast-mode I2C
	IOCON_PIO0_4 &= ~((1 << 8) | (1 << 9));

	// FUNC = I2C
	IOCON_PIO0_5 |= 0x1;
	// I2CMODE = Standard mode/ Fast-mode I2C
	IOCON_PIO0_5 &= ~((1 << 8) | (1 << 9));

	SYSCON.SYSAHBCLKCTRL
}




