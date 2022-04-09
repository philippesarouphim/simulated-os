#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "pcb.h"
#include "pagetable.h"
#include "shell.h"
#include "shellmemory.h"

#include "macros.h"

// This method cleares the process' code from the page table and framestore.
void free_memory(struct pcb* this){
    // this->pageTable->clearAllPages(this->pageTable);
}

// This method executes a single line of code from the process.
// Returns -1 in case of a page fault, 0 if end of code is reached, 1 otherwise.
int execute_next(struct pcb* this){
    // Fetch current line of code.
    char* instruction = this->pageTable->getLineFromPage(this->pageTable, this->counter / 3, this->counter % 3);

    // Check for page fault
    if(instruction && strcmp(instruction, PAGE_NOT_FOUND) == 0){
        this->load_next_page_into_memory(this);
        return -1;
    }

    // Check if end of code
    if(!instruction && this->endReached) {
        free_memory(this);
        return 0;   
    }

    // Execute line of code.
    parseInput(instruction);

    if(this->counter % 3 == 2 && this->counter / 3 == this->pageCounter -1 && this->endReached){
        free_memory(this);
        return 0;   
    }

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
        int code = this->execute_next(this);
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
    while(this->execute_next(this));
}

// This method of PCB loads a specified page into the frame store.
// If the end of file is reached it returns 1, otherwise 0.
int load_page_into_memory(struct pcb* this, int page){
    if(this->endReached) return 0;

    int eof = 0;
    char* lines[3];
    for(int i = 0; i < 3; i++){
        // Check if eof is reached, in which case fill the rest of the pointers with NULL.
        eof = feof(this->code_file);
        if(eof) {
            lines[i] = NULL;
            continue;
        }

        // Get the line from the file
        lines[i] = malloc(sizeof(char) * 100);
        fgets(lines[i], 100, this->code_file);
    }

    // Create the new page in the frame store with the fetched lines
    this->pageTable->createPage(this->pageTable, this->pageCounter, lines);

    // Check again if eof is reached, and set the endReached flag accordingly.
    eof = feof(this->code_file);
    if(eof) this->endReached = 1;

    // Increment page counter
    this->pageCounter++;

    // Return whether eof is reached
    return -eof + 1;
}

// This method loads the next page into the frame store.
// If the end of file is reached it returns 1, otherwise 0.
int load_next_page_into_memory(struct pcb* this){
    return this->load_page_into_memory(this, this->pageCounter);
}

// This method of pcb loads the next n pages into the framestore.
void load_next_n_pages_into_memory(struct pcb* this, int n){
    for(int i = 0; i < n; i++){
        int code = this->load_next_page_into_memory(this);
        if(code != 1) return;
    }
}

// This method of pcb loads all remainaing pages into the frame store.
void load_all_pages_into_memory(struct pcb* this){
    while(this->load_next_page_into_memory(this));
}

// This method loads code into the backing store.
void load_code_into_backingstore(char* code_file, char* backing_location){
    // Write cp command to load into backing store
    char* loadInBackingStoreCommmand = malloc(sizeof(char) * (4 + strlen(code_file) + strlen(backing_location)));
    sprintf(loadInBackingStoreCommmand, "cp %s %s", code_file, backing_location);

    // Execute command
    system(loadInBackingStoreCommmand);
}

// This function generates a new unique PID.
int generate_pid(){
    // Get counter from memory
    char* current_pid = mem_get_value(PID_COUNTER);

    // If counter does not exist, create it, set its value to 0, and return id 0.
    if(strcmp(PID_COUNTER, "Variable does not exist") == 0){
        current_pid = "0";
        mem_set_value(PID_COUNTER, current_pid);
        return 0;
    }

    // Generate new PID by incrementing current one by 1, save, and return
    int new_pid = atoi(current_pid) + 1;
    char* new_pid_s = malloc(8 * sizeof(char));
    sprintf(new_pid_s, "%d", new_pid);
    mem_set_value(PID_COUNTER, new_pid_s);
    return new_pid;
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
    block->pageTable = create_PageTable();

    block->load_page_into_memory = load_page_into_memory;
    block->load_next_page_into_memory = load_next_page_into_memory;
    block->load_all_pages_into_memory = load_all_pages_into_memory;
    block->load_next_n_pages_into_memory = load_next_n_pages_into_memory;
    block->execute_next = execute_next;
    block->execute_next_n = execute_next_n;
    block->execute_until_end = execute_until_end;
    block->free_memory = free_memory;

    load_code_into_backingstore(code_file, block->code_file_name);

    block->code_file = fopen(block->code_file_name, "r");

    return block;
}