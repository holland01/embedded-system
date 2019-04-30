#define FUNC_ADDR(x) ((unsigned)(x) + 1)

extern void hardfault() __attribute__((weak, alias("default_hardfault")));									  
extern void NMI() __attribute__((weak, alias("default_NMI")));
								
extern void reset();

extern unsigned __INITIAL_SP;
extern unsigned __VECTOR_CHECKSUM;

#define DEFAULTIRQ __attribute__((weak, alias("default_handler")))

void IRQ0() DEFAULTIRQ;
void IRQ1() DEFAULTIRQ;
void IRQ2() DEFAULTIRQ;
void IRQ3() DEFAULTIRQ;
void IRQ4() DEFAULTIRQ;
void IRQ5() DEFAULTIRQ;
void IRQ6() DEFAULTIRQ;
void IRQ7() DEFAULTIRQ;
void IRQ8() DEFAULTIRQ;
void IRQ9() DEFAULTIRQ;
void IRQ10() DEFAULTIRQ;
void IRQ11() DEFAULTIRQ;
void IRQ12() DEFAULTIRQ;
void IRQ13() DEFAULTIRQ;
void IRQ14() DEFAULTIRQ;
void IRQ15() DEFAULTIRQ;
void IRQ16() DEFAULTIRQ;
void IRQ17() DEFAULTIRQ;
void IRQ18() DEFAULTIRQ;
void IRQ19() DEFAULTIRQ;
void IRQ20() DEFAULTIRQ;
void IRQ21() DEFAULTIRQ;
void IRQ22() DEFAULTIRQ;
void IRQ23() DEFAULTIRQ;
void IRQ24() DEFAULTIRQ;
void IRQ25() DEFAULTIRQ;
void IRQ26() DEFAULTIRQ;
void IRQ27() DEFAULTIRQ;
void IRQ28() DEFAULTIRQ;
void IRQ29() DEFAULTIRQ;
void IRQ30() DEFAULTIRQ;
void IRQ31() DEFAULTIRQ;

void default_handler() {
	
}

unsigned vector[40] __attribute__((section(".vector"))) = {
	(unsigned)&__INITIAL_SP,

	FUNC_ADDR(reset),
	FUNC_ADDR(NMI),
	FUNC_ADDR(hardfault),

	0,
	0,
	0,

	(unsigned)&__VECTOR_CHECKSUM,
	
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

