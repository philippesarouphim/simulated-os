#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

int generate_pid();
struct pcb* create_pcb(int i, char* code_file);
void load_code_into_backingstore(char* code_file, char* backing_location);

char* pid_counter = "pid_counter";

struct pcb{
    int pid;
    int score;
    int counter;

    char* code_file_name;
    FILE* code_file;
    int pageCounter;
    int pageTable[100];
    int endReached;

    int (*load_next_page_into_memory) (struct pcb* this);
    void (*load_all_pages_into_memory) (struct pcb* this);
    int (*execute_next) (struct pcb* this);
    int (*execute_next_n) (struct pcb* this, int num);
    void (*execute_until_end) (struct pcb* this);
    void (*free_memory) (struct pcb* this);
};

// This method cleares the process' code from memory.
void free_memory(struct pcb* this){
    // Iterate through every single line and reset the memory used.
    for(int i = 0; i < 100; i++){
        if(this->pageTable[i] != -1) deleteFrame(this->pageTable[i]);
    }
}

// This method executes a single line of code from the process.
// Returns -1 in case of a page fault, 0 if end of code is reached, 1 otherwise.
int execute_next(struct pcb* this){
    // Fetch current line of code.
    int page = this->pageTable[this->counter / 3];

    // Check for page fault
    if(page == -1){
        this->load_next_page_into_memory(this);
        return -1;
    }

    char* instruction = getFrameLine(page, this->counter % 3);

    // Check if end of code
    if(instruction == NULL && this->endReached) {
        free_memory(this);
        return 0;   
    }

    // Execute line of code.
    parseInput(instruction);

    // Increment PCB counter.
    this->counter++;

    return 1;
}

// This method executes a determined number of lines of code.
// Returns -1 in case of a page fault, 0 if end of code is reached, 1 otherwise.
int execute_next_n(struct pcb* this, int num){
    // Execute determined number of lines of code.
    for(int i = 0; i < num; i++){
        // Check whether end is reached, in which case return 0.
        int code = execute_next(this);
        if(code != 1){
            return code;
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

// This method of PCB loads a specified page into the frame store.
// If the end of file is reached it returns 1, otherwise 0.
int load_page_into_memory(struct pcb* this, int page){
    if(this->endReached) return 0;

    int eof = 0;
    char* lines[3];
    for(int i = 0; i < 3; i++){
        eof = feof(this->code_file);
        if(eof) {
            lines[i] = NULL;
            continue;
        }
        lines[i] = malloc(sizeof(char) * 100);
        fgets(lines[i], 100, this->code_file);
    }
    this->pageTable[page] = loadInFirstEmptyFrame(lines);
    if(eof) this->endReached = 1;
    return -eof + 1;
}

// This method loads the next page into the frame store.
// If the end of file is reached it returns 1, otherwise 0.
int load_next_page_into_memory(struct pcb* this){
    int code= load_page_into_memory(this, this->pageCounter);
    this->pageCounter++;
    return code;
}

// This method loads all remainaing pages into the frame store.
void load_all_pages_into_memory(struct pcb* this){
    while(load_next_page_into_memory(this));
}

// This is a constructor for the PCB struct.
// It generates the PCB with all default values, a unique PID, then loads the input code in memory.
// If ran out of memory while loading code, returns null.
struct pcb* create_pcb(int i, char* code_file){
    struct pcb* block = malloc(sizeof(struct pcb));

    block->pid = generate_pid();
    block->score = 0;
    block-> counter = 0;

    block->code_file_name = malloc(sizeof(char) * 22);
    sprintf(block->code_file_name, "./backingStore/prog%d", i);
    block->pageCounter = 0;
    block->endReached = 0; 
    for(int i = 0; i < 100; i++) block->pageTable[i] = -1;

    block->load_next_page_into_memory = load_next_page_into_memory;
    block->load_all_pages_into_memory = load_all_pages_into_memory;
    block->execute_next = execute_next;
    block->execute_next_n = execute_next_n;
    block->execute_until_end = execute_until_end;
    block->free_memory = free_memory;

    load_code_into_backingstore(code_file, block->code_file_name);

    block->code_file = fopen(block->code_file_name, "r");

    return block;
}

// This method loads code into the backing store.
void load_code_into_backingstore(char* code_file, char* backing_location){
    char* loadInBackingStoreCommmand = malloc(sizeof(char) * (4 + strlen(code_file) + strlen(backing_location)));
    sprintf(loadInBackingStoreCommmand, "cp %s %s", code_file, backing_location);
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