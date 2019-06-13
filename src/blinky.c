#include "lpc1114.h"
#include "framework.h"

void blink(int pin, int delay);

//THREAD(one, blink, 64, PIO_1, 1000000, 0, 0);
//THREAD(two, blink, 64, PIO_2,  660000, 0, 0);

/* This is the function body for both threads.
 */
void blink(int pin, int delay) {
  while(1) {
    GPIO0.DATA[pin] ^= ~0;
    for(int i = 0; i < delay; ++i) {}
  }
}
