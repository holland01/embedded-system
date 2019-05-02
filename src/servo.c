#include "lpc1114.h"

static inline void set_pll_ctrl(unsigned MSEL, unsigned PSEL) {
	SYSCON.SYSPLLCTRL.MSEL = MSEL;
	SYSCON.SYSPLLCTRL.PSEL = PSEL;

	// this must be explicitly zero'd out,
	// otherwise the compiler will write a
	// 1 to one of the reserved bits.
	SYSCON.SYSPLLCTRL.RESERVED = 0; 
}

#define SET_LOW_16(reg, val) (reg) &= 0xFFFF0000; (reg) |= (val)
#define GET_LOW_16(reg) ((reg) & 0xFFFF0000)

/* Setup
 *  There are three primary setup steps: PLL, PIO1_9, and PIO1_8.  The PLL 
 is set
 *  for 48 MHz.  PIO1_9 is set as output.  PIO1_8 is set as input 
 triggering
 *  interrupt on falling edge.  Finally, GPIO1 triggers IRQ30, which is 
 enabled
 *  is ISER.
 */

static volatile unsigned* DEBUG_DUMP = 0;

void setup() {
#if 0
	SYSCON.SYSPLLCLKSEL    = 0;
	SYSCON.PDRUNCFG.SYSPLL = 0;
	
	set_pll_ctrl(3, 1);
	
	SYSCON.SYSPLLCLKUEN    = 1;
	
	while (!SYSCON.SYSPLLSTAT)
		asm("");

	SYSCON.MAINCLKSEL = 3;
	SYSCON.MAINCLKUEN = 1;
#endif
	
	GPIO1.DIR |=  PIO_9;

	GPIO1.DIR &= ~PIO_8;
	GPIO1.IS  &= ~PIO_8;
	GPIO1.IBE &= ~PIO_8;
	GPIO1.IEV &= ~PIO_8;
	GPIO1.IE  |=  PIO_8;

	GPIO1.DATA[PIO_9] = 0;

	asm volatile ("CPSIE i");
	
	//	ISER |= 1 << 30;
	ISER |= 1 << 16;
	
	SYSCON.SYSAHBCLKCTRL |= (1 << 7); /* CT16B0 */
	
	SET_LOW_16(TMR16B0.PR, 12000); /* 20 */
	SET_LOW_16(TMR16B0.TC, 0);
	SET_LOW_16(TMR16B0.PC, 0);
	SET_LOW_16(TMR16B0.MR1, 1000); /* 20 millisec interval */

	TMR16B0.MCR |= (1 << 3); /* MCR.MR1R */
	TMR16B0.MCR |= (1 << 4); /* MCR.MR1I */
	
	TMR16B0.TCR |= 0x1; /* enable counter */
	
	DEBUG_DUMP = &TMR16B0.PWM;
}
/* Loop
 *  Simple loop to blink an LED on PIO1_9.
 */
#define COUNT 120000
void loop() {
	#if 0
	volatile int k = 0;

	asm volatile(
		"mov r0, #0xFF\n\t"
		"mov r1, #0x1F\n\t"
		"bic r0, r1\n\t"
		"mov %[result], r0"
		: [result] "=r" (k)
		);

	if (k == 0xe0) {
		GPIO1.DATA[PIO_9] = PIO_9;
		for(int i = 0; i < COUNT; ++i)
			asm("");
	}
	#endif

	asm volatile("wfi");
}

static volatile unsigned start_state = 0;

void IRQ16() {
	TMR16B0.IR |= (1 << 1); /* set MR1 to HIGH */

	//	ICER |= 1 << 16;
	//ISER |= 1 << 16;
	
	GPIO1.DATA[PIO_9] ^= PIO_9;

	//unsigned h = GET_LOW_16(TMR16B0.TC);

	//if (500 <= h && h <= 2500) {
		//TMR16B0.IR |= (1 << 0); /* set MR0 to HIGH */
	//} else {
		
	//

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
