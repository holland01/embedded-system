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
	SYSCON.SYSPLLCLKSEL = SYSCON_SYSPLLCLK_IRC;
	SYSCON.PDRUNCFG &= SYSCON_PDRUNCFG_SYSPLL_ON; 
	
	set_pll_ctrl(3, 1);
	
	SYSCON.SYSPLLCLKUEN = 1;
	
	while (!SYSCON.SYSPLLSTAT)
		asm("");

	SYSCON.MAINCLKSEL = SYSCON_MAINCLKSEL_PLL;
	SYSCON.MAINCLKUEN = 1;

	GPIO0.DATA[PIO_8] = 0;	
	GPIO1.DIR |=  PIO_9;
	
	GPIO1.DATA[PIO_9] = 0;

	asm volatile ("CPSIE i");

	SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_CT16B0_ON;
	SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_ADC_ON;
	SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_IOCON_ON;
	SYSCON.PDRUNCFG &= SYSCON_PDRUNCFG_ADC_ON;

	ADC.CR &= ADC_CR_SEL_CLEAR_MASK; 
	ADC.CR |= ADC_CR_SEL_CHANNEL_0; 

	ADC.CR &= ADC_CR_CLKDIV_CLEAR_MASK;
	ADC.CR |= ADC_CR_CLKDIV_SHIFT(11);

	ADC.CR &= ~(1 << 16); /* [16] BURST = Disabled */
	ADC.CR &= ~((1 << 17) | (1 << 18) | (1 << 19)); /* [19:17] CLKS = 11 clocks / 10 bits */
	ADC.CR &= ~((1 << 24) | (1 << 25) | (1 << 26)); /* [26:24] START = No start */

	//ADC.INTEN |= (1 << 0); /* generate interrupt on conversion complete for AD0 */
	//ADC.INTEN &= ~(1 << 8); /* disable ADGINTEN */

	ADC.INTEN &= ~(1 << 0);
	ADC.INTEN |= (1 << 8);
	
	ISER |= 1 << 24; /* IRQ24 = Enabled */
	ISER |= 1 << 16; /* IRQ16 = Enabled */
		
	SET_LOW_16(TMR16B0.PR, 48); /* 20 */
	SET_LOW_16(TMR16B0.TC, 0);
	SET_LOW_16(TMR16B0.PC, 0);
	SET_LOW_16(TMR16B0.MR1, 20000); /* 20 millisec interval */

	TMR16B0.MCR |= (1 << 3); /* MCR.MR1I */
	TMR16B0.MCR |= (1 << 4); /* MCR.MR1R */
	
	TMR16B0.PWMC |= 0x1 << 0; /* set MAT1 to PWM */
	//TMR16B0.PWMC |= 0x1 << 1;
	
	//TMR16B0.EMR |= 0x1 << 0;
	//TMR16B0.EMR |= 0x2 << 4;
	
	IOCON_R_PIO0_11 &= ~((1 << 0) | (1 << 1) | (1 << 2)); /* [2:0] Set FUNC = AD0 */
	IOCON_R_PIO0_11 |= 0x2;
	
	IOCON_R_PIO0_11 &= ~((1 << 3) | (1 << 4)); /* [4:3] Set MODE = Inactive */
	IOCON_R_PIO0_11 &= ~(1 << 5); /* [5] Set HYS = Disabled */
	IOCON_R_PIO0_11 &= ~(1 << 7); /* [7] Set ADMODE = Analog input */
	IOCON_R_PIO0_11 &= ~(1 << 10);

	IOCON_PIO0_8 &= ~((1 << 11) - 1); /* [2:0] FUNC = CT16B0_MAT0 */
	IOCON_PIO0_8 |= 0x2;

	TMR16B0.TCR |= 2; /* reset timer */
	TMR16B0.TCR &= ~0x3;
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

// volatile unsigned WIDTH = MR1 - 500 - (VREF << 1);

volatile unsigned IRQ24_TICK = 1;

void IRQ24() {
	volatile unsigned DONE = ADC.STAT & 1;

	if (DONE) {
		volatile unsigned VREF = (ADC.R0 >> 6) & 0x3FF;
		volatile unsigned MR1 = GET_LOW_16(TMR16B0.MR1);
		
		unsigned ROTATE = ((VREF) | 1) * IRQ24_TICK;
		ROTATE &= 0x3ff;
		
		volatile unsigned WIDTH= MR1 - 500 - (ROTATE);
		volatile unsigned TC = GET_LOW_16(TMR16B0.TC);

		IRQ24_TICK++;
		
		SET_LOW_16(TMR16B0.MR0, WIDTH);
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
