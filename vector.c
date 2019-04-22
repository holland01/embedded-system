#include "gpio.h"

#define ATTR(...) __attribute__((__VA_ARGS__))
#define FPTR(x) ((word_t)(x) + 1)

extern void hardfault() ATTR(weak, alias("default_hardfault"));
extern void NMI() ATTR(weak, alias("default_NMI"));
extern void reset();

extern word_t __INITIAL_SP;
extern word_t __VECTOR_CHECKSUM;

word_t vector[8] ATTR(section(".vector")) = {
	(word_t)&__INITIAL_SP,
	FPTR(reset),
	FPTR(NMI),
	FPTR(hardfault),
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

static volatile bool_t default_hardfault_loop = True;

void default_hardfault() {
	while (default_hardfault_loop) {
		
	}
}

void default_NMI() {
	
}
