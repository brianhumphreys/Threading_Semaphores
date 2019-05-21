#ifndef FIBER_STRUCT_H
#define FIBER_STRUCT_H

#include <setjmp.h>
#include <cstdint>


enum FIBER_STATE { RUNNING, READY, BLOCKED, TERMINATED };

typedef struct fiber_t
{
	fiber_t* waiting_fiber;
	fiber_t* sema_next_waiting_fiber;
	jmp_buf context;
	void* (*function)(void*);
	void* arg;
	void* retval;
	FIBER_STATE status;
	uint64_t pid;

} fiber_t;

#endif