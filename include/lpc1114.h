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

volatile struct TMR16 {
	unsigned IR;
	
	unsigned TCR;
	
	unsigned TC;  

	unsigned PR;
	
	unsigned PC;
	
    unsigned MCR;
	
	unsigned MR0;
	
	unsigned MR1;
	
	unsigned MR2;

	unsigned MR3;

	unsigned CCR;
	
	unsigned CR0;

	unsigned __PADDING[3];
	
	unsigned EMR;
	unsigned __RESERVED[12];

	unsigned CTCR;
	
	unsigned PWM;
} extern TMR16B0;

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
	
	unsigned SYSAHBCLKCTRL;               // 128

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
