/**
 * Brian Humphreys May 2019
 *
 * This file will automatically grade your cs170 threads library
 * Please see the README for more info
 */


#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define NUM_TESTS 1
#define PASS 1
#define FAIL 0

#define _DEBUG
#define TEST_WAIT_MILI 2000 // how many miliseconds do we wait before assuming a test is hung

void lock();
void unlock();

//if your code compiles you pass test 0 for free
//==============================================================================
static int test0(void){
    return PASS;
}



//end of tests
//==============================================================================


/**
 *  Some implementation details: Main spawns a child process for each
 *  test, that way if test 2/20 segfaults, we can still run the remaining
 *  tests. It also hands the child a pipe to write the result of the test.
 *  the parent polls this pipe, and counts the test as a failure if there
 *  is a timeout (which would indicate the child is hung).
 */


static int (*test_arr[NUM_TESTS])(void) = {&test0}; //, &test6, &test7
// static int (*test_arr[NUM_TESTS])(void) = {&test6}; //, &test6, &test7


int main(void){
    
    int status; pid_t pid;
    int pipe_fd[2]; int timeout; struct pollfd poll_fds;
    int score = 0; int total_score = 0;

    int devnull_fd = open("/dev/null", O_WRONLY);

    pipe(pipe_fd);
    poll_fds.fd = pipe_fd[0]; // only going to poll the read end of our pipe
    poll_fds.events = POLLRDNORM; //only care about normal read operations
        
    for(int i = 0; i < NUM_TESTS; i++){
        score = 0;
        pid = fork();

        //child, launches the test
        if (pid == 0){
#ifndef _DEBUG
            dup2(devnull_fd, STDOUT_FILENO); //begone debug messages
            dup2(devnull_fd, STDERR_FILENO);
#endif
            
            score = test_arr[i]();
            
            write(pipe_fd[1], &score, sizeof(score));
            exit(0); 
        }

        //parent, polls on the pipe we gave the child, kills the child,
        //keeps track of score
        else{  
            
            if(poll(&poll_fds, 1, TEST_WAIT_MILI)){
                read(pipe_fd[0], &score, sizeof(score));
            }
            
            total_score += score;
            kill(pid, SIGKILL);
            waitpid(pid,&status,0);
            
            
            if(score){
                printf("test %i : PASS\n", i);
            }
            else{
                printf("test %i : FAIL\n", i);
            }
        }
    }
    
    printf("total score was %i / %i\n", total_score, NUM_TESTS);
    return 0;
}