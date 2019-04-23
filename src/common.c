#include "common.h"

// 11 cycles per iteration without "register", but iterations are
// still too slow for a reliable 1 microsecond per
// iteration at 12 mhz. using the register keyword
// lowers the cycle count to 5, which seems accurate
// enough.
void sleep(word_t millisec) {
	const word_t TICK = 1000; // 1 millisecond in microseconds
	const word_t STEP = millisec * TICK;

	register word_t counter = 0;

	while (counter < STEP) {
		counter++;
		//	asm("nop");
	}
}
