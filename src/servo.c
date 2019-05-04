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

	SYSCON.PDRUNCFG &= ~SYSCON_PDRUNCFG_SYSPLL_OFF; 
	
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
	
	SYSCON.PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_OFF;

	ADC.CR = ADC_CR_SEL_SET_CHANNEL(0); 
	ADC.CR = ADC_CR_CLKDIV_SET_VALUE(11);
	ADC.CR = ADC_CR_BURST_SET_OFF;
	ADC.CR = ADC_CR_CLKS_SET_11_CLK_10_BIT;
	ADC.CR = ADC_CR_START_SET_NO_START;

	ADC.INTEN = ADC_INTEN_SET_ADGINTEN_ONLY;
	
	ISER = ISER_IRQ24_ENABLED; 
	ISER = ISER_IRQ16_ENABLED;
	
	SET_LOW_16(TMR16B0.PR, 48); 
	SET_LOW_16(TMR16B0.TC, 0);
	SET_LOW_16(TMR16B0.PC, 0);
	SET_LOW_16(TMR16B0.MR1, 20000); 

	TMR16B0.MCR = TMR16B0_MCR_ENABLE_MR1_I;
	TMR16B0.MCR = TMR16B0_MCR_ENABLE_MR1_R;
	
	TMR16B0.PWMC = TMR16B0_PWMC_ENABLE_MR0;
	
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
