#ifndef __LPC1114_H__
#define __LPC1114_H__

#define PIO_0 (1 << 0)
#define PIO_1 (1 << 1)
#define PIO_2 (1 << 2)
#define PIO_3 (1 << 3)
#define PIO_4 (1 << 4)
#define PIO_5 (1 << 5)
#define PIO_6 (1 << 6)
#define PIO_7 (1 << 7)
#define PIO_8 (1 << 8)
#define PIO_9 (1 << 9)
#define PIO_10 (1 << 10)
#define PIO_11 (1 << 11)

struct GPIO {
	unsigned DATA[1 << 12];
	unsigned UNUSED[1 << 12];

	unsigned DIR: 11;
	unsigned DIR_R: 0;

	unsigned IS: 11;
	unsigned IS_R: 0;

	unsigned IBE: 11;
	unsigned IBE_R: 0;

	unsigned IEV: 11;
	unsigned IEV_R: 0;

	unsigned IE: 11;
	unsigned IE_R: 0;
} extern GPIO1, GPIO2;

struct IOCON_PIO_8 {
	unsigned FUNC: 3;
	unsigned MODE: 2;
	unsigned HYS : 1;
	unsigned _RESERVED: 4;
	unsigned OD: 1;
} extern IOCON_PIO_8;

struct SYSCON {
	unsigned SYSMEMREMAP;

	unsigned PRESETCTRL;

	struct {
		unsigned MSEL : 5:
		unsigned PSEL : 2;
		unsigned UNUSED : 0;
	} SYSPLLCTRL;

	unsigned SYSPLLSTAT: 1;
	unsigned SYSPLLSTAT_R: 0;

	unsigned RESERVED0[4];
	
	unsigned SYSOSCCTRL;
	unsigned WDTOSCCTRL;
	unsigned IRCCTRL;
	
	unsigned RESERVED1;

	unsigned SYSRSTSTAT;
	
	unsigned RESERVED2[3];
		
	unsigned SYSPLLCLKSEL: 2;
	unsigned SYSPLLCLKSEL_R: 0;

	unsigned SYSPLLCLKUEN: 1;
	unsigned SYSPLLCLKUEN_R: 0;
	
	unsigned MAINCLKSEL:2;
	unsigned MAINCLKSEL_R: 0;
	
	unsigned MAINCLKUEN: 1;
	unsigned MAINCLKUEN_R: 0;

	unsigned SYSAHBCLKDIV;

	unsigned RESERVED3;
	
	struct {
		unsigned UNUSED0 : 7;
		unsigned CT16B0 : 1;
		unsigned UNUSED1 : 7;
		unsigned IOCON: 1;
		unsigned UNUSED2: 0;
	} SYSAHBCLKCTRL;

	unsigned RESERVED4[4];
	
	unsigned SSP0CLKDIV;
	unsigned UARTCLKDIV;
	unsigned SSP1CLKDIV;

	unsigned RESERVED5[12];

	unsigned WDTCLKSEL;
	unsigned WDTCLKUEN;
	unsigned WDTCLKDIV;

	unsigned RESERVED6;

	unsigned CLKOUTCLKSEL;
	unsigned CLKOUTUEN;
	unsigned CLKOUTCLKDIV;

	unsigned RESERVED7[5];

	unsigned PIOPORCAP0;
	unsigned PIOPORCAP1;

	unsigned RESERVED8[18];

	unsigned BODCTRL;
	unsigned SYSTCKCAL;

	unsigned RESERVED9[6];

	unsigned IRQLATENCY;
	unsigned NMISRC;

	unsigned RESERVED10[34];

	unsigned STARTAPRP0;
	unsigned STARTERP0;

	unsigned STARTRSPR0CLR;
	unsigned STARTSRP0;

	unsigned RESERVED11[8];

	unsigned PDSLEEPCFG;
	unsigned PDAWAKECFG;
	
	struct {
		unsigned UNUSED: 6;
		unsigned SYSPLL: 1;
		unsigned _R: 0; 
	} PDRUNCFG:

	unsigned RESERVED12[110];

	unsigned DEVICE_ID;
} extern SYSCON;

extern unsigned ISER;

#endif // __LPC1114_H__
