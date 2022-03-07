#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

int generate_pid();
struct pcb* create_pcb(char* code_file);
void load_code_into_pcb(struct pcb* block, char* code_file);

char* pid_counter = "pid_counter";

struct pcb{
    int pid;
    char* mem_prefix;
    int length;
    int counter;
    int (*execute_next) (struct pcb* this);
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

// This method executes all lines of code in the process.
// On finish, it cleares the process' code from memory.
void execute_until_end(struct pcb* this){
    while(execute_next(this));
}

// This is a constructor for the PCB struct.
// It generates the PCB with all default values, a unique PID, then loads the input code in memory.
struct pcb* create_pcb(char* code_file){
    struct pcb* block = malloc(sizeof(struct pcb));

    block->pid = generate_pid();
    block->mem_prefix = malloc(sizeof(char) * 16);
    sprintf(block->mem_prefix, "process|%d|", block->pid);
    block->length = 0;
    block-> counter = 0;
    block->execute_next = execute_next;
    block->execute_until_end = execute_until_end;
    block->free_memory = free_memory;

    load_code_into_pcb(block, code_file);

    return block;
}

// This method loads code of a PCB into memory.
// It also sets the length of code to the appropriate value.
void load_code_into_pcb(struct pcb* block, char* code_file){
    FILE* code = fopen(code_file, "rt");

    char line[1000];

    // Loop through every line of code and save it.
    fgets(line,999, code);
	while(1){
		// Save the line of code
        char* line_key = malloc(sizeof(char) * 24);
        sprintf(line_key, "%s%d", block->mem_prefix, block->length);
		mem_set_value(line_key, line);

        block->length++;

        // Check if end of file is reached
		if(feof(code)){
			break;
		}

		fgets(line,999,code);
	}

    fclose(code);
}

// This method generates a new unique PID.
int generate_pid(){
    // Get counter from memory
    char* current_pid = mem_get_value(pid_counter);

    // If counter does not exist, create it, set its value to 0, and return id 0.
    if(!current_pid){
        current_pid = "0";
        mem_set_value(pid_counter, current_pid);
        return 0;
    }

    // Generate new PID by incrementing current one by 1, save, and return
    int new_pid = atoi(current_pid) + 1;
    char* new_pid_s = malloc(8 * sizeof(char));
    sprintf(new_pid_s, "%d", 42);
    mem_set_value(pid_counter, new_pid_s);
    return new_pid;
}