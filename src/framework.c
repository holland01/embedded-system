#include "lpc1114.h"
#include "framework.h"

#include "i2c.h"
#include "ssd1306.h"

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
 * see vector.c
 */
extern unsigned HARDFAULT_CODE;
extern volatile char* HARDFAULT_MSG;

/*
 * Defined here, but declared so it can be referenced via 
 * thread macro invoke.
 */
extern void adc_thread();

/*
 * This is the only lock that's used
 */

bsem_t LOCK = { false };

thread_t* CURCTX = NULL;

volatile unsigned __PSP = 0;

/*
 * Really only used for debugging
 */

static volatile unsigned SCHEDULE_COUNT = 0;

static thread_list_t RUNLIST = { NULL, NULL };
static thread_list_t FREELIST = { NULL, NULL };

/*
 * Thread declarations.
 * These are the only threads we use.
 */

THREAD(__main__, 0, 64, 0, 0, 0, 0);
THREAD(display, SSD1306_display_thread, 128, 0, 0, 0, 0);
THREAD(adc, adc_thread, 128, 0, 0, 0, 0);

/*
 * Millisecond-Sleep
 * 
 * 1 / 48_000_000 ~= 20e-9
 * so, 20 nanoseconds per clock cycle
 * 13 clock cycles per iteration => 20 * 13 = 260 nanoseconds per iteration
 * msec * 1e6 / 260 = iterations
 */

void msleep(unsigned msec) {
  unsigned iterations = (unsigned)((double)msec * 1.0e6 / 260.0);
  volatile unsigned i = 0;
  while (i < iterations) {
    i++;
  }
}

/*
 * Kept as a struct since it went through
 * several iterations
 */

volatile struct {
  bool needs_reset;
} static SYSTICK_STATE = {
  false
};



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
      thread_list_append(&RUNLIST, (thread_t*)thd);
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
 * Initialize-System
 */

void __init_system() {  
  setup_data_and_bss();
  
  CURCTX = (thread_t*)&__main__.thread;
  __PSP = __main__.thread.sp;

  init_threads();
  __reset();
}



/*
 * Setup
 *
 * We set bit 6 to 1
 * of SYSCON.SYSAHBCLKCTRL
 * to enable the GPIO clock.
 *
 * Then we initialize the I2C bus,
 * init and clear the SSD1306 screen
 * (we can do this without locking since
 * the threads haven't actually been initialized yet),
 * and setup the ioconfig/adc registers needed for analog
 * to digital conversion.
 *
 * Finally, we turn on the systick timer via
 * systick_reset()
 */

void setup() {
  setup_pll();

  SYSCON.SYSAHBCLKCTRL |= 1 << 6;
  
  // --- SCREEN
  I2C_init();
  
  SSD1306_init();
  SSD1306_clear_screen();

  // --- ADC
  setup_iocon();
  setup_adc();
  
  systick_reset();
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

/*
 * Analog-To-Digital-Conversion Thread
 *
 * Continuously samples resistance
 * off of AD0 and sends the value to the
 * SSD1306_display_thread for printing.
 *
 * While the msleep() call implies a 1000 millisecond
 * delay, it's worth noting that, technically,
 * the amount of time it takes to finish
 * is 1000 * number_of_threads,
 * so for this to be truly 1000 milliseconds
 * it's necessary to have a more sophisticated timing
 * mechanism.
 */

void adc_thread() {
  while (1) {
    
    ADC.CR |= (1 << 24); // start ADC conversion ([26:24])

    
    while (ADC.R0 < 0x7FFFFFFF) { // wait until sample is finished
      asm("");
    }

    volatile unsigned VREF = (ADC.R0 >> 6) & 0x3FF;
    volatile unsigned VREF2 = (ADC.R0 & 0xFFC0) >> 8;

    volatile double v_in = 3.3;
    volatile double v_out = 3.3 * (double)(VREF) / 1024.0;

    volatile double R = (v_out + 470) / (v_in - v_out);

    bsem_enter(&LOCK);
    
    SSD1306_clear_screen();
    SSD1306_print_num(R);

    bsem_leave(&LOCK);

    msleep(1000);
  }

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
 * use a separate thread instead of an interrupt for ADC
 * handling.
 */

void setup_adc() {
  SYSCON.SYSAHBCLKCTRL |= SYSCON_SYSAHBCLKCTRL_ADC_ON;
  SYSCON.PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_OFF;

  ADC.CR = ADC_CR_SEL_SET_CHANNEL(0); 
  ADC.CR |= (10 << 8);

  ADC.CR = ADC_CR_BURST_SET_OFF;
  ADC.CR = ADC_CR_CLKS_SET_11_CLK_10_BIT;
  ADC.CR = ADC_CR_START_SET_NO_START;
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

  if (thread_list_empty(&RUNLIST)) {
    RUNLIST.head = FREELIST.head;
    RUNLIST.tail = FREELIST.tail;
    
    FREELIST.head = NULL;
    FREELIST.tail = NULL;
  }

  CURCTX = thread_list_next(&RUNLIST);
  
  thread_list_append(&FREELIST, CURCTX);

  SCHEDULE_COUNT++;

  if (SYSTICK_STATE.needs_reset) {
    systick_reset();
  }
  
  enable_int;
}

/*
 * Thread-List-Empty
 */

bool thread_list_empty(thread_list_t* runlist) {
  bool ret = false;
  if (runlist->head == NULL) {
    assert(runlist->tail == NULL, "thread_list_empty");
    ret = true;
  }
  return ret;
}

/*
 * Thread-List-Append
 */

void thread_list_append(thread_list_t* runlist, thread_t* thd) {
  if (runlist->head == NULL) {
    assert(runlist->tail == NULL, "thread_append");
    
    runlist->head = thd;
    runlist->head->next = NULL;
  } else {
    if (runlist->tail == NULL) {
      runlist->tail = thd;
      runlist->head->next = runlist->tail;
    } else {
      runlist->tail->next = thd;
      runlist->tail = thd;
    }
  }
}

/*
 * Thread-List-Next
 */

static volatile unsigned TLN_COUNT = 0;

thread_t* thread_list_next(thread_list_t* runlist) {
  TLN_COUNT++;
  thread_t* ret = NULL;
  
  if (runlist->head != NULL) {
    thread_t* n = runlist->head->next;
    
    ret = runlist->head;

    runlist->head = n;
  }

  if (runlist->head == runlist->tail) {
    runlist->tail = NULL;
  }

  if (ret != NULL) {
    ret->next = NULL;
  }

  return ret;
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

/*
 * Assert
 */

void assert(bool cond, char* msg) {
  if (!cond) {
    HARDFAULT_MSG = msg;
    void** x = NULL;
    *x;
  }
}


/*
 * System-Tick-Timer-Force-Switch
 */

void systick_force_switch() {
  if (!SYSTICK_STATE.needs_reset) {
    SYST.CSR &= ~0x1; // disable counter
    SYST.CSR &= ~0x2; // disable interrupt

    SYST.CVR &= ~((1 << 24) - 1); // clear out counter
  
    SYST.RVR |= 100; // bogus value to force switch

    SYST.CSR |= 0x2; // enable interrupt
    SYST.CSR |= 0x1; // enable counter

    SYSTICK_STATE.needs_reset = true; 
  }
}

/*
 * System-Tick-Timer-Reset
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

void systick_reset() {
  SYST.RVR |= (48000 * 10) - 1; 

  SYST.CVR &= ~((1 << 24) - 1);

  SYST.CSR |= 1 << 0; 
  SYST.CSR |= 1 << 1; 
  SYST.CSR |= 1 << 2;
}

/*
 * Binary-Semaphore-Enter
 */

void bsem_enter(bsem_t* b) {
  while (b->locked) {
    systick_force_switch();
  }

  b->locked = true;
}

/*
 * Binary-Semaphore-Leave
 */

void bsem_leave(bsem_t* b) {
  b->locked = false;
}
