#include "lpc1114.h"

#ifdef MODULE_BLINK

void setup() {
	GPIO1.DIR |= PIO_9;
}

void loop() {
	GPIO1.DATA[PIO_9] ^= PIO_9;
	
	for (int i = 0; i < 1000000; ++i) {
		asm("");
	}
}

#endif
