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

enum /* morse tick values in milliseconds */ {
	MT_DOT = 250,
	MT_DASH = 750,
	MT_INTER_DOT = 250,
	MT_INTER_WORD = 1750,
	MT_INTER_CHAR = 750
};

enum /* morse command indices */ {
	MC_H = 0,
	MC_E,
	MC_L,
	MC_O,
	MC_W,
	MC_R,
	MC_D,
	MC_INTER_CHAR,
	MC_INTER_WORD,
	NUM_MORSE_COMMANDS
};

#define MAX_MORSE_TICK 10

struct MORSE_COMMAND {
	word_t tick[MAX_MORSE_TICK];
	word_t pin[MAX_MORSE_TICK];
	word_t tick_count;
};

struct MORSE_COMMAND MORSE_CMDS[NUM_MORSE_COMMANDS] = {0};

//-------------------------------------------------------

word_t* FLASH_CTRL_FLASHCFG = NULL;

struct SYS_CTRL_BLOCK* SYSCTRL = NULL;

static void setup_morse() {
	// H
	{
		MORSE_CMDS[MC_H].tick_count = 7;
			
		MORSE_CMDS[MC_H].tick[0] = MT_DOT;
		MORSE_CMDS[MC_H].tick[1] = MT_INTER_DOT;

		MORSE_CMDS[MC_H].tick[2] = MT_DOT;
		MORSE_CMDS[MC_H].tick[3] = MT_INTER_DOT;

		MORSE_CMDS[MC_H].tick[4] = MT_DOT;
		MORSE_CMDS[MC_H].tick[5] = MT_INTER_DOT;

		MORSE_CMDS[MC_H].tick[6] = MT_DOT;

		MORSE_CMDS[MC_H].pin[0] = GPIO_PIN_9;
		MORSE_CMDS[MC_H].pin[1] = 0;

		MORSE_CMDS[MC_H].pin[2] = GPIO_PIN_9;
		MORSE_CMDS[MC_H].pin[3] = 0;

		MORSE_CMDS[MC_H].pin[4] = GPIO_PIN_9;
		MORSE_CMDS[MC_H].pin[5] = 0;

		MORSE_CMDS[MC_H].pin[6] = GPIO_PIN_9;
	}

	// E
	{
		MORSE_CMDS[MC_E].tick_count = 1;
		MORSE_CMDS[MC_E].tick[0] = MT_DOT;
		MORSE_CMDS[MC_E].pin[0] = GPIO_PIN_9;
	}

	// L
	{
		MORSE_CMDS[MC_L].tick_count = 5;

		MORSE_CMDS[MC_L].tick[0] = MT_DOT;
		MORSE_CMDS[MC_L].tick[1] = MT_DASH;
		MORSE_CMDS[MC_L].tick[2] = MT_DOT;
		
		MORSE_CMDS[MC_L].tick[3] = MT_INTER_DOT;

		MORSE_CMDS[MC_L].tick[4] = MT_DOT;

		MORSE_CMDS[MC_L].pin[0] = GPIO_PIN_9;
		MORSE_CMDS[MC_L].pin[1] = GPIO_PIN_9;
		MORSE_CMDS[MC_L].pin[2] = GPIO_PIN_9;

		MORSE_CMDS[MC_L].pin[3] = 0;

		MORSE_CMDS[MC_L].pin[4] = GPIO_PIN_9;
	}
	
	// inter char
	{
		MORSE_CMDS[MC_INTER_CHAR].tick_count = 1;
		MORSE_CMDS[MC_INTER_CHAR].tick[0] = MT_INTER_CHAR;
		MORSE_CMDS[MC_INTER_CHAR].pin[0] = 0;
	}

	// inter word
	{
		MORSE_CMDS[MC_INTER_WORD].tick_count = 1;
		MORSE_CMDS[MC_INTER_WORD].tick[0] = MT_INTER_WORD;
		MORSE_CMDS[MC_INTER_WORD].pin[0] = 0;
	}
}

void sleep(word_t millisec) {
	const word_t TICK = 1000; // 1 millisecond in microseconds
	const word_t STEP = millisec * TICK;

	volatile word_t counter = 0;
	while (counter < STEP) {
		counter++;
	}
}

static void write_morse_command(word_t index) {
	struct MORSE_COMMAND* m = &MORSE_CMDS[index];

	volatile word_t counter = 0;
	
	while (counter < m->tick_count) {
		GPIO[1]->data[GPIO_PIN_9] = m->pin[counter];
		sleep(m->tick[counter]);
		
		counter++;
	}
}

// clock notes:
// internal RC oscillator is 12 MHz
// crystal oscillator is 1 MHz to 25 MHz
// watchdog oscillator is 7.8 kHz to 1.8 MHz

static void setup() {
	// setup morse code info
	setup_morse();
			
	SYSCTRL = (struct SYS_CTRL_BLOCK*)SYS_CTRL_BLOCK_ADDR;
	
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
		FLASH_CTRL_FLASHCFG = ((word_t*)FLASH_CTRL_BLOCK_ADDR) + FLASH_CTRL_FLASHCFG_OFFSET;
		
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

	// set GPIO values
	GPIO[0] = (struct GPIO_CTRL*) GPIO_PORT_0;
	GPIO[1] = (struct GPIO_CTRL*) GPIO_PORT_1;
	GPIO[2] = (struct GPIO_CTRL*) GPIO_PORT_2;
	GPIO[3] = (struct GPIO_CTRL*) GPIO_PORT_3;
}

void reset() {
	setup();

	const word_t MAX_STEP = 0x000f4240; // 1_000_000
	
	// set pin 9 as output
	GPIO[1]->io_dir |= GPIO_PIN_9;
	
	while (True) {
		write_morse_command(MC_H);
		write_morse_command(MC_INTER_CHAR);
		write_morse_command(MC_E);
		write_morse_command(MC_INTER_CHAR);
		write_morse_command(MC_L);
		write_morse_command(MC_INTER_CHAR);
		write_morse_command(MC_L);
		write_morse_command(MC_INTER_WORD);
	}
}
