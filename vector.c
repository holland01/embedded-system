#define ATTR(s) __attribute__((s))
#define THUMB(x) ((unsigned)(x) + 1)

extern void hardfault() ATTR(weak);
extern void NMI() ATTR(weak);
extern void reset();

extern unsigned __INITIAL_SP;
extern unsigned __VECTOR_CHECKSUM;

unsigned int vector[8] ATTR(section(".vector"))
	= {
	THUMB(&__INITIAL_SP),
	THUMB(reset),
	THUMB(NMI),
	THUMB(hardfault),
	0,
	0,
	0,
	THUMB(&__VECTOR_CHECKSUM)
};
