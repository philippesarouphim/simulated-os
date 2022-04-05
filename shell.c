
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "interpreter.h"
#include "shellmemory.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[]) {

	printf("%s\n", "Shell version 1.1 Created January 2022");
	help();

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	mem_init();
	frameStore_init();

	system("test -d backingStore && rm -r ./backingStore");
	system("mkdir backingStore");

	printf("Frame Store Size = %d; Variable Store Size = %d\n\n", FRAME_STORE_SIZE, VARIABLE_STORE_SIZE);

	while(1) {
		printf("%c ",prompt);
		fgets(userInput, MAX_USER_INPUT-1, stdin);

		// If end of file is reached (only happens in batch),
		// then, switch the input from batch to command line.
		if(feof(stdin)){
			freopen("/dev/tty", "r", stdin);
		}

		errorCode = parseInput(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors
		memset(userInput, 0, sizeof(userInput));
	}

	return 0;

}

// Extract words from the input then call interpreter
int parseInput(char ui[]) {
 
	char tmp[200];
	char *words[10][100];							
	int a=0,b;							
	int v=0;
	int w[10];

	// Parse all commands
	while(ui[a] != '\0' && a<1000){
		w[v] = 0;

		// Skip white spaces
		for(a = a; ui[a]==' ' && a<1000; a++);

		// Extracts all words from a single command
		while(ui[a] != ';' && ui[a] != '\0' && a<1000) {

			// Extract a word
			for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
				tmp[b] = ui[a];
			tmp[b] = '\0';

			words[v][w[v]] = strdup(tmp);

			if(ui[a] != ';' && ui[a] != '\0') a++; 
			w[v]++;
		}

		if(ui[a] != '\0') a++;
		v++;
	}

	// Execute all commands
	for(int i = 0; i < v; i++){
		int code = interpreter(words[i], w[i]);
	}

	return 0;
}
