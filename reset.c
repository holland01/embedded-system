#include "common.h"
#include "gpio.h"

#define SYS_CTRL_DATA_LEN (0x3F8 >> 2)

#define SYS_CTRL_BLOCK_ADDR 0x40048000

enum SYS_CTRL_BLOCK_REG_OFFSETS {
	SYS_CTRL_PLL_CTRL = REG_OFFSET(0x8),
	SYS_CTRL_PLL_STATUS = REG_OFFSET(0xC),
	SYS_CTRL_PLL_CLOCK_SRC_SEL = REG_OFFSET(0x40),
	SYS_CTRL_PLL_CLOCK_SRC_UPDATE = REG_OFFSET(0x44),
	SYS_CTRL_MAIN_CLOCK_SRC_SEL = REG_OFFSET(0x70),
	SYS_CTRL_MAIN_CLOCK_SRC_UPDATE = REG_OFFSET(0x74),
	SYS_CTRL_PWR_DOWN_CFG = REG_OFFSET(0x238)
};

#define SYS_CTRL_MAIN_CLOCK_SRC_SEL_PLL_OUT 0x3

#define SYS_CTRL_PWR_DOWN_CFG_RESERVED_MASK			\
	((1 << 8) | (0 << 9) | (1 << 10) | (1 << 11) |	\
	 (0 << 12) | (1 << 13) | (1 << 14) | (1 << 15))

#define SYS_CTRL_PWR_DOWN_PLL_POWERED 0x0
#define SYS_CTRL_PWR_DOWN_PLL(value) ((value & 0x1) << 7)

#define SYS_CTRL_PLL_CTRL_POSTDIV_4 0x2
#define SYS_CTRL_PLL_CTRL_POSTDIV_2 0x1

#define SYS_CTRL_PLL_CTRL_MSEL(div) (div - 1)
#define SYS_CTRL_PLL_CTRL_PSEL(value) ((value & 0x3) << 5)

struct SYS_CTRL_BLOCK {
	word_t data[SYS_CTRL_DATA_LEN];
};

enum CLOCK_SOURCE_SELECT_ {
	CSS_IRC_OSCILLATOR = 0
};

//------------------------------------------------------------

#define FLASH_CTRL_BLOCK_ADDR 0x4003C000
#define FLASH_CTRL_FLASHCFG_OFFSET REG_OFFSET(0x10)
#define FLASH_CTRL_MEMORY_ACCESS_TIME_2 0x1
			
//-------------------------------------------------------

static word_t* FLASH_CTRL_FLASHCFG =
	((word_t*)FLASH_CTRL_BLOCK_ADDR) + FLASH_CTRL_FLASHCFG_OFFSET;

static struct SYS_CTRL_BLOCK* SYSCTRL = (struct SYS_CTRL_BLOCK*)(SYS_CTRL_BLOCK_ADDR);

static void setup() {
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

	// set flash control memory access time to
	// 2 system clocks
	{
		word_t flash = *FLASH_CTRL_FLASHCFG;

		flash |= FLASH_CTRL_MEMORY_ACCESS_TIME_2;
		
		*FLASH_CTRL_FLASHCFG = flash;
	}

	// loop until phase lock status
	// is set (i.e., the frequency has been appropriately updated) (see 3.11.1)
	{
		while (SYSCTRL->data[SYS_CTRL_PLL_STATUS] == False) {
			asm("nop");
		}
	}

	// main system clock
	{
		// take the input from the output of the PLL
		SYSCTRL->data[SYS_CTRL_MAIN_CLOCK_SRC_SEL] |= SYS_CTRL_MAIN_CLOCK_SRC_SEL_PLL_OUT;

		// notify hardware we've updated the clock source
		SYSCTRL->data[SYS_CTRL_MAIN_CLOCK_SRC_UPDATE] |= True;
	}
}

void reset() {
	setup();

	volatile word_t counter = 0;
	
	while (True) {
		counter++;
	}
}
