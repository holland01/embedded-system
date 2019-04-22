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
#define SYS_CTRL_MAIN_CLOCK_SRC_UPDATE 0x1

#define SYS_CTRL_PLL_CLOCK_SRC_UPDATE 0x1

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
#define FLASH_CTRL_FLASHCFG_OF FSET REG_OFFSET(0x10)

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

		// power down the phase locked loop (why????)
		{
			volatile word_t power_down = SYSCTRL->data[SYS_CTRL_PWR_DOWN_CFG];
		
			// set system phase locked loop
			// state to "powered down"
			power_down &= ~(0x80);

			SYSCTRL->data[SYS_CTRL_PWR_DOWN_CFG] = power_down;
		}

	
		// update pll clock src config,
		// maybe to power down the phase locked loop
		SYSCTRL->data[SYS_CTRL_PLL_CLOCK_SRC_UPDATE] |= SYS_CTRL_PLL_CLOCK_SRC_UPDATE;
	}
	
	
}

void reset() {
	volatile word_t counter = 0;
	
	while (True) {
		counter++;
	}
}
