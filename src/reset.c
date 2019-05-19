#include "lpc1114.h"

/*
 * NOTE:
 * setup() is defined in framework.c (also declared in framework.h)
 */

extern void setup() __attribute__((weak));
extern void loop() __attribute__((weak));

void reset() {	
	if (setup != NULL) { 
		setup();
	}

	if (loop != NULL) {
		while (1) {
			loop();
		}
	}
}
