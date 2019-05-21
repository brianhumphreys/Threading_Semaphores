#ifndef SEMAPHORE_H
#define SEMAPHORE_H


typedef struct sema_t {
	long int sem_id;
	bool is_init;
	unsigned val;
	fiber_t* waiting_fibers;
    sema_t* next_sema;
} sema_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);

#endif