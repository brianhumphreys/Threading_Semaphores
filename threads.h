#ifndef FIBERS_H
#define FIBERS_H



pthread_t pthread_self(void);
void pthread_exit(void *retval);
int pthread_create(pthread_t *fiber_ptr, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
int pthread_join(pthread_t thread, void **value_ptr);
void lock();
void unlock();
void scheduler();

#endif