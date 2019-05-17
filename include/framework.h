#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

/* Setup-PLL
 * 
 * Sets the main system clock to receive power 
 * from the system's phase lock loop,
 * so we can boost the CPU frequency from 12 MHZ to 48 MHZ
 */

void setup_pll();

/*
 * Setup-Timer
 *
 * Uses match register 0 to serve as a pulse width modulated
 * output on pin 1. Match register 1 represents a 20 millisecond
 * cycle that resets the timer counter each time the end of the cycle
 * is hit. On every cycle, IRQ16 is fired (which in turn marks the beginning
 * of the analog to digital conversion process)
 */

void setup_timer();

/* 
 * Setup-IOCON
 *
 * We use the IOCON registers to set the functionality
 * of Pins 4 and 1. Pin 4 involves the ADC input (PIO0_11),
 * and Pin 1 is used as the output to the servo motor.
 */

void setup_iocon();

/* 
 * Enable-interrupts
 *
 * Enables IRQ24 and IRQ16.
 * IRQ24 is used to handle ADC conversion,
 * and IRQ16 is fired at the start of every
 * 20 millisecond cycle 
 */

void enable_ints();
#endif // __FRAMEWORK_H__
