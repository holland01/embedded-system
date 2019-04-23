#include "gpio.h"

#define FUNC_ADDR(x) ((word_t)(x) + 1)

extern void hardfault() __attribute__((weak, alias("default_hardfault")));									  
extern void NMI() __attribute__((weak, alias("default_NMI")));
								
extern void reset();

extern word_t __INITIAL_SP;
extern word_t __VECTOR_CHECKSUM;

word_t vector[8] __attribute__((section(".vector"))) = {
	(word_t)&__INITIAL_SP,
	FUNC_ADDR(reset),
	FUNC_ADDR(NMI),
	FUNC_ADDR(hardfault),
	0,
	0,
	0,
	(word_t)&__VECTOR_CHECKSUM
};

struct GPIO_CTRL* GPIO[GPIO_NUM_PORTS] = {
	NULL,
	NULL,
	NULL,
	NULL
};

void default_hardfault() {
	while (True) {
		asm("nop");
	}
}

void default_NMI() {
	
}

