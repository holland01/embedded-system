#ifndef __COMMON_H__
#define __COMMON_H__

#define REG_OFFSET(byte_offset) ((byte_offset) >> 2)

#ifndef NULL
#define NULL 0
#endif

typedef unsigned word_t;
typedef unsigned bool_t;

enum {
	True = 1,
	False = 0
};

void sleep(word_t millisec);

#endif // __COMMON_H__