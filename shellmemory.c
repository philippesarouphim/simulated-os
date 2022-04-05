#include<stdlib.h>
#include<string.h>
#include<stdio.h>

// -------------
// VARIABLE STORE
// --------------
struct variable_memory_struct{
	char *key;
	char *value;
};

struct variable_memory_struct variableStore[VARIABLE_STORE_SIZE];

// Helper functions
int match(char *model, char *key) {
	int i, len=strlen(key), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(key+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){

	int i;
	for (i=0; i<VARIABLE_STORE_SIZE; i++){		
		variableStore[i].key = "none";
		variableStore[i].value = "none";
	}
}

// This method frees up memory where the data with specified key was stored.
void mem_reset(char* key){
	// Iterate through each key-value pair in memory to find the requested key
	for (int i = 0; i < VARIABLE_STORE_SIZE; i++){
		if (strcmp(variableStore[i].key, key) == 0){
			// Set default values for key and value.
			variableStore[i].key = "none";
			variableStore[i].value = "none";
		} 
	}
}

// Set key value pair.
// Returns 1 if saving was successful, 0 otherwise.
int mem_set_value(char *var_in, char *value_in) {
	
	int i;

	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variableStore[i].key, var_in) == 0){
			variableStore[i].value = strdup(value_in);
			return 1;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variableStore[i].key, "none") == 0){
			variableStore[i].key = strdup(var_in);
			variableStore[i].value = strdup(value_in);
			return 1;
		} 
	}

	return 0;

}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<VARIABLE_STORE_SIZE; i++){
		if (strcmp(variableStore[i].key, var_in) == 0){
			return strdup(variableStore[i].value);
		} 
	}
	return "Variable does not exist";

}


// -----------
// FRAME STORE
// -----------

#define FRAME_SIZE 3

struct Frame{
	int isEmpty;
	char* lines[FRAME_SIZE];
	void (*clearFrame) (struct Frame* this);
	void (*loadFrame) (struct Frame* this, char* lines[]);
};

// This is a method of Frame which clears the frame.
void clearFrame(struct Frame* this){
	for(int i = 0; i < FRAME_SIZE; i++) this->lines[i] = NULL;
	this->isEmpty = 1;
}

// This is a method of Frame which loads the frame with the input lines.
// The input lines must contain exactly 3 elements.
void loadFrame(struct Frame* this, char* lines[]){
	for(int i = 0; i < FRAME_SIZE; i++) this->lines[i] = lines[i];
	this->isEmpty = 0;
}

struct Frame* frameStore[FRAME_STORE_SIZE / FRAME_SIZE];

// This method initializes the frame store.
void frameStore_init(){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		frameStore[i] = malloc(sizeof(struct Frame));
		frameStore[i]->clearFrame = clearFrame;
		frameStore[i]->loadFrame = loadFrame;
		frameStore[i]->clearFrame(frameStore[i]);
	}
}

// This method loads a page in the first empty frame of the store.
// The input lines must contain exactly 3 elements.
int loadInFirstEmptyFrame(char* lines[]){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		if(frameStore[i]->isEmpty){
			frameStore[i]->loadFrame(frameStore[i], lines);
			return i;
		}
	}
	return -1;
}

// Get a certain line from a certain frame.s
char* getFrameLine(int frame, int line){
	if(line >= 3 || line < 0 || frame < 0 || frame >= FRAME_STORE_SIZE / FRAME_SIZE) return NULL;
	return frameStore[frame]->lines[line];
}

// Delete a certain frame.
void deleteFrame(int frame){
	if(frame < 0 || frame >= FRAME_STORE_SIZE / FRAME_SIZE) return;
	frameStore[frame]->clearFrame(frameStore[frame]);
}