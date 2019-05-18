#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

typedef void (*ctor_t)();

typedef struct thread thread_t;

struct thread {
	thread_t* next;
	unsigned sp;
};

extern const ctor_t* __INIT_ARRAY_START;
extern const ctor_t* __INIT_ARRAY_END;

extern thread_t* CURCTX;

#define THREAD(name, function, stacksize, arg0, arg1, arg2, arg3)			\
struct {																														\
  thread_t thread;																											\
	unsigned stack[(stacksize) - 16];																		\
	unsigned r8;																												\
	unsigned r9;																												\
	unsigned r10;																												\
	unsigned r11;																												\
	unsigned r4;																												\
  unsigned r5;																												\
  unsigned r6;																												\
  unsigned r7;																												\
	unsigned r0;																												\
	unsigned r1;																												\
	unsigned r2;																												\
	unsigned r3;																												\
	unsigned r12;																												\
	unsigned lr;																												\
	unsigned pc;																												\
	unsigned psr;																												\
	} static name = {																											\
	{ NULL, (unsigned)(&(name.r8)) },																			\
	{ 0 },																																\
	0, 0, 0, 0,																														\
	0x33, 0x22, 0x11, 0,																									\
	(arg0), (arg1), (arg2), (arg3),																				\
	0, 0,																																	\
	(unsigned)((function) + 1),																						\
	(1 << 24)																															\
};																																			\
void * const __##name##__ptr __attribute__((section(".threads"))) = &name;

extern unsigned __PSP;

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

/*
 * Thread-Append
 * 
 * Append the given thread
 * to the global context list of threads,
 * each of which is scheduled/executed
 * in a round-robin fashion.
 */

void thread_append(thread_t** head, thread_t* thd);

/*
 * Thread-Next
 *
 * Deque the next thread from RUNLIST.
 * If RUNLIST is empty, NULL is returned.
 */

thread_t* thread_next(thread_t** head);

#endif // __FRAMEWORK_H__
