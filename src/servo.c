#include "lpc1114.h"
#include "framework.h"

/* Setup
 * Involves the following:
 *  - Initialize PLL
 *  - Enable interrupts
 *  - Configure clocks with SYSAHBCLKCTRL
 *  - Setup analog to digital input
 *  - Setup IOCON_R_PIO0_11 to use ADC
 *  - Setup IOCON_PIO0_8 to use CTB16_MAT0
 *  - Configure TMR16B0 block to utilize
 *    pulse width modulation at a 20 millisecond
 *    cycle. This is for the servo motor connection. 
 */

void setup() {
	setup_pll();
	
	enable_ints();

	setup_iocon();
	setup_adc();
	setup_timer();
	
	GPIO0.DATA[PIO_8] = 0;
	GPIO1.DIR |=  PIO_9;
	GPIO1.DATA[PIO_9] = 0;
}

/* Loop
 *  Simple loop to blink an LED on PIO1_9.
 */

#define COUNT 120000
void loop() {
	asm volatile("wfi");
}

/* IRQ30
 *  When PIO1_8 triggers the interrupt, IRQ30 resets the PLL to a 
 different
 *  speed.  The array speeds holds the PLL settings and speed is the 
 current
 *  index into the speeds array.
 */

/*
const
struct {
	unsigned MSEL;
	unsigned PSEL;
} speeds [] =
	{
		{3, 1}, {2, 2}, {1, 2}, {0, 3}
	};
int speed = 0;
void IRQ30() {
	GPIO1.IC = PIO_8;
	SYSCON.MAINCLKSEL = 0;
	SYSCON.MAINCLKUEN = 1;
	speed = (speed + 1) & 0x3;
	//SYSCON.SYSPLLCTRL.MSEL = speeds[speed].MSEL;
	//SYSCON.SYSPLLCTRL.PSEL = speeds[speed].PSEL;

	set_pll_ctrl(speeds[speed].MSEL, speeds[speed].PSEL);

	SYSCON.SYSPLLCLKUEN    = 1;
	while (!SYSCON.SYSPLLSTAT)
		asm("");
	SYSCON.MAINCLKSEL = 3;
	SYSCON.MAINCLKUEN = 1;
}

*/
/*
* - prescale register (PR)
* specifies max value for prescale counter (PC)
* 
* - prescale counter (PC)
*   controls division of PCLK by some constant value
*   before it's applied to the timer counter. The frequency
    of PCLK defines the rate at which the prescale counter is incremented.
*
* - PCLK (peripheral clock)
*   - externally supplied clock
*/
