#include "sysctrl.h"
#include "flashctrl.h"

struct SYS_CTRL_BLOCK* SYSCTRL = NULL;

// clock notes:
// internal RC oscillator is 12 MHz
// crystal oscillator is 1 MHz to 25 MHz
// watchdog oscillator is 7.8 kHz to 1.8 MHz

void setup_sysctrl() {
	// setup morse code info
	SYSCTRL = (struct SYS_CTRL_BLOCK*) SYS_CTRL_BLOCK_ADDR;

	// phase locked loop
	{
		// set the system pll clock source to be the IRC oscillator
		SYSCTRL->data[SYS_CTRL_PLL_CLOCK_SRC_SEL] = CSS_IRC_OSCILLATOR;

		// system pll control
		{
			volatile word_t pll_ctrl = 0;

			// feedback divider value
			pll_ctrl |= SYS_CTRL_PLL_CTRL_MSEL(4);

			// post divider ratio
			pll_ctrl |= SYS_CTRL_PLL_CTRL_POSTDIV_2;

			SYSCTRL->data[SYS_CTRL_PLL_CTRL] = pll_ctrl;
		}

		// disable power down for the phase locked loop,
		// (see 3.11.2).
		// when power down happens, dividers enter reset state and oscillator
		// is stopped, among other things.
		// this is supposed to give the pll power
		{
			volatile word_t power_down = SYSCTRL->data[SYS_CTRL_PWR_DOWN_CFG];
		
			// set system phase locked loop
			// state to "powered"
			power_down &= ~(0x80);

			SYSCTRL->data[SYS_CTRL_PWR_DOWN_CFG] = power_down;
		}

	
		// update pll clock src config,
		// since we've changed a number of its
		// settings
		SYSCTRL->data[SYS_CTRL_PLL_CLOCK_SRC_UPDATE] |= True;
	}

	setup_flashctrl();
	
	// loop until phase lock status
	// is set (i.e., the frequency has been appropriately updated) (see 3.11.1)
	
	while (SYSCTRL->data[SYS_CTRL_PLL_STATUS] == False) {
		asm("nop");
	}
	

	// main system clock
	{
		// take the input from the output of the PLL
		SYSCTRL->data[SYS_CTRL_MAIN_CLOCK_SRC_SEL] |= SYS_CTRL_MAIN_CLOCK_SRC_SEL_PLL_OUT;

		// notify hardware we've updated the clock source
		SYSCTRL->data[SYS_CTRL_MAIN_CLOCK_SRC_UPDATE] |= True;
	}
}
