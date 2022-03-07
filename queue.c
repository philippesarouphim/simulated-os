#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcb.h"


struct Queue{
    struct QueueNode* head;
    void (*enqueue) (struct Queue* this, struct pcb* block);
    struct pcb* (*dequeue) (struct Queue* this);
};

struct QueueNode{
    struct pcb* block;
    struct QueueNode* next;
};

// This is a constructor for the queue node.
// It takes a pointer to a PCB as input and returns the node.
struct QueueNode* create_queue_node(struct pcb* block){
    struct QueueNode* node = malloc(sizeof(struct QueueNode));
    node->block = block;
    node->next = NULL;
    return node;
}

// This method enqueues a new PCB into the queue.
void enqueue(struct Queue* this, struct pcb* block){
    if(!this->head){
        this->head = create_queue_node(block);
        return;
    }
    struct QueueNode* curr = this->head;
    while(curr->next){
        curr = curr->next;
    }
    curr->next = create_queue_node(block);
}

// This method dequeues the head PCB from the queue and returns it.
struct pcb* dequeue(struct Queue* this){
    if(!this->head) return NULL;
    struct QueueNode* top = this->head;
    this->head = this->head->next;
    struct pcb* toReturn = top->block;
    free(top);
    return toReturn;
}

// This method creates a queue.
struct Queue* create_queue(){
    struct Queue* queue = malloc(sizeof(struct Queue));
    queue->head = NULL;
    queue->enqueue = enqueue;
    queue->dequeue = dequeue;
    return queue;
}