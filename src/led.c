#include "led.h"
#include "gpio.h"
#include "common.h"

enum /* morse tick values in milliseconds */ {
	MT_DOT = 250,
	MT_DASH = 750,
	MT_INTER_DOT = 250,
	MT_INTER_WORD = 1750,
	MT_INTER_CHAR = 750
};

void led_on() {
	GPIO[1]->data[GPIO_PIN_9] = GPIO_PIN_9;
}

void led_off() {
	GPIO[1]->data[GPIO_PIN_9] = 0;
}

static void blink_morse_command(char c) {
    switch (c) {
	case 'H':
		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);
		break;
		
	case 'E':
		led_on();
		sleep(MT_DOT);
		
		led_off();
		sleep(MT_INTER_DOT);
		break;
	
	case 'L':
		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DASH);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);
		
		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);
		break;

	case 'O':
		led_on();

		sleep(MT_DASH);
		sleep(MT_DASH);
		sleep(MT_DASH);

		led_off();
		break;

	case 'W':
		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DASH);
		sleep(MT_DASH);

		led_off();
		break;

	case 'R':
		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DASH);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);
		break;

	case 'D':
		led_on();
		sleep(MT_DASH);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);

		led_on();
		sleep(MT_DOT);

		led_off();
		sleep(MT_INTER_DOT);
		break;
		
	case '_':
		sleep(MT_INTER_CHAR);
		break;

	case ' ':
		sleep(MT_INTER_WORD);
		break;
	}
}

void led_blink_morse(const char* str) {
	const char* p = str;

	while (*p != '\0') {
		blink_morse_command(*p);

		p++;
	}
}
