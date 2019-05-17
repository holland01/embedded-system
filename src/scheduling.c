#include "lpc1114.h"
#include "framework.h"

typedef struct thread thread_t;

struct thread {
	thread_t* next;
	unsigned sp;
};

#define THREAD(name, function, stacksize, arg0, arg1, arg2, arg3)	\
	struct {																												\
		thread_t thread;																							\
		unsigned stack[(stacksize) - 16];														 
  }
	

void setup() {

}

void loop() {
	asm volatile("");
}
