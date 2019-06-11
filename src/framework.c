#include "lpc1114.h"
#include "framework.h"
//#include "i2c.h"
//#include "ssd1306.h"

/*
 * Macros
 */

#define disable_int asm volatile("CPSID i")
#define enable_int asm volatile("CPSIE i")

/*
 * External dependencies
 */

extern void __reset() __attribute__((section(".text")));

extern unsigned __DATA_LMA;
extern unsigned __DATA_END;
extern unsigned __DATA_VMA;

extern unsigned __BSS_VMA;
extern unsigned __BSS_END;

extern void* __THREADS_START;
extern void* __THREADS_END;

/* 
 * Internal global variables 
 */

THREAD(__main__, 0, 64, 0, 0, 0, 0);

/* 
 * Global variables 
 */

thread_t* CURCTX = NULL;
thread_t* RUNLIST = NULL;
thread_t* FREELIST = NULL;

volatile unsigned __PSP = 0;

/*
 * Internal function definitions
 */

/*
 * Set Phase-Lock-Loop Control
 *
 * Just a utility function that ensures 
 * SYSPLLCTRL MSEL and PSEL bitfields
 * are set properly: the RESERVED
 * bitfield must also be explicitly set
 * to 0, otherwise the compiler will write
 * a 1 to one of its bits.
 */
static inline void set_pll_ctrl(unsigned MSEL, unsigned PSEL) {
  SYSCON.SYSPLLCTRL.MSEL = MSEL;
  SYSCON.SYSPLLCTRL.PSEL = PSEL;

  SYSCON.SYSPLLCTRL.RESERVED = 0; 
}

/* 
 * Debug-Hang
 *
 * Nothing special: this is used
 * occasionally to pause on startup.
 * There have been times when 
 * "monitor reset halt" hasn't
 * worked as intended in gdb,
 * for whatever reason.
 */
static inline void debug_hang(int count) {
  volatile int ccount = count * 1000000;
  volatile int i = 0;
  
  while (i < ccount) {
    i++;
  }
}

/*
 * Init-Threads
 *
 * Any threads which have been 
 * defined using the THREAD()
 * macro (apart from the __main__ thread)
 * will be appended to RUNLIST,
 * which will be iterated over
 * during interrupts.
 */

static void init_threads() {  
  volatile unsigned thd_end = (unsigned)&__THREADS_END;
  volatile unsigned thd_iter = (unsigned)&__THREADS_START;
  
  while (thd_iter < thd_end) {
    volatile void** as_double_p = (volatile void**) thd_iter;   
    volatile thread_t* thd = (volatile thread_t*)(*as_double_p);
    
    if (thd != &__main__.thread) {
      thd->sp = (unsigned)thd + 8 + (48 * 4);
      
      thread_append(&RUNLIST, (thread_t*)thd);
    }

    thd_iter += 4;
  }
}

/*
 * Setup-Data-And-Bss
 *
 * Just as it says: transfer
 * the data segment from its
 * flash memory over to sram,
 * and ensure that the BSS segment is set to zero.
 */

static void setup_data_and_bss() {
  {
    unsigned* from = &__DATA_LMA; 
    unsigned* to = &__DATA_VMA;
    unsigned* end = &__DATA_END;
  
    while (from != end) {
      *to = *from;
      to++;
      from++;
    }
  }

  {
    unsigned* bss = &__BSS_VMA;
    unsigned* bss_end = &__BSS_END;

    while (bss != bss_end) {
      *bss = 0;
      bss++;
    }
  }
}

/*
 * Functions declared in framework.h
 *
 */

/*
 * Initialize-System
 */

void __init_system() {  
  setup_data_and_bss();
  
  CURCTX = (thread_t*)&__main__.thread;
  __PSP = __main__.thread.sp;

  //  init_threads();
  __reset();
}

/*
 * System-Tick-Timer-On
 * 
 * SYST.RVR is set to 10 milliseconds for a 48 mhz
 * CPU.
 * 
 * We clear out the counter register, CVR,
 * which will fire the interrupt
 * when the systick timer is enabled.
 *
 * Bit 0 of SYST.CSR enables the systic timer.
 * Bit 1 of SYST.CSR enables the actual interrupt.
 * Bit 2 of SYST.CSR ensures our clock is the system clock.
 */

void systick_on() {
  SYST.RVR |= (48000 * 10) - 1; 

  SYST.CVR &= ~((1 << 24) - 1);

  SYST.CSR |= 1 << 0; 
  SYST.CSR |= 1 << 1; 
  SYST.CSR |= 1 << 2;
}

extern unsigned HARDFAULT_CODE;

#define L(x) HARDFAULT_CODE = x

/*
 * Setup
 *
 * We set bit 6 to 1
 * of SYSCON.SYSAHBCLKCTRL
 * to enable the GPIO clock.
 *
 * We set PIO_1, PIO_2
 * to be outputs for GPIO0, via GPIO0.DIR.
 *
 * We set each of those to low (off), via GPIO0.DATA.
 *
 * Then set both IOCON_PIO0_1 and IOCON_PIO0_2
 * registers to ensure the settings for their
 * respective pins are configured properly.
 *
 * The memory map for both of these registers is idential.
 * (i.e., the following list applies to both)
 *
 * - &= ~0x3: zeros out bits [2:0]
 *   a value of 0 for bits [2:0] indicates that the 
 *   FUNC the pin is to be set to "PIO0".
 *
 * - &= ~((1 << 3) | (1 << 4)): zeros out bits [4:3]
 *   a value of 0 for bits [4:3] indicates that the 
 *   MODE for the pin is to be set to "inactive".
 *   
 * - &= ~(1 << 5): zeros out bit [5]
 *   a value of 0 for bit [5] indicates that the 
 *   HYS (for "hysteresis") be set to "disabled".
 *
 * - &= ~(1 << 10): zeros out bit [10]
 *   a value of 0 for bit [10] indicates that the
 *   OD (for "psuedo open-drain mode)" for the pin
 *   is set to "standard GPIO output"
 *
 * Finally, we turn on the systick timer via
 * systick_on()
 */

void setup() {
  L(0);
  
  setup_pll();

  SYSCON.SYSAHBCLKCTRL |= 1 << 6;
  
#if 0
  GPIO0.DIR |= PIO_1 | PIO_2;

  GPIO0.DATA[PIO_1 | PIO_2] = 0;
 
  IOCON_PIO0_1 &= ~0x3;
  IOCON_PIO0_1 &= ~((1 << 3) | (1 << 4));
  IOCON_PIO0_1 &= ~(1 << 5);
  IOCON_PIO0_1 &= ~(1 << 10);

  IOCON_PIO0_2 &= ~0x3;
  IOCON_PIO0_2 &= ~((1 << 3) | (1 << 4));
  IOCON_PIO0_2 &= ~(1 << 5);
  IOCON_PIO0_2 &= ~(1 << 10);
#endif
  // --- SCREEN
#if 0
  I2C_init();
  
  SSD1306_init();
  SSD1306_clear_screen();

  const char* letters = "Hello World";

  SSD1306_set_col_range(0, strlen(letters) * 6);
  SSD1306_set_page_range(0, 4);
  
  SSD1306_write_text(letters);
#endif
  // --- ADC

  L(1);
  setup_iocon();

  L(2);
  setup_adc();

  L(3);
  setup_timer();

  L(4);
  enable_ints();

  L(5);
  GPIO0.DATA[PIO_8] = 0;
}

/*
 * Loop
 *
 * Currently a dead function;
 * called by reset()...maybe,
 * assuming the interrupt isn't
 * fired before then. Either way,
 * it only gets called once,
 * since after the interrupt is 
 * fired, only threads initially
 * on the run list will be executed.
 */

void loop() {
  asm("wfi");
}

/* Setup-PLL
 * 
 * Sets the main system clock to receive power 
 * from the system's phase lock loop,
 * so we can boost the CPU frequency from 12 MHZ to 48 MHZ
 */

void setup_pll() {
  SYSCON.SYSPLLCLKSEL = SYSCON_SYSPLLCLK_IRC;

  SYSCON.PDRUNCFG &= ~SYSCON_PDRUNCFG_SYSPLL_OFF; 
  
  set_pll_ctrl(3, 1);
  
  SYSCON.SYSPLLCLKUEN = 1;
  
  while (!SYSCON.SYSPLLSTAT)
    asm("");

  SYSCON.MAINCLKSEL = SYSCON_MAINCLKSEL_PLL;
  SYSCON.MAINCLKUEN = 1;
}

/* 
 * Setup-IOCON
 *
 * We use the IOCON registers to set the functionality
 * of Pins 4 and 1. Pin 4 involves the ADC input (PIO0_11),
 * and Pin 1 is used as the output to the servo motor.
 */

void setup_iocon() {
  SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_IOCON_ON;

  IOCON_R_PIO0_11 = IOCON_R_PIO0_11_SET_AD0_INPUT;

  IOCON_PIO0_8 &= ~((1 << 11) - 1); /* [2:0] FUNC = CT16B0_MAT0 */
  IOCON_PIO0_8 |= 0x2;
}

/*
 * Setup-ADC
 *
 * Set the appropriate input channel for the ADC converter (0);
 * set the clock divider, which is necessary for ensuring
 * that samples are converted at an appropriate rate (the rate
 * must be 4.5 MHZ or below, so we use 11 since 48 MHZ / 11 
 * is within that range); we don't use Burst mode, and we 
 * only use the global DONE flag to trigger an interrupt.
 */

void setup_adc() {
  SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_ADC_ON;
  SYSCON.PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_OFF;

  ADC.CR = ADC_CR_SEL_SET_CHANNEL(0); 
  ADC.CR = ADC_CR_CLKDIV_SET_VALUE(11);
  ADC.CR = ADC_CR_BURST_SET_OFF;
  ADC.CR = ADC_CR_CLKS_SET_11_CLK_10_BIT;
  ADC.CR = ADC_CR_START_SET_NO_START;

  ADC.INTEN = ADC_INTEN_SET_ADGINTEN_ONLY;
}

/* 
 * Enable-interrupts
 *
 * Enables IRQ24 and IRQ16.
 * IRQ24 is used to handle ADC conversion,
 * and IRQ16 is fired at the start of every
 * 20 millisecond cycle 
 */

void enable_ints() {
  asm volatile ("CPSIE i");

  ISER = ISER_IRQ24_ENABLED; 
  ISER = ISER_IRQ16_ENABLED;
}

/*
 * Setup-Timer
 *
 * Uses match register 0 to serve as a pulse width modulated
 * output on pin 1. Match register 1 represents a 20 millisecond
 * cycle that resets the timer counter each time the end of the cycle
 * is hit. On every cycle, IRQ16 is fired (which in turn marks the beginning
 * of the analog to digital conversion process)
 */

void setup_timer() {
  SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_CT16B0_ON;

  SET_LOW_16(TMR16B0.PR, 48); 
  SET_LOW_16(TMR16B0.TC, 0);
  SET_LOW_16(TMR16B0.PC, 0);
  SET_LOW_16(TMR16B0.MR1, 20000); 

  TMR16B0.MCR = TMR16B0_MCR_ENABLE_MR1_I;
  TMR16B0.MCR = TMR16B0_MCR_ENABLE_MR1_R;
  
  TMR16B0.PWMC = TMR16B0_PWMC_ENABLE_MR0;
  
  TMR16B0.TCR |= 2; /* reset timer */
  TMR16B0.TCR &= ~0x3;
  TMR16B0.TCR |= 0x1; /* enable counter */
}

/*
 * IRQ16
 *
 * Triggered at the beginning of every Match 1 register cycle (every 20 milliseconds).
 * We start the ADC sample process here.
 */
void IRQ16() {
  TMR16B0.IR |= (1 << 1); /* set MR1 to HIGH */
  ADC.CR |= (1 << 24); /* [26:24] START = Start conversion now */
  
  GPIO1.DATA[PIO_9] = 0;
  GPIO0.DATA[PIO_8] = 0;
}

/*
 * SysTick-Schedule
 *
 * Interrupted by SysTick,
 * every 10 milliseconds
 * we provide the context
 * for the next thread
 */

void systick_schedule() {
  disable_int;

  if (RUNLIST != NULL) {    
    thread_t* next = thread_next(&RUNLIST);
    CURCTX = next;
    thread_append(&FREELIST, next);
    
  } else {
    
    RUNLIST = FREELIST;
    FREELIST = NULL;

    CURCTX = RUNLIST;
  }

  enable_int;
}

/*
 * Thread-Append
 */

void thread_append(thread_t** root, thread_t* thd) {
  if (*root != NULL) {
    thread_t* p = *root;
    thd->next = NULL;

    while (p->next != NULL) {
      p = p->next;
    }

    p->next = thd;
  } else {
    *root = thd;
  }
}

/*
 * Thread-Next
 *
 * This may or may not be obvious 
 * (at least at first glance):
 * it's very important that 
 * *root = (*root)->next is executed
 * before k->next = NULL,
 * since before then k and *root
 * point to the same location :^)
 */

thread_t* thread_next(thread_t** root) {
  thread_t* k = NULL;

  if (*root != NULL) {
    k = *root;
    *root = (*root)->next;
    k->next = NULL;
  }
  
  return k;
}

/*
 * IRQ24
 * 
 * The ADC sampling process calls this function every time a sample has completed.
 * We get the VREF, and use that as a primary driver to the servo motor.
 * It's important to note that the manipulations performed on the VREF
 * are designed to allow for a pseudo interpolation process that currently
 * isn't effective as it could be, but it demonstrates well enough that the motor
 * can be turned.
 */

volatile unsigned IRQ24_TICK = 1;

void IRQ24() {
  volatile unsigned DONE = ADC.STAT & 1;

  if (DONE) {
    volatile unsigned VREF = (ADC.R0 >> 6) & 0x3FF;
    volatile unsigned MR1 = GET_LOW_16(TMR16B0.MR1);

#ifdef MODULE_SERVO
    unsigned ROTATE = ((VREF) | 1) * IRQ24_TICK;
    ROTATE &= 0x3ff;
    
    volatile unsigned WIDTH = MR1 - 500 - (ROTATE);
    volatile unsigned TC = GET_LOW_16(TMR16B0.TC);

    SET_LOW_16(TMR16B0.MR0, WIDTH);
#else
    asm("nop");
#endif

    IRQ24_TICK++;
  }
}

/*
 * String-Length
 */

unsigned strlen(const char* str) {
  unsigned count = 0;
  while (*str) {
    str++;
    count++;
  }

  return count;
}
