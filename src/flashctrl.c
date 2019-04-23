#include "flashctrl.h"
#include "common.h"

word_t* FLASH_CTRL_FLASHCFG = NULL;

void setup_flashctrl() {
	// set flash control memory access time to
	// 2 system clocks
	{
		FLASH_CTRL_FLASHCFG = ((word_t*)FLASH_CTRL_BLOCK_ADDR) + FLASH_CTRL_FLASHCFG_OFFSET;
		
		word_t flash = *FLASH_CTRL_FLASHCFG;

		flash |= FLASH_CTRL_MEMORY_ACCESS_TIME_2;
		
		*FLASH_CTRL_FLASHCFG = flash;
	}

}
