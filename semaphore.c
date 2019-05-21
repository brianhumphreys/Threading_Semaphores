#include <semaphore.h>
#include <stdlib.h>
#include <errno.h>
#include <cstdio>

#include "fiber_struct.h"
#include "linkedList.h"
#include "threads.h"
// #include "scheduler.h"
#include "semaphore.h"


#define SEM_VALUE_MAX 65536

sema_t* sema_head;
sema_t* sema_tail;


sema_t* semaphore_new(unsigned int val) {
	sema_t* new_sema = (sema_t*) malloc(sizeof(sema_t));
    new_sema->sem_id = (long int) new_sema;
	new_sema->val = val;
	new_sema->is_init = true;
    new_sema->next_sema = NULL;
	return new_sema;
}

void push_sema(sema_t* new_sema) {
    if(new_sema==NULL) return;
    if(sema_head==NULL) {
        sema_head=new_sema;
        sema_tail=new_sema;
    } else {
        sema_tail->next_sema=new_sema;
        sema_tail = sema_tail->next_sema;
    }
}

sema_t* pop_sema() {
    if(sema_head==NULL) return NULL;
    else if(sema_head==sema_tail) {
        sema_t* head = sema_head;
        sema_head=NULL;
        sema_tail=NULL;
        return head;
    } else {
        sema_t* head = sema_head;
        sema_head = head->next_sema;
        head->next_sema=NULL;
        return head;
    }
}

void rotate_calling_sema(sem_t* sem) {
    if(sema_head!=sema_tail) {
        while(sem->__align != (long int) sema_head) push_sema(pop_sema());
    }
}

int sem_init(sem_t *sem, int pshared, unsigned int value){

    /* Error handling
    *  If the semaphore value max is exceeded, return EINVAL
    */

    if(value>SEM_VALUE_MAX) return EINVAL;

    if(pshared!=0) return ENOSYS;

    if(sem==NULL) return -1;

    /********************************************************************/
    sema_t* new_sema = semaphore_new(value);
    sem->__align = (long int) new_sema;
    push_sema(new_sema);
	return 0;
}


bool sema_exists(sem_t* sem) {
    sema_t* sema_ptr = sema_head;
    while(sema_ptr!=NULL && sem->__align != (long int) sema_ptr) {
        sema_ptr = sema_ptr->next_sema;
    }
    if(sema_ptr!=NULL) return true;
    return false;
}


int sem_destroy(sem_t *sem) {
    lock();
    /* We should not assume that the semaphore is existent.  Exit with error code if it does not. */

    if(sem==NULL) return -1;

    if(!sema_exists(sem)) return EINVAL;

    /*********************************************************************************************/

    /* The semaphore should be removed from the list before freeing from memory */
    rotate_calling_sema(sem);

    sema_t* dead_sema = pop_sema();
    free(dead_sema);
    unlock();
	return 0;
}

void push_block_waiting_fiber() {
    fiber_t* new_waiting_fiber = get_running_fiber();
    fiber_t* waiting_fiber_ptr = sema_head->waiting_fibers;

    /* If no thread are wating yet, we must initialize the waiting list */
    if(waiting_fiber_ptr==NULL) {
        sema_head->waiting_fibers = new_waiting_fiber;
        waiting_fiber_ptr = new_waiting_fiber;
    } else { /* If there are thread waiting already, then we will iterate to the end of the list and append */
        while(waiting_fiber_ptr->sema_next_waiting_fiber!=NULL) {
            waiting_fiber_ptr = waiting_fiber_ptr->sema_next_waiting_fiber;
        }
    }
    new_waiting_fiber->status = BLOCKED;
    waiting_fiber_ptr->sema_next_waiting_fiber = new_waiting_fiber;
}

fiber_t* pop_unblock_waiting_fiber() {
    fiber_t* next_owner_fiber_ptr = sema_head->waiting_fibers;
    sema_head->waiting_fibers = next_owner_fiber_ptr->sema_next_waiting_fiber;
    next_owner_fiber_ptr->status = READY;
    return next_owner_fiber_ptr;
}

int sem_wait(sem_t *sem) {
    lock();

    /* We should not assume that the semaphore is existent.  Exit with error code if it does not. */

    if(sem==NULL) return -1;

    if(!sema_exists(sem)) return -1;

    /*********************************************************************************************/

    /* move the calling semaphore to head */
    rotate_calling_sema(sem);
    if(sema_head->val==0) {
        push_block_waiting_fiber();
        unlock();
        if(setjmp(get_running_fiber()->context)==0)
            scheduler();
    } else {
        sema_head->val--;
        unlock();
    }

	return 0;
}
int sem_post(sem_t *sem) {
    lock();

    /* We should not assume that the semaphore is existent.  Exit with error code if it does not. */

    if(sem==NULL) return -1;

    if(!sema_exists(sem)) return -1;

    /*********************************************************************************************/

    /* The semaphore should be removed from the list before freeing from memory */
    rotate_calling_sema(sem);

    if(sema_head->val==0 && sema_head->waiting_fibers!=NULL) {
        pop_unblock_waiting_fiber();
        unlock();
    } else {
        /* Do not allow overflow to occur */

        if(sema_head->val==SEM_VALUE_MAX) return EOVERFLOW;

        /******************************************************************/

        sema_head->val++;
        unlock();
    }

	return 0;
}