#include "common.h"
#include "gpio.h"

extern word_t __DATA_LMA;
extern word_t __DATA_END;
extern word_t __DATA_VMA;

extern word_t __BSS_VMA;
extern word_t __BSS_END;

extern void setup() __attribute__((weak));
extern void loop();

void reset() {

	// copy over data segment
	{
		word_t* from = &__DATA_LMA; // location in flash
		word_t* to = &__DATA_VMA; // location in sram
		word_t* end = &__DATA_END;
	
		while (from != end) {
			*to = *from;
			to++;
			from++;
		}
	}

	// zero out bss
	{
		word_t* bss = &__BSS_VMA;
		word_t* bss_end = &__BSS_END;

		while (bss != bss_end) {
			*bss = 0;
			bss++;
		}
	}
	
	if (setup) { 
		setup();
	}
	
	while (True) {
		loop();
	}
}
