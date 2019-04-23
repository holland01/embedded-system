#include "common.h"
#include "gpio.h"

static void setup() {
	// set GPIO values
	GPIO[0] = (struct GPIO_CTRL*) GPIO_PORT_0;
	GPIO[1] = (struct GPIO_CTRL*) GPIO_PORT_1;
	GPIO[2] = (struct GPIO_CTRL*) GPIO_PORT_2;
	GPIO[3] = (struct GPIO_CTRL*) GPIO_PORT_3;
}

void reset() {
	setup();
	
	// set pin 9 as output
	GPIO[1]->io_dir |= GPIO_PIN_9;
	
	while (True) {
		led_blink_morse("HELLO WORLD");
		sleep(5000);
	}
}
