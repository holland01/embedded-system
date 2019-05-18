#include "common.h"
#include "lpc1114.h"


extern void setup() __attribute__((weak));
extern void loop();

void reset() {	
	if (setup) { 
		setup();
	}
	
	while (1) {
		loop();
	}
}
