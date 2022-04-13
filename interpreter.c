#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "queue.h"
#include "interpreter.h"
#include "shellmemory.h"

int MAX_ARGS_SIZE = 7;

int help();
int quit();
int badcommand();
int printError(char* error);
int set(char* var, char* values[], int n);
int print(char* var);
int run(char* script);
int echo(char* value);
int my_ls();
int exec_conc();
int badcommandFileDoesNotExist();

// Interpret commands and their arguments
int interpreter(struct UserInput* input){
	printf("Number of words: %d\n", input->size);
	for(int i = 0; i < input->size; i++) printf("Word #%d: %s\n", i, input->words[i]);
	// int i;

	// if ( args_size < 1 ){
	// 	return badcommand();
	// } else if(args_size > MAX_ARGS_SIZE){
	// 	return printError("Too many tokens.\r\n");
	// }


	// for ( i=0; i<args_size; i++){ //strip spaces new line etc
	// 	command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	// }

	// if (strcmp(command_args[0], "help")==0){
	//     //help
	//     if (args_size != 1) return badcommand();
	//     return help();
	
	// } else if (strcmp(command_args[0], "quit")==0) {
	// 	//quit
	// 	if (args_size != 1) return badcommand();
	// 	return quit();

	// } else if (strcmp(command_args[0], "set")==0) {
	// 	// set

	// 	// Check if we have between 3 to 7 arguments
	// 	if (args_size < 3) return badcommand();
	// 	if (args_size > 7) return printError("Too many tokens.");

	// 	// Extract the words to be set to the variable
	// 	char *values[args_size - 2];
	// 	for (int i = 2, j = 0; i < args_size; i++, j++) values[j] = command_args[i];

	// 	// Perform set and return
	// 	return set(command_args[1], values, args_size - 2);
	
	// } else if (strcmp(command_args[0], "print")==0) {
	// 	if (args_size != 2) return badcommand();
	// 	return print(command_args[1]);
	
	// } else if (strcmp(command_args[0], "run")==0) {
	// 	if (args_size != 2) return badcommand();
	// 	return run(command_args[1]);
	
	// } else if(strcmp(command_args[0], "echo") == 0){
	// 	// echo

	// 	// Check if there are exactly 2 arguments
	// 	if (args_size != 2) return badcommand();

	// 	// Echo and return
	// 	return echo(command_args[1]);

	// } else if(strcmp(command_args[0], "my_ls") == 0){
	// 	// my_ls

	// 	// Check if there is only 1 args
	// 	if (args_size != 1) return badcommand();

	// 	// Perform ls and return
	// 	return my_ls();

	// } else if(strcmp(command_args[0], "exec") == 0){
	// 	// exec

	// 	// Check if args between 2 and 5
	// 	if(args_size < 2 || args_size > 5) return badcommand();

	// 	// Make the call for concurrent execution of processes.
	// 	return exec_conc(command_args, args_size);

	// } else if(strcmp(command_args[0], "resetmem") == 0){
	// 	// resetmem

	// 	if(args_size != 1) return badcommand();
	// 	mem_init();

	// } else return badcommand();
}

int help(){

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n";
	printf("%s\n", help_string);
	return 0;
}

int quit(){
	system("rm -r backingStore");
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

// Print the given error string and return code -1
int printError(char* error){
	printf("Bad command: %s", error);
	return -1;
}

// For run command only
int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int set(char* var, char* values[], int n){

	char *link = "=";
	char buffer[1000] = "";
	for(int i = 0; i < n - 1; i++){
		strcat(buffer, values[i]);
		strcat(buffer, " ");
	}
	strcat(buffer, values[n - 1]);

	mem_set_value(var, buffer);

	return 0;

}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

// Saves input code in memory, then runs it.
int run(char* script){
	// Create queue, and enqueue the only process
	struct Queue* queue = create_queue(FCFS);
	queue->enqueue(queue, create_pcb(0, script));

	// Load first two pages and execute processes in the queue.
	queue->load_pages(queue);
	queue->execute(queue);
}

// Prints value to the console.
// Or if preceded by $, print the variable if it exists.
int echo(char* value){
	// Check if the value is prefixed with '$'
	if(value[0] == '$'){
		// Get variable from memory and print
		char* val = mem_get_value(value + sizeof(char));
		printf("%s\r\n", strcmp(val, "Variable does not exist") == 0 ? "" : val);
	}
	// Print the text as is
	else printf("%s\r\n", value);
}

// List all of the files present in the current directory
int my_ls(){
	// Run the ls -1 command which lists each of the files in the current directory on a seperate line.
	system("ls -1");
	return 0;
}

// This method performs the concurrent execution of the processes based on the given scheduling policy.
int exec_conc(char* args[], int args_size){
	// Retrieve and parse policy from args
	enum Policy policy;
	if(args_size == 2) policy = FCFS;
	else policy = string_to_policy(args[args_size - 1]);
	if(policy == INVALID){
		printf("Policy invalid\n");
		return -1;
	}

	// Create PCBs and for each one, check if we ran out of memory and handle error.
	struct pcb* block1 = create_pcb(0, args[1]);

	struct pcb* block2;
	if(args_size >= 4) block2 = create_pcb(1, args[2]);

	struct pcb* block3;
	if(args_size == 5) block3 = create_pcb(2, args[3]);

	// Create ready queue and enqueue processes.
	struct Queue* queue = create_queue(policy);
	queue->enqueue(queue, block1);
	if(block2) queue->enqueue(queue, block2);
	if(block3) queue->enqueue(queue, block3);

	// Load first two pages and execute processes in the queue.
	queue->load_pages(queue);
	queue->execute(queue);
}
