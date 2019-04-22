#include "gpio.h"

#define SYSCTRL_DATA_LEN 0x300

#define REG_OFFSET(byte_offset) (byte_offset) >> 2

enum SYS_CTRL_BLOCK_REG_OFFSETS {
	SYS_CTRL_PLL_CLOCK_SRC_SEL = REG_OFFSET(0x40),
	SYS_CTRL_PLL_CTRL = REG_OFFSET(0x8),
	SYS_CTRL_PWR_DOWN_CFG = REG_OFFSET(0x238)
};

#define SYS_CTRL_PWR_DOWN_CFG_RESERVED_MASK			\
	((1 << 8) | (0 << 9) | (1 << 10) | (1 << 11) |	\
	 (0 << 12) | (1 << 13) | (1 << 14) | (1 << 15))

#define SYS_CTRL_PWR_DOWN

#define SYS_CTRL_PLL_CTRL_POSTDIV_4 0x2

#define SYS_CTRL_PLL_CTRL_MSEL(div) (div - 1)
#define SYS_CTRL_PLL_CTRL_PSEL(value) ((value & 0x3) << 5)

struct SYS_CTRL {
	word_t data[SYSCTRL_DATA_LEN];
} _SYS_CTRL;

enum CLOCK_SOURCE_SELECT_ {
	CSS_IRC_OSCILLATOR = 0
};

void reset() {
	volatile word_t counter = 0;

	while (True) {
		counter++;
	}
}
