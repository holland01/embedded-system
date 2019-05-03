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
#define GET_LOW_16(reg) ((reg) & 0x0000FFFF)

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
	SYSCON.SYSPLLCLKSEL    = 0;
	SYSCON.PDRUNCFG &= ~(1 << 7); /* SYSPLL = Powered */
	
	set_pll_ctrl(3, 1);
	
	SYSCON.SYSPLLCLKUEN    = 1;
	
	while (!SYSCON.SYSPLLSTAT)
		asm("");

	SYSCON.MAINCLKSEL = 3;
	SYSCON.MAINCLKUEN = 1;

	GPIO0.DIR |= PIO_8;
	GPIO0.DATA[PIO_8] = 0;
	
	GPIO1.DIR |=  PIO_9;

	//GPIO1.DIR &= ~PIO_8;
	//GPIO1.IS  &= ~PIO_8;
	//GPIO1.IBE &= ~PIO_8;
	//GPIO1.IEV &= ~PIO_8;
	//GPIO1.IE  |=  PIO_8;

	GPIO1.DATA[PIO_9] = 0;

	asm volatile ("CPSIE i");

	SYSCON.SYSAHBCLKCTRL |= (1 << 7); /* CT16B0 = enable */
	SYSCON.SYSAHBCLKCTRL |= (1 << 13); /* ADC = enable */
	SYSCON.SYSAHBCLKCTRL |= (1 << 16); /* IOCON = enable */
	SYSCON.PDRUNCFG &= ~(1 << 4) /* ADC = Powered */;

	ADC.CR &= ~0xFF; /* [7:0] SEL = Channel 0 */
	ADC.CR |= (1 << 0); 

	ADC.CR &= 0xFFFF00FF; /* [15:8] CLKDIV = 12 */
	ADC.CR |= 12 << 8;

	ADC.CR &= ~(1 << 16); /* [16] BURST = Disabled */
	
	ADC.CR &= ~((1 << 17) | (1 << 18) | (1 << 19)); /* [19:17] CLKS = 11 clocks / 10 bits */
	
	ADC.CR &= ~((1 << 24) | (1 << 25) | (1 << 26)); /* [26:24] START = No start */
	
	ISER |= 1 << 24; /* IRQ24 = Enabled */
	ISER |= 1 << 16; /* IRQ16 = Enabled */
		
	SET_LOW_16(TMR16B0.PR, 48); /* 20 */
	SET_LOW_16(TMR16B0.TC, 0);
	SET_LOW_16(TMR16B0.PC, 0);
	SET_LOW_16(TMR16B0.MR1, 20000); /* 20 millisec interval */

	TMR16B0.MCR |= (1 << 3); /* MCR.MR1R */
	TMR16B0.MCR |= (1 << 4); /* MCR.MR1I */
	
	//TMR16B0.PWMC |= 0x1; /* set MAT1 to PWM */


	IOCON_R_PIO0_11 &= ~((1 << 0) | (1 << 1) | (1 << 2)); /* [2:0] Set FUNC = AD0 */
	IOCON_R_PIO0_11 |= 0x2;
	
	IOCON_R_PIO0_11 &= ~((1 << 3) | (1 << 4)); /* [4:3] Set MODE = Inactive */
	IOCON_R_PIO0_11 &= ~(1 << 5); /* [5] Set HYS = Disabled */
	IOCON_R_PIO0_11 &= ~(1 << 7); /* [7] Set ADMODE = Analog input */
	IOCON_R_PIO0_11 &= ~(1 << 10);
	
	TMR16B0.TCR |= 0x1; /* enable counter */
}
/* Loop
 *  Simple loop to blink an LED on PIO1_9.
 */
#define COUNT 120000
void loop() {
	asm volatile("wfi");
}

static volatile unsigned start_state = 0;

void pause() {
	for (int i = 0; i < 4000000; ++i)
		asm("nop");

}

void IRQ16() {
	TMR16B0.IR |= (1 << 1); /* set MR1 to HIGH */
	ADC.CR |= (1 << 24); /* [26:24] START = Start conversion now */

	GPIO1.DATA[PIO_9] = 0;
	GPIO0.DATA[PIO_8] = 0;
}

void IRQ24() {
	unsigned DONE = ADC.STAT & 1;

	if (DONE) {
		unsigned k = (ADC.R0 >> 6) & 0x3FF;
 
		unsigned time = GET_LOW_16(TMR16B0.TC);

		if (0 <= time && time <= 500) {
			if (530 <= k && k <= 535) {
				GPIO1.DATA[PIO_9] = PIO_9;
				GPIO0.DATA[PIO_8] = 0;
			} else {
				GPIO1.DATA[PIO_9] = 0;
				GPIO0.DATA[PIO_8] = PIO_8;
			}
		}

	} 	
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
