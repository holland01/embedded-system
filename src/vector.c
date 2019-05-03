#define FUNC_ADDR(x) ((unsigned)(x) + 1)

extern void hardfault() __attribute__((weak, alias("default_hardfault")));									  
extern void NMI() __attribute__((weak, alias("default_NMI")));
								
extern void reset();

extern unsigned __INITIAL_SP;
extern unsigned __VECTOR_CHECKSUM;

#define DEFAULTIRQ __attribute__((weak, alias("default_handler")))


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


void default_handler() {
}


unsigned vector[48] __attribute__((section(".vector"))) = {
	(unsigned)&__INITIAL_SP,

	FUNC_ADDR(reset),
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
	0,
	
	FUNC_ADDR(IRQ0),
	FUNC_ADDR(IRQ1),
	FUNC_ADDR(IRQ2),
	FUNC_ADDR(IRQ3),
	FUNC_ADDR(IRQ4),
	FUNC_ADDR(IRQ5),
	FUNC_ADDR(IRQ6),
	FUNC_ADDR(IRQ7),
	FUNC_ADDR(IRQ8),
	FUNC_ADDR(IRQ9),
	FUNC_ADDR(IRQ10),
	FUNC_ADDR(IRQ11),
	FUNC_ADDR(IRQ12),
	FUNC_ADDR(IRQ13),
	FUNC_ADDR(IRQ14),
	FUNC_ADDR(IRQ15),
	FUNC_ADDR(IRQ16),
	FUNC_ADDR(IRQ17),
	FUNC_ADDR(IRQ18),
	FUNC_ADDR(IRQ19),
	FUNC_ADDR(IRQ20),
	FUNC_ADDR(IRQ21),
	FUNC_ADDR(IRQ22),
	FUNC_ADDR(IRQ23),
	FUNC_ADDR(IRQ24),
	FUNC_ADDR(IRQ25),
	FUNC_ADDR(IRQ26),
	FUNC_ADDR(IRQ27),
	FUNC_ADDR(IRQ28),
	FUNC_ADDR(IRQ29),
	FUNC_ADDR(IRQ30),
	FUNC_ADDR(IRQ31)
};

void default_hardfault() {
	while (1) {
		asm("nop");
	}
}

void default_NMI() {
	
}

