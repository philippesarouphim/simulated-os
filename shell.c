#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"
#include "framestore.h"
#include "pagetable.h"

#include "macros.h"


void addWord(struct UserInput* this, char* word);
char* getCommand(struct UserInput* this);
char** getArgs(struct UserInput* this);
int getArgsSize(struct UserInput* this);
struct UserInput* create_UserInput();

int parseInput(char ui[]);

char worstChar(char first, char second);

// Start of everything
int main(int argc, char *argv[]) {

	// Display shell info
	printf("%s\n", "Shell version 1.1 Created January 2022");
	help();

	// Initialize all memory components
	mem_init();
	frameStore_init();
	LRUCache_init();

	// Reset backing store in case of unexpected exit
	system("test -d backingStore && rm -r backingStore");
	system("mkdir backingStore");

	// Display memory info
	printf("Frame Store Size = %d; Variable Store Size = %d\n\n", FRAME_STORE_SIZE, VARIABLE_STORE_SIZE);

	// Input loop
	while(1) {
		char* userInput = malloc(sizeof(char) * MAX_COMMAND_SIZE);
		printf("\n%c ", PROMPT); // Print prompt
		fgets(userInput, MAX_COMMAND_SIZE - 1, stdin); // Read input from user

		// If end of file is reached (only happens in batch),
		// then, switch the input from batch to command line.
		if(feof(stdin)){
			freopen("/dev/tty", "r", stdin);
		}
		
		// Parse input and send to interpreter, exit if error -1
		if(parseInput(userInput) == -1) exit(99);
	}

	return 0;

}

// Parse an input line into multiple UserInput objects.
int parseInput(char ui[]) {
	// Remove last character is it is a line return
	if(ui[strlen(ui) - 1] == '\n') ui[strlen(ui) - 1] = '\0';

	// Create UserInput dynamic array and add the first input to it
	struct UserInput** inputs = malloc(sizeof(struct UserInput*));
	int n = 1;
	struct UserInput* input = create_UserInput();
	inputs[0] = input;

	// Initialize starting pointer and skip all beginning white spaces.
	char* start = &ui[0];
	while (*start == ' ') start += sizeof(char);
	char* scout = start;
	while(1){
		// Stop on encountering special character.
		if(*scout == '\0' || *scout == ';' || *scout == ' '){
			char* anchor = scout; // Mark location of first special character.

			// Continue scouting to discover all consecutive special characters and retrieve the
			// highest priority one.
			char worst = *anchor;
			while(*scout == ';' || *scout == ' '){
				worst = worstChar(worst, *scout);
				scout = scout + sizeof(char);
			}
			if(*scout == '\0') worst = '\0';

			// Mark end of string at anchor point (first SC) and add word to UI object and update pointer for next iteration.
			*anchor = '\0';
			input->addWord(input, start);
			start = scout;

			// Based on event, wither continue parsing words, stop, or continue in another UI object.
			if(worst == ' ') continue;
			if(worst == '\0') break;

			// Create new input, expand input array, and add new input
			input = create_UserInput();
			inputs = realloc(inputs, sizeof(inputs) + sizeof(struct UserInput*));
			inputs[n] = input;
			n++;
		}
		scout += sizeof(char);
	}

	// Parsing done, moving on to interpretation
	for(int i = 0; i < n; i++) interpreter(inputs[i]);
	
}

// Find highest priority event depending on characters.
// Priority: '\0' > ';' > ' '.
char worstChar(char first, char second){
	if(first == '\0' || second == '\0') return '\0';
	if(first == ';' || second == ';') return ';';
	return ' ';
}

// This is a method of the UserInput to add a word.
// It resizes the string array and inserts the new word.
void addWord(struct UserInput* this, char* word){
	this->words = realloc(this->words, sizeof(this->words) + sizeof(char*));
	this->words[this->size] = word;
	this->size++;
}

// This method of UserInput returns the input command.
// The input command simply refers to the first word in the input.
char* getCommand(struct UserInput* this){
	return this->words[0];
}

// This method of UserInput returns the passed arguments.
// The arguments simply refer to all of the words passed in the input except the first.
char** getArgs(struct UserInput* this){
	return this->words + sizeof(char) * strlen(this->words[0]);
}

// This method of UserInput returns the number of args passed in the input.
// This number is simply the number of input words minus one.
int getArgsSize(struct UserInput* this){
	return this->size - 1;
}

// This method is a constructor for the UserInput struct.
struct UserInput* create_UserInput(){
	struct UserInput* input = malloc(sizeof(struct UserInput));
	input->words = malloc(sizeof(char*));
	input->size = 0;

	input->addWord = addWord;
	input->getCommand = getCommand;
	input-> getArgs = getArgs;
	input->getArgsSize = getArgsSize;

	return input;
}