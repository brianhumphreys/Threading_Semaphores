#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node_t{
    node_t *next;
    fiber_t* fiber;
    
 } node_t;

void init_main_node(fiber_t* main);
void printPids();
fiber_t *get_running_fiber();
fiber_t *pop(); 
void push(fiber_t* new_fiber);
void rotate();
const char* getFiberStatus(enum FIBER_STATE status);
fiber_t* get_fiber(pthread_t target_pid);
fiber_t* fiber_exists_terminated(pthread_t target_pid);
void rotate_next_ready();

#endif