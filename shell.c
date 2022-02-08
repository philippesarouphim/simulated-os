
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

	FILE* input = stdin;

	while(1) {
		printf("%c ",prompt);
		fgets(userInput, MAX_USER_INPUT-1, input);

		errorCode = parseInput(userInput);
		if (errorCode == -1) exit(99);	// ignore all other errors
		memset(userInput, 0, sizeof(userInput));

		/*if(feof(stdin)){
			printf("Hello");
			freopen("/dev/console", "rt", input);
		}*/
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

	while(ui[a] != '\0' && a<1000){
		w[v] = 0;

		for(a = a; ui[a]==' ' && a<1000; a++);		// skip white spaces

		while(ui[a] != ';' && ui[a] != '\0' && a<1000) {

			for(b=0; ui[a]!=';' && ui[a]!='\0' && ui[a]!=' ' && a<1000; a++, b++)
				tmp[b] = ui[a];						// extract a word
		
			tmp[b] = '\0';

			words[v][w[v]] = strdup(tmp);

			if(ui[a] != ';') a++; 
			w[v]++;
		}

		a++;
		v++;
	}

	for(int i = 0; i < v; i++){
		int code = interpreter(words[i], w[i]);
	}

	return 0;
}
