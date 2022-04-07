#include "pcb.h"

enum Policy { FCFS, SJF, RR, AGING, INVALID };
enum Policy string_to_policy(char* string);

struct Queue{
    struct QueueNode* head;
    void (*enqueue) (struct Queue*, struct pcb* block);
    struct pcb* (*dequeue) (struct Queue*);
    void (*execute) (struct Queue*);
    void (*load_pages) (struct Queue*);
};

struct Queue* create_queue(enum Policy policy);