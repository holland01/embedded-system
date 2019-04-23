#include "gpio.h"
#include "led.h"

void setup() {
	GPIO[0] = (struct GPIO_CTRL*) GPIO_PORT_0;
	GPIO[1] = (struct GPIO_CTRL*) GPIO_PORT_1;
	GPIO[2] = (struct GPIO_CTRL*) GPIO_PORT_2;
	GPIO[3] = (struct GPIO_CTRL*) GPIO_PORT_3;

	// set pin 9 as output
	GPIO[1]->io_dir |= GPIO_PIN_9;
}

void loop() {
	led_blink_morse("HELLO WORLD");
	/*
	led_on();
	sleep(1000);
	led_off();
	sleep(3000);
	*/
}
