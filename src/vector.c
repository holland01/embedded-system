/*
 * Macros
 */

#define FUNC_ADDR(x) ((unsigned)(x) + 1)

/*
 * External dependencies
 */

extern void hardfault() __attribute__((weak, alias("default_hardfault")));									  
extern void NMI() __attribute__((weak, alias("default_NMI")));
								
extern void reset();

extern unsigned __INITIAL_SP;
extern unsigned __VECTOR_CHECKSUM;

extern void __irq_generic();

/* See framework.h and framework.c for __init_system() */
extern void __init_system();

/* SysTick function called via __irq_generic (see framework.h/framework.c) */
extern void systick_schedule();

/*
 * Vector table
 */

unsigned vector[48] __attribute__((section(".vector"))) = {
	(unsigned)&__INITIAL_SP,

	FUNC_ADDR(__init_system),
	FUNC_ADDR(NMI),
	FUNC_ADDR(hardfault),

	0,
	0,
	0,

	(unsigned)&__VECTOR_CHECKSUM,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	FUNC_ADDR(__irq_generic), /* SysTick interrupt handler */
	
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic),
	FUNC_ADDR(__irq_generic)
};

const unsigned __irq_handlers[48] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	FUNC_ADDR(systick_schedule), 
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

/*
 * Vector table weak symbol defaults
 */

void default_hardfault() {
	while (1) {
		asm("nop");
	}
}

void default_NMI() {
	
}

