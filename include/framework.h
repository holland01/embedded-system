#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

/*
 * TODO: .init_array is currently unused; it will
 * be implemented soon.
 */

typedef void (*ctor_t)();

extern const ctor_t* __INIT_ARRAY_START;
extern const ctor_t* __INIT_ARRAY_END;

typedef struct thread thread_t;

struct thread {
  thread_t* next;
  unsigned sp;
};

extern thread_t* CURCTX;

/*
 * TODO:
 *
 * The stack area for threads defined using this macro
 * currently won't actually be used; as a result,
 * it's necessary to compile this project using -O1.
 *
 * This will be fixed soon.
 */

#define THREAD(name, function, stacksize, arg0, arg1, arg2, arg3)     \
volatile struct {                                                     \
  thread_t thread;                                                    \
  unsigned stack[(stacksize) - 16];                                   \
  unsigned r8;                                                        \
  unsigned r9;                                                        \
  unsigned r10;                                                       \
  unsigned r11;                                                       \
  unsigned r4;                                                        \
  unsigned r5;                                                        \
  unsigned r6;                                                        \
  unsigned r7;                                                        \
  unsigned r0;                                                        \
  unsigned r1;                                                        \
  unsigned r2;                                                        \
  unsigned r3;                                                        \
  unsigned r12;                                                       \
  unsigned lr;                                                        \
  unsigned pc;                                                        \
  unsigned psr;                                                       \
  } static name = {                                                     \
  { NULL, (unsigned)(&(name.r8)) },                                     \
  { 0 },                                                                \
  0, 0, 0, 0,                                                           \
  0x33, 0x22, 0x11, 0,                                                  \
  (arg0), (arg1), (arg2), (arg3),                                       \
  0, 0,                                                                 \
  (unsigned)((function) + 1),                                           \
  (1 << 24)                                                             \
};                                                                      \
volatile void * const __##name##__ptr __attribute__((section(".threads"))) = &name;

extern volatile unsigned __PSP;

/*
 * Initialize-System
 *
 * Our true entry point.
 * Here we ensure the data
 * and bss segments are 
 * properly handled;
 * set a starting thread
 * context with CURCTX;
 * and the __PSP global
 * that points to the initial
 * stack pointer, which is later
 * set to the actual psp register
 * via __reset().
 *
 * Once __reset() is finished,
 * we call reset(), which calls
 * a user defined "setup()" function.
 */

void __init_system();

/*
 * System-Tick-Timer-On
 *
 * Used to initialize the system-tick-timer
 * interrupt to a 10ms interval. The interrupt is 
 * used to perform thread context
 * switching. The interrupt code
 * is defined in systick_schedule().
 */

void systick_on();

/*
 * Setup
 *
 * This is called by reset() (defined in reset.c).
 * Sets the system clock to 48 MHZ using the PLL.
 * Currently, anything else this does (GPIO setup, etc.)
 * is subject to change, however details for those
 * arbitrary specifics are provided in the definition.
 */

void setup();

/*
 * Loop 
 *
 * Not required,
 * but if defined will be 
 * continuously
 * called in an iterative loop
 * by the reset() function
 *
 * (at the time of this writing,
 * it's somewhat a mixed bag because
 * reset() essentially will be blown
 * out of the picture via
 * thread context switches, which
 * will never return to the reset()
 * function)
 */

void loop();

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
 * Setup-ADC
 *
 * Initializes any necessary analog to digital conversion
 * components
 */

void setup_adc();

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

/*
 * String-Length
 *
 * Counts each character until a null terminator
 * is hit and returns the count
 */

unsigned strlen(const char* str);

#endif // __FRAMEWORK_H__
