#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

int generate_pid();
struct pcb* create_pcb(int i, char* code_file);
void load_code_into_backingstore(int i, char* code_file);

char* pid_counter = "pid_counter";

struct pcb{
    int pid;
    char* mem_prefix;
    int length;
    int score;
    int counter;

    int (*execute_next) (struct pcb* this);
    int (*execute_next_n) (struct pcb* this, int num);
    void (*execute_until_end) (struct pcb* this);
    void (*free_memory) (struct pcb* this);
};

// This method cleares the process' code from memory.
void free_memory(struct pcb* this){
    // Iterate through every single line and reset the memory used.
    for(int i = 0; i < this->length; i++){
        char* curr_line_key = malloc(sizeof(char) * 24);
        sprintf(curr_line_key, "%s%d", this->mem_prefix, i);
        mem_reset(curr_line_key);
    }
}

// This method executes a single line of code from the process.
// If it executes the last line of code, it cleares the process' code from memory.
// Returns 0 if end of code is reached, 1 otherwise.
int execute_next(struct pcb* this){
    // Fetch current lie of code.
    char* curr_line_key = malloc(sizeof(char) * 24);
    sprintf(curr_line_key, "%s%d", this->mem_prefix, this->counter);
    char* instruction = mem_get_value(curr_line_key);

    // Execute line of code.
    parseInput(instruction);

    // Increment PCB counter.
    this->counter++;

    // If end of code is reached, clear process' code from memory.
    if(this->counter == this->length){
        this->free_memory(this);
        return 0;
    }

    return 1;
}

// This method executes a determined number of lines of code.
// If it executes the last line of code, it cleares the process' code from memory.
// Returns 0 if end of code is reached, 1 otherwise.
int execute_next_n(struct pcb* this, int num){
    // Execute determined number of lines of code.
    for(int i = 0; i < num; i++){
        // Check whether end is reached, in which case return 0.
        if(!execute_next(this)){
            return 0;
        }
    }

    // Execution terminated without reaching end of code, return 1.
    return 1;
}

// This method executes all lines of code in the process.
// On finish, it cleares the process' code from memory.
void execute_until_end(struct pcb* this){
    while(execute_next(this));
}

// This is a constructor for the PCB struct.
// It generates the PCB with all default values, a unique PID, then loads the input code in memory.
// If ran out of memory while loading code, returns null.
struct pcb* create_pcb(int i, char* code_file){
    struct pcb* block = malloc(sizeof(struct pcb));

    block->pid = generate_pid();
    block->mem_prefix = malloc(sizeof(char) * 16);
    sprintf(block->mem_prefix, "process|%d|", block->pid);
    block->length = 0;
    block->score = 0;
    block-> counter = 0;
    block->execute_next = execute_next;
    block->execute_next_n = execute_next_n;
    block->execute_until_end = execute_until_end;
    block->free_memory = free_memory;

    load_code_into_backingstore(i, code_file);

    return block;
}

// This method loads code into the backing store.
void load_code_into_backingstore(int i, char* code_file){
    char* loadInBackingStoreCommmand = malloc(sizeof(char) * (25 + strlen(code_file)));
    sprintf(loadInBackingStoreCommmand, "cp %s ./backingStore/prog%d", code_file, i);
    system(loadInBackingStoreCommmand);
}

// This method generates a new unique PID.
int generate_pid(){
    // Get counter from memory
    char* current_pid = mem_get_value(pid_counter);

    // If counter does not exist, create it, set its value to 0, and return id 0.
    if(strcmp(pid_counter, "Variable does not exist") == 0){
        current_pid = "0";
        mem_set_value(pid_counter, current_pid);
        return 0;
    }

    // Generate new PID by incrementing current one by 1, save, and return
    int new_pid = atoi(current_pid) + 1;
    char* new_pid_s = malloc(8 * sizeof(char));
    sprintf(new_pid_s, "%d", new_pid);
    mem_set_value(pid_counter, new_pid_s);
    return new_pid;
}