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
	unsigned : 21;

	unsigned IS: 11;
	unsigned : 21;

	unsigned IBE: 11;
	unsigned : 21;

	unsigned IEV: 11;
	unsigned : 21;

	unsigned IE: 11;
	unsigned : 21;

	unsigned RIS;
	unsigned MIS;
	
	unsigned IC: 11;
	unsigned : 21;
} extern GPIO1, GPIO2;

struct IOCON_PIO_8 {
	unsigned FUNC: 3;
	unsigned MODE: 2;
	unsigned HYS : 1;
	unsigned RESERVED: 4;
	unsigned OD: 1;
} extern IOCON_PIO_8;


struct TMR16 {
	struct {
		unsigned MR0 : 1;
		unsigned MR1 : 1;
		unsigned MR2 : 1;
		unsigned MR3 : 1;
		unsigned CR0 : 1;
		unsigned : 27;
	} IR;

	struct {
		unsigned CEn : 1;
		unsigned CRst : 1;
		unsigned : 30;
	} TCR;
	
	unsigned TC : 16;
	unsigned : 16;  

	unsigned PR : 16;
	unsigned : 16;
	
	unsigned PC : 16;
	unsigned : 16;
	
	struct {
		unsigned MR0I : 1;
		unsigned MR0R : 1;
		unsigned MR0S : 1;

		unsigned MR1I : 1;
		unsigned MR1R : 1;
		unsigned MR1S : 1;

		unsigned MR2I : 1;
		unsigned MR2R : 1;
		unsigned MR2S : 1;

		unsigned MR3I : 1;
		unsigned MR3R : 1;
		unsigned MR3S : 1;

		unsigned : 20;
	} MCR;
	
	unsigned MR0 : 16;
	unsigned : 16;
	
	unsigned MR1 : 16;
	unsigned : 16;
	
	unsigned MR2 : 16;
	unsigned : 16;

	unsigned MR3 : 16;
	unsigned : 16;

	struct {
		unsigned CAP0RE : 1;
		unsigned CAP0FE : 1;
		unsigned CAP0I : 1;

		unsigned : 29;
	} CCR;
	
	unsigned CR0 : 16;
	unsigned : 16;

	unsigned EMR;

	unsigned __RESERVED[12];

	unsigned CTCR;
	
	unsigned PWM;
} extern TMR16B0;

struct IOCON_R_PIO0_11 {
	unsigned FUNC: 3;
	unsigned MODE: 2;
	unsigned HYS: 1;
	unsigned __RESERVED0: 1;
	unsigned ADMODE: 1;
	unsigned __RESERVED1: 2;
	unsigned OD:
} extern IOCON_R_PIO_11;

struct SYSCON {
	unsigned SYSMEMREMAP;          // 0

	unsigned PRESETCTRL;           // 4

	//    unsigned SYSPLLCTRL;           // 8

	struct {
		unsigned MSEL : 5;
		unsigned PSEL : 2;
		unsigned RESERVED : 25;
	} SYSPLLCTRL;
	
	unsigned SYSPLLSTAT: 1;        // 12
	unsigned SYSPLLSTAT_R: 31;

	unsigned RESERVED0[4];         // 16, 20, 24, 28
	
	unsigned SYSOSCCTRL;           // 32
	unsigned WDTOSCCTRL;           // 36
	unsigned IRCCTRL;              // 40
	
	unsigned RESERVED1;            // 44

	unsigned SYSRSTSTAT;           // 48
	
	unsigned RESERVED2[3];         // 52, 56, 60
		
	unsigned SYSPLLCLKSEL: 2;      // 64
	unsigned SYSPLLCLKSEL_R: 30;

	unsigned SYSPLLCLKUEN: 1;      // 68
	unsigned SYSPLLCLKUEN_R: 31;

	unsigned __RESERVED3[10];      // 72, 76, 80, 84, 88
                                   // 92, 96, 100, 104, 108
	
	unsigned MAINCLKSEL:2;         // 112
	unsigned MAINCLKSEL_R: 30;
	
	unsigned MAINCLKUEN: 1;        // 116
	unsigned MAINCLKUEN_R: 31;

	unsigned SYSAHBCLKDIV;         // 120

	unsigned RESERVED3;            // 124
	
	struct {
		unsigned UNUSED0 : 7;
		unsigned CT16B0 : 1;
		unsigned UNUSED1: 5;
		unsigned ADC: 1;
		unsigned RESERVED0 : 1;
		unsigned WDT: 1;
		unsigned IOCON: 1;
		unsigned RESERVED1: 15;
	} SYSAHBCLKCTRL;               // 128

	unsigned RESERVED4[4];         // 132, 136, 140, 144
	 
	unsigned SSP0CLKDIV;           // 148
	unsigned UARTCLKDIV;           // 152
	unsigned SSP1CLKDIV;           // 156

	unsigned RESERVED5[12];        // 160, 164, 168, 172
	                               // 176, 180, 184, 188
	                               // 192, 196, 200, 204

	unsigned WDTCLKSEL;            // 208
	unsigned WDTCLKUEN;            // 212
	unsigned WDTCLKDIV;            // 216

	unsigned RESERVED6;            // 220

	unsigned CLKOUTCLKSEL;         // 224
	unsigned CLKOUTUEN;            // 228
	unsigned CLKOUTCLKDIV;         // 232

	unsigned RESERVED7[5];         // 236, 240, 244, 248
	                               // 252
	
	unsigned PIOPORCAP0;           // 256
	unsigned PIOPORCAP1;           // 260

	unsigned RESERVED8[18];        // 264, 268, 272, 276, 280, 284
	                               // 288, 292, 296, 300, 304, 308
	                               // 312, 316, 320, 324, 328, 332

	unsigned BODCTRL;              // 336
	unsigned SYSTCKCAL;            // 340

	unsigned RESERVED9[6];         // 344, 348, 352, 356, 360, 364

	unsigned IRQLATENCY;           // 368
	unsigned NMISRC;               // 372

	unsigned RESERVED10[34];       // 376, 380, 384, 388, 392,
	                               // 396, 400, 404, 408, 412,
	                               // 416, 420, 424, 428, 432,
	                               // 436, 440
	
	                               // 444, 448
	                               // 452, 456, 460, 464, 468,
	                               // 472, 476, 480, 484, 488,
	                               // 492, 496, 500, 504, 508

	unsigned STARTAPRP0;           // 512
	unsigned STARTERP0;            // 516

	unsigned STARTRSPR0CLR;        // 520
	unsigned STARTSRP0;            // 524

	unsigned RESERVED11[8];        // 528, 532, 536, 540
                                   // 544, 548, 552, 556

	unsigned PDSLEEPCFG;           // 560
	unsigned PDAWAKECFG;           // 564
	
	struct {
		unsigned UNUSED0: 4;
		unsigned ADC : 1;
		unsigned UNUSED1: 2;        
		unsigned SYSPLL: 1;
		unsigned RESERVED: 25; 
	} PDRUNCFG;                    // 568

	unsigned RESERVED12[110];

	unsigned DEVICE_ID;
} extern SYSCON;

extern unsigned ISER;

#endif // __LPC1114_H__
