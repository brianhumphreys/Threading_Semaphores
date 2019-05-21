#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include "fiber_struct.h"
#include "semaphore.h"
#include "linkedList.h"
#include "threads.h"


#define MAX_FIBERS 129
#define STACK_SIZE 32767
#define JB_RSP 6
#define JB_PC 7

/* The index of the currently executing fiber */
static long int fiber_count = 0;
fiber_t* main_fiber;

static long int i64_ptr_mangle(long int p)
{
	long int ret;
	asm(" mov %1, %%rax;\n"
		" xor %%fs:0x30, %%rax;"
		" rol $0x11, %%rax;"
		" mov %%rax, %0;"
	: "=r"(ret)
	: "r"(p)
	: "%rax"
	);
	return ret;
}

static void _modALRM(int how) {
	sigset_t alrm_set;
	sigemptyset(&alrm_set);
	sigaddset(&alrm_set, SIGALRM);
	sigprocmask(how, &alrm_set, NULL);
}

void lock() {
	_modALRM(SIG_BLOCK);
}

void unlock() {
	_modALRM(SIG_UNBLOCK);
}

void scheduler() {
	rotate_next_ready();
	get_running_fiber()->status = RUNNING;
	longjmp(get_running_fiber()->context, 1);
}

static void signalHandler( int signum ){
	if(setjmp(get_running_fiber()->context)==0) {
		get_running_fiber()->status = READY;
		scheduler();
	} else {
		return;
	}
}

void initTimer() {
    /* Install timer_handler as the signal handler for SIGVTALRM. */
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
    sa.sa_handler = &signalHandler;
	
	sa.sa_flags = SA_NODEFER | SA_RESTART;
    if (sigaction (SIGALRM, &sa, NULL)<0) {
		perror("ERROR:");
		exit(errno);
	};
	ualarm(50000,50000);
}

void initMainFiber() {
	
	main_fiber = (fiber_t*) malloc(sizeof(fiber_t));
	main_fiber->status = RUNNING;
	main_fiber->pid = 0;

	push(main_fiber);
	fiber_count++;
}

void initFibers()
{
	initMainFiber();
	initTimer();
}

void start_routine_wrapper() {
	/* The function is executed and return value is stored and passed into the pthread_exit() */
	void* retval = (*get_running_fiber()->function)(get_running_fiber()->arg);
	pthread_exit(retval);
}

int pthread_create(pthread_t *fiber_ptr, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
	lock();
	if(fiber_count==0){
		initFibers();
	}
	
	(*fiber_ptr) = fiber_count;
	void* stack = malloc(STACK_SIZE);
	fiber_t *new_fiber = (fiber_t*) malloc(sizeof(fiber_t));

	new_fiber->function = start_routine;
	new_fiber->arg = arg;
	new_fiber->pid = fiber_count;
	new_fiber->status = READY;
	new_fiber->context[0].__jmpbuf[JB_RSP] = i64_ptr_mangle((long int) ((int*)stack + STACK_SIZE/8 - 2));
	new_fiber->context[0].__jmpbuf[JB_PC] = i64_ptr_mangle((long int) &start_routine_wrapper);

	push(new_fiber);
	fiber_count++;
	unlock();
	return 0;
}

pthread_t pthread_self() {
	return get_running_fiber()->pid;
}

void unblock_waiting_fiber() {
	if (get_running_fiber()->waiting_fiber!=NULL) get_running_fiber()->waiting_fiber->status = READY;
}

void pthread_exit(void *value_ptr) {
	lock();

	/* Store return value from thread if it is available
	*  This will be accessible when the thread is terminated
	*/

	if(value_ptr != NULL) get_running_fiber()->retval = value_ptr;

	/************************************************************/

	get_running_fiber()->status = TERMINATED;
	unblock_waiting_fiber();
	unlock();

	scheduler();
	exit(0);
}

int pthread_join(pthread_t thread, void **value_ptr) {

	lock();
	
	
	/* Error code checking 
	*  If the fiber does not exist or it has been terminated, return error ESRCH 
	*  If the thread is joining with itself, this will deadlock the system.  Return EDEADLK
	*  If there is already a thread waiting to join, return EINVAL
	*/
	if(get_fiber(thread)==NULL) return ESRCH;

	if(get_fiber(thread)==get_running_fiber()) return EDEADLK;

	if(get_fiber(thread)->waiting_fiber!=NULL) return EINVAL;

	fiber_t* term = fiber_exists_terminated(thread);
	if(term!=NULL) {
		*value_ptr = term->retval;
		free(term);
		return 0;
	}

	/****************************************************************************/

	fiber_t* target = get_fiber(thread);

	if(target==NULL || target->status==TERMINATED) unlock();
	else {
		target->waiting_fiber = get_running_fiber();
		unlock();
		if(setjmp(get_running_fiber()->context)==0) {
			get_running_fiber()->status = BLOCKED;
			scheduler();
		}
	}
	if(target->retval!=NULL) *value_ptr = target->retval;
	free(target);
	return 0; // return 0 on success
}

