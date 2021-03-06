# Threading and Semaphores Library
By Brian Humphreys

## Background and Motivation
This project not only showcases knowledge of how to use threading libraries but most importantly shows my knowledge of how the libraries work under the hood.  Thourough knowledge on memory managament, operating system calls, data structures (Linked Lists), process execution and general coding practices.  I will soon be creating a reading/writing and memory managment application that uses this library as proof of concept that this works.  Until that point, try running the tests yourself.  Simply perform the following commands:

`make`
`./sample_threading`

All the tests should pass.  You can view the tests in the `autotester_main_3.c` file.


## Function Documentation
This file is a simple threading library which can be interfaced in the same way as the `pthread` library and the `sem_t` library.  The difference here is that both the thread pool and the semaphore pool are both implemented using linked lists and are much more dynamic than the common implementation of fixed sized arrays.

Each thread is maintained by a Thread (Fiber) Control Block which allows access to the threads stack, ID, context and start_routine.  The library is preemptive and switches between threads ever 50ms.  When switching, the signal handler will save the current context and then call on the scheduler to start the next waiting thread by a round robin approach.

### threads.c
`pthread_create()`:  In this function, I set the context of the new thread and add it to the FCB (fiber) struct list.  

`pthread_self()`:  simply returns the ID of the calling thread

`pthread_exit()`:  deallocates the calling thread and makes the TCB inactive

`pthread_join()`: calling thread will be blocked until the target thread has completed execution

### semaphores.c
`sem_init()`: this will create a semaphore structure and add it to the list of sems.  Sems get rotated to head of list upon calling them.

`sem_destory()`: this will pop the calling semaphore from the stack and delete from memory.

`sem_wait()`: called when entering a critical section of code.  This will decrement the semaphore initialized in `sem_init` and block the calling thread if the semaphore is 0.

`sem_post()`:  called when exiting a critical section of code.  this will increment the calling semaphore's value and unblock the next waiting thread if there is one.


