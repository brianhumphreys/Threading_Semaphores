Threading Library
By Brian Humphreys

This file is a simple threading library which can be interfaced with by three simple functions: `pthread_create()`, `pthread_self()` and `pthread_exit()`. 

Each thread is maintained by a Thread (Fiber) Control Block which allows aacess to the threads stack, ID, context and start_routine.  The library is preemptive and switches between threads ever 50ms.  When switching, the signal handler will save the current context and then call on the scheduler to start the next waiting thread by a round robin approach.

`pthread_create()`:  In this function, I set the context of the new thread and add it to the FCB (fiber) struct list.  

`pthread_self()`:  simply returns the ID of the calling thread

`pthread_exit()`:  deallocates the calling thread and makes the TCB inactive

`pthread_join()`: calling thread will be blocked until the target thread has completed execution

`sem_init()`: this will create a semaphore structure and add it to the list of sems.  Sems get rotated to head of list upon calling them.

`sem_destory()`: this will pop the calling semaphore from the stack and delete from memory.

`sem_wait()`: called when entering a critical section of code.  This will decrement the semaphore initialized in `sem_init` and block the calling thread if the semaphore is 0.

`sem_post()`:  called when exiting a critical section of code.  this will increment the calling semaphore's value and unblock the next waiting thread if there is one.

## PA2
The biggest trouble I had with this project was figuring out the proper size specification.  I divided the stack size by eight while others advised to do other things like dividing by 4 or not dividing at all and subtracting a couple bytes.  Some clarification on this would be great for the next project.  Also, I think I am going to change to a doubly linked list for a couple of reasons.  1) a linked list would just be easier to reference than a fix sized array and it can create as many threads as it wants.  2) doubly linked because some people advised that after the main thread, you should add new threads in reverse order.  

## PA3
The main issue here is my linking.  This is a large project and I felt it was better practice to spread the code across multiple files.  The problem is that now I was not able to succesfully make a single threads.o file.  My makefile succesfully compiles for my code however and you are able to run the test by dropping your test code in here and running `make` and then `sample_grader` just as usual.  i hop this is not an issue.  Sorry for any inconvinience this causes.

Learned a lot.  Thanks to Ryan and Sean for helping out a lot, you guys are the GOATS.