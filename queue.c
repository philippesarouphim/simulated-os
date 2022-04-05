#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcb.c"

enum Policy { FCFS, SJF, RR, AGING, INVALID };

// Convert from string to Policy enum.
// If the string corresponds to no enum, INVALID is returned.
enum Policy string_to_policy(char* string){
    if(strcmp(string, "FCFS") == 0) return FCFS;
    if(strcmp(string, "SJF") == 0) return SJF;
    if(strcmp(string, "RR") == 0) return RR;
    if(strcmp(string, "AGING") == 0) return AGING;
    return INVALID;
}

struct Queue{
    struct QueueNode* head;
    void (*enqueue) (struct Queue* this, struct pcb* block);
    struct pcb* (*dequeue) (struct Queue* this);
    void (*execute) (struct Queue* this);
    void (*load_pages) (struct Queue* this);
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
void enqueue_regular(struct Queue* this, struct pcb* block){
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

// This method places a new PCB in the queue in ascending order of length.
void enqueue_shortest(struct Queue* this, struct pcb* block){
    struct QueueNode* newNode = create_queue_node(block);
    if(!this->head){
        this->head = newNode;
        return;
    }
    struct QueueNode* curr = this->head;
    while(curr->next && curr->next->block->pageCounter <= block->pageCounter);
    struct QueueNode* next = curr->next;
    curr->next = newNode;
    newNode->next = next;
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

// This method executes processes in the queue with a FCFS scheduling policy.
void fcfs(struct Queue* this){
    while(1){
        struct pcb* process = this->dequeue(this);
        if(!process) break;
        process->execute_until_end(process);
    }
}

// This method executes processes in the queue with a RR scheduling policy.
void rr(struct Queue* this){
    while(1){
        struct pcb* process = this->dequeue(this);
        if(!process) break;
        if(process->execute_next_n(process, 2)){
            this->enqueue(this, process);
        }
    }
}

// This method ages processes in the queue, and perfoms promotions if necessary.
void age(struct Queue* queue){
    if(!queue->head) return;

    // Age all processes except that at the head of the queue.
    struct QueueNode* curr = queue->head->next;
    while(curr){
        curr->block->score--;
        if(curr->block->score < 0) curr->block->score = 0;
        curr = curr->next;
    }

    // If the head no longer has the lowest score, perform promotion.
    if(queue->head->next && queue->head->block->score > queue->head->next->block->score){
        // Look for promotion spot
        struct QueueNode* young = queue->head;
        queue->head = queue->head->next;
        curr = queue->head;
        while(curr->next && curr->next->block->score <= young->block->score) curr = curr->next;

        // Move head to said promotion spot.
        young->next = curr->next;
        curr->next = young;
    }
    
}

// This method executes processes in the queue using an AGING policy.
void aging(struct Queue* this){
    while(1){
        if(!this->head) break;

        // Execute one line of code from the process at the head of the queue.
        if(this->head->block->execute_next(this->head->block)){
            // Age other processes and promote if necessary.
            age(this);
        } else{
            // If process is finished, dequeue it.
            this->dequeue(this);
        }
    }
}

// This method performs the initial loading of pages into memory.
void load_pages(struct Queue* this){
    struct QueueNode* curr = this->head;
    while(curr != NULL){
        curr->block->load_next_n_pages_into_memory(curr->block, 2);
        curr = curr->next;
    }
}

// This method creates a queue.
struct Queue* create_queue(enum Policy policy){
    struct Queue* queue = malloc(sizeof(struct Queue));
    queue->head = NULL;

    // Select enqueuing strategy depending on scheduling policy.
    if(policy == FCFS || policy == RR)
        queue->enqueue = enqueue_regular;
    else if(policy == SJF || policy == AGING)
        queue->enqueue = enqueue_shortest;

    queue->dequeue = dequeue;

    // Select dequeueing strategy depending on scheduling policy.
    if(policy == FCFS || policy == SJF)
        queue->execute = fcfs;
    if(policy == RR)
        queue->execute = rr;
    if(policy == AGING)
        queue->execute = aging;

    queue->load_pages = load_pages;

    return queue;
}