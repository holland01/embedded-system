#include "lpc1114.h"
#include "framework.h"

void blink(int pin, int delay);

THREAD(one, blink, 64, PIO_1, 1000000, 0, 0);
THREAD(two, blink, 64, PIO_2,  660000, 0, 0);
/* Setup
 *  GPIO0 1, 2, and 3 as output and zero.
 *  GPIO0_1 (physical pin 24)  Loop 1's LED
 *  GPIO0_2 (physical pin 25)  Loop 2's LED
 *  GPIO0_3 (physical pin 26)  Hard Fault's LED
 */
#if 0
void setup() {
		/* enable clock for GPIO */
	SYSCON.SYSAHBCLKCTRL |= 1 << 6;
	GPIO0.DIR |= PIO_1 | PIO_2 | PIO_3;
	GPIO0.DATA[PIO_1 | PIO_2 | PIO_3] = 0;
}
#endif
/* This is the function body for both threads.
 */
void blink(int pin, int delay) {
	while(1) {
		GPIO0.DATA[pin] ^= ~0;
		for(int i = 0; i < delay; ++i) {}
	}
}
/* HardFault
 *  This is really here to tell you that something went wrong with your
 *  scheduler.  If you attached GDB and end up here, take a look at the 
 program
 *  stack (psp) with gdb.  Since this is an exception, sp is actually msp 
 not
 *  psp.
 */
void HardFault () {
	/* enable clock for GPIO */
	SYSCON.SYSAHBCLKCTRL |= 1 << 6;
	GPIO0.DIR |= PIO_3;
	GPIO0.DATA[PIO_3] = ~0;
	
	while(1)
		asm("wfi");
}

#if 0
/* It would be better if your OS did not require a loop funciton.  Just in 
	 case
	 *  yours does require a loop, here it is.
	 */
void loop () {
	asm("wfi");
}
#endif
