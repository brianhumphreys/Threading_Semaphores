#include <cstdlib>
#include <cstdio>
#include <stdlib.h>

#include "fiber_struct.h"
#include "linkedList.h"

node_t * head;
node_t * tail;

static int listLength = 0;

fiber_t *get_running_fiber() {
    fiber_t* h_node = head->fiber;
    return h_node;
}

void printPids() {
    node_t* ptr = head;
    while(ptr!=NULL) {
        ptr = ptr->next;
    }
}

fiber_t* pop() {
    if(head==NULL) { // List is empty
        perror("ERROR: Threads have not been initialized"); 
        return NULL;
    } else if (head == tail) { // List has 1 item in it
        node_t * head_node_ptr = head;

        head = NULL;
        tail = NULL;
        listLength--;

        return head_node_ptr->fiber;
    } else { // List has more than 1 item in it
        /* Set temp head pointer */
        node_t * head_node_ptr = head;

        /*Then set head to second node in list */
        head = head_node_ptr->next;
        fiber_t * ret_fiber = head_node_ptr->fiber;
        free(head_node_ptr);

        listLength--;
        return ret_fiber;
    }
}

void push(fiber_t* new_fiber) {

    node_t * new_node_ptr = (node_t*) malloc(sizeof(node_t));
    new_node_ptr->fiber = new_fiber;
    new_node_ptr->next = NULL;

    // only called for main thread
    if(listLength==0) {
        /* Set the list current, tail, and head*/
        
        head = new_node_ptr;
        tail = new_node_ptr;

        listLength++;
    } else {

        tail->next = new_node_ptr;
        tail = new_node_ptr;

        listLength++;
    }
}

void rotate() {
    fiber_t *rot = pop();
    push(rot);
}

const char* getFiberStatus(enum FIBER_STATE status) 
{
    printf("getting status\n");
   switch (status) 
   {
      case RUNNING: return "Running\n";
      case READY: return "Ready\n";
      case BLOCKED: return "Blocked\n";
      case TERMINATED: return "Terminated\n";
      default: return "Status not defined\n";
   }
}

fiber_t * get_fiber(pthread_t target_pid) {
    node_t* node_ptr = head;
    
    do {
        if(node_ptr==NULL) return NULL;
        if((node_ptr->fiber->pid==target_pid)  && node_ptr->fiber->status != TERMINATED) return node_ptr->fiber;
        node_ptr = node_ptr->next;
    } while(1);
}

fiber_t * fiber_exists_terminated(pthread_t target_pid) {
    node_t* node_ptr = head;
    
    do {
        if(node_ptr==NULL) return NULL;
        if((node_ptr->fiber->pid==target_pid)  && node_ptr->fiber->status == TERMINATED) return node_ptr->fiber;
        node_ptr = node_ptr->next;
    } while(1);
}

void rotate_next_ready() {
    do {
        rotate();
        // getFiberStatus(get_running_fiber()->status);
    } while(get_running_fiber()->status==BLOCKED || get_running_fiber()->status==TERMINATED);

}








