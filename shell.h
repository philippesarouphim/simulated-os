struct UserInput{
	char** words;
	int size;
	void (*addWord) (struct UserInput* this, char* word);
	char* (*getCommand) (struct UserInput* this);
	char** (*getArgs) (struct UserInput* this);
	int (*getArgsSize) (struct UserInput* this);
};

int parseInput(char ui[]);