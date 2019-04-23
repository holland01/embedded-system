#include "common.h"

void sleep(word_t millisec) {
	const word_t TICK = 1000; // 1 millisecond in microseconds
	const word_t STEP = millisec * TICK;

	volatile word_t counter = 0;
	while (counter < STEP) {
		counter++;
	}
}
