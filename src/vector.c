#define FUNC_ADDR(x) ((unsigned)(x) + 1)

extern void hardfault() __attribute__((weak, alias("default_hardfault")));									  
extern void NMI() __attribute__((weak, alias("default_NMI")));
								
extern void reset();

extern unsigned __INITIAL_SP;
extern unsigned __VECTOR_CHECKSUM;

#define DEFAULTIRQ __attribute__((weak, alias("default_irq")))

extern void IRQ0() DEFAULTIRQ;
extern void IRQ1() DEFAULTIRQ;
extern void IRQ2() DEFAULTIRQ;
extern void IRQ3() DEFAULTIRQ;
extern void IRQ4() DEFAULTIRQ;
extern void IRQ5() DEFAULTIRQ;
extern void IRQ6() DEFAULTIRQ;
extern void IRQ7() DEFAULTIRQ;
extern void IRQ8() DEFAULTIRQ;
extern void IRQ9() DEFAULTIRQ;
extern void IRQ10() DEFAULTIRQ;
extern void IRQ11() DEFAULTIRQ;
extern void IRQ12() DEFAULTIRQ;
extern void IRQ13() DEFAULTIRQ;
extern void IRQ14() DEFAULTIRQ;
extern void IRQ15() DEFAULTIRQ;
extern void IRQ16() DEFAULTIRQ;
extern void IRQ17() DEFAULTIRQ;
extern void IRQ18() DEFAULTIRQ;
extern void IRQ19() DEFAULTIRQ;
extern void IRQ20() DEFAULTIRQ;
extern void IRQ21() DEFAULTIRQ;
extern void IRQ22() DEFAULTIRQ;
extern void IRQ23() DEFAULTIRQ;
extern void IRQ24() DEFAULTIRQ;
extern void IRQ25() DEFAULTIRQ;
extern void IRQ26() DEFAULTIRQ;
extern void IRQ27() DEFAULTIRQ;
extern void IRQ28() DEFAULTIRQ;
extern void IRQ29() DEFAULTIRQ;
extern void IRQ30() DEFAULTIRQ;
extern void IRQ31() DEFAULTIRQ;

void default_irq() {
}

extern void __irq_generic();
extern void __init_system();

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
	FUNC_ADDR(__irq_generic), /* SysTick interrupt */
	
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
	0, /* sys-tick */
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

void default_hardfault() {
	while (1) {
		asm("nop");
	}
}

void default_NMI() {
	
}

