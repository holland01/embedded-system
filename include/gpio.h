#ifndef __GPIO_H__
#define __GPIO_H__

#include "common.h"

/*
 * DATA_ADDR_REG_OFS: data address register offset  
 */

#define GPIO_PORT_0 0x50000000
#define GPIO_PORT_1 0x50010000
#define GPIO_PORT_2 0x50020000
#define GPIO_PORT_3 0x50030000

#define GPIO_PORT_DATA_SIZE (1 << 12)

#define GPIO_NUM_PORTS 4

enum GPIO_REG_OFFSETS_ {
	GRO_DATA_ADDR_START = 0x0,
	GRO_DATA_ADDR_END = REG_OFFSET(0x3FF8),
	
	GRO_DATA = REG_OFFSET(0X3FFC),

	GRO_IO = REG_OFFSET(0X8000),
	GRO_ISENSE = REG_OFFSET(0x8004),
	GRO_IBE = REG_OFFSET(0x8008),
	GRO_IEV = REG_OFFSET(0x800c),

	GRO_IMASK = REG_OFFSET(0x8010),
	GRO_RAWST = REG_OFFSET(0x8014),
	GRO_MASKED_IS = REG_OFFSET(0x8018),
	GRO_CLR = REG_OFFSET(0x801C)
};

enum GPIO_PINS_ {
	GPIO_PIN_0 = 1 << 0,
	GPIO_PIN_1 = 1 << 1,
	GPIO_PIN_2 = 1 << 2,
	GPIO_PIN_3 = 1 << 3,
	GPIO_PIN_4 = 1 << 4,
	GPIO_PIN_5 = 1 << 5,
	GPIO_PIN_6 = 1 << 6,
	GPIO_PIN_7 = 1 << 7,
	GPIO_PIN_8 = 1 << 8,
	GPIO_PIN_9 = 1 << 9,
	GPIO_PIN_10 = 1 << 10,
	GPIO_PIN_11 = 1 << 11
};

#define GPIO_REG(decl) \
	decl : 12;\
	word_t :0

struct GPIO_CTRL {
	word_t data[1 << 12];
	word_t unused[1 << 12];

	GPIO_REG(word_t io_dir);
	GPIO_REG(word_t int_sense);
	GPIO_REG(word_t int_both_edge_sense);
	GPIO_REG(word_t int_event);
	GPIO_REG(word_t int_mask);
	GPIO_REG(word_t raw_int_status);
	GPIO_REG(word_t masked_int_status);
	GPIO_REG(word_t int_clear);
};

extern struct GPIO_CTRL* GPIO[GPIO_NUM_PORTS];

#endif // __GPIO_H__
