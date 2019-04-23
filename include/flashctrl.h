#ifndef __FLASHCTRL_H__
#define __FLASHCTRL_H__

#define FLASH_CTRL_BLOCK_ADDR 0x4003C000
#define FLASH_CTRL_FLASHCFG_OFFSET REG_OFFSET(0x10)
#define FLASH_CTRL_MEMORY_ACCESS_TIME_2 0x1

extern word_t* FLASH_CTRL_FLASHCFG;

void setup_flashctrl();

#endif
