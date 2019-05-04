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
} extern GPIO1, GPIO0;

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
	
	unsigned PWMC;
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
  
	unsigned PDRUNCFG;                    // 568

	unsigned RESERVED12[110];

	unsigned DEVICE_ID;
} extern SYSCON;

volatile struct ADC {
	unsigned CR;
	unsigned GD;
	
	unsigned __RESERVED0;

	unsigned INTEN;
	
	unsigned R0;
	unsigned R1;
	unsigned R2;
	unsigned R3;
	unsigned R4;
	unsigned R5;
	unsigned R6;
	unsigned R7;
	
	unsigned STAT;
} extern ADC;

extern unsigned ISER;
extern unsigned ICER;

extern unsigned IOCON_PIO0_8;
extern unsigned IOCON_R_PIO0_11;

/* Masks are used either to
   a) ensure that unwanted data isn't set, or
   b) to zero out specific bits */

/*  */

#define SYSCON_PDRUNCFG_SYSPLL_OFF (1 << 7)
#define SYSCON_PDRUNCFG_ADC_OFF (1 << 4)

#define SYSCON_SYSPLLCLK_IRC 0
#define SYSCON_MAINCLKSEL_PLL 3

#define SYSCON_SYSAHBCLKCTRL_CT16B0_ON (1 << 7)
#define SYSCON_SYSAHBCLKCTRL_ADC_ON (1 << 13)
#define SYSCON_SYSAHBCLKCTRL_IOCON_ON (1 << 16)

#define ADC_CR_SEL_CLEAR_MASK (~0xFF)
#define ADC_CR_SEL_CHANNEL_SHIFT(x) (1 << ((x))
#define ADC_CR_SEL_SET_CHANNEL(x) ( (ADC.CR & ADC_CR_SEL_CLEAR_MASK) | ADC_CR_SEL_CHANNEL_SHIFT((x))) )

#define ADC_CR_BURST_ON (1 << 16)
#define ADC_CR_BURST_SET_OFF (ADC.CR & (~ADC_CR_BURST_ON))

#define ADC_CR_CLKDIV_CLEAR_MASK 0xFFFF00FF
#define ADC_CR_CLKDIV_SHIFT(x) ((x) << 11)
#define ADC_CR_CLKDIV_SET_VALUE(x) ((ADC.CR & ADC_CR_CLKDIV_CLEAR_MASK) | ADC_CR_CLKDIV_SHIFT((x)))

#define ADC_CR_CLKS_MASK ((1 << 17) | (1 << 18) | (1 << 19))
#define ADC_CR_CLKS_SET_11_CLK_10_BIT ((ADC.CR & ADC_CR_CLKS_MASK) | ~ADC_CR_CLKS_MASK)

#define ADC_CR_START_MASK ((1 << 24) | (1 << 25) | (1 << 26))

#define ADC_CR_START_SET_NO_START (ADC.CR & (~ADC_CR_START_MASK))

#define ADC_INTEN_ADINTEN_CHANNEL(x) (1 << (x))
#define ADC_INTEN_ADGINTEN (1 << 8)

#define ADC_INTEN_SET_ADGINTEN_ONLY ((ADC.INTEN & (~ADC_INTEN_ADINTEN_CHANNEL(0))) | ADC_INTEN_ADGINTEN)

#define ISER_IRQ16_ENABLED (ISER | (1 << 16))
#define ISER_IRQ24_ENABLED (ISER | (1 << 24))

#define TMR16B0_MCR_ENABLE_MR1_I (TMR16B0.MCR | (1 << 3))
#define TMR16B0_MCR_ENABLE_MR1_R (TMR16B0.MCR | (1 << 4))

#define TMR16B0_PWMC_ENABLE_MR0 (TMR16B0.PWMC | (1 << 0))

#endif // __LPC1114_H__
