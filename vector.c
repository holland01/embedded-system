#include "gpio.h"

#define ATTR(s) __attribute__((s))
#define FPTR(x) ((word_t)(x) + 1)

extern void hardfault() ATTR(weak);
extern void NMI() ATTR(weak);
extern void reset();

extern word_t __INITIAL_SP;
extern word_t __VECTOR_CHECKSUM;

word_t vector[8] ATTR(section(".vector"))
	= {
	&__INITIAL_SP,
	FPTR(reset),
	FPTR(NMI),
	FPTR(hardfault),
	0,
	0,
	0,
	FPTR(&__VECTOR_CHECKSUM)
};

struct GPIO* gpio[GPIO_NUM_PORTS] = {
	(struct GPIO*) GPIO_PORT_0,
	(struct GPIO*) GPIO_PORT_1,
	(struct GPIO*) GPIO_PORT_2,
	(struct GPIO*) GPIO_PORT_3
};
