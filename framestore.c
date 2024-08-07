#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "macros.h"
#include "framestore.h"

// This is a method of Frame which clears the frame.
// It sets the empty flag to true.
void clearFrame(struct Frame* this){
	for(int i = 0; i < FRAME_SIZE; i++) this->lines[i] = NULL;
	this->isEmpty = 1;
}

// This is a method of Frame which loads the frame with the input lines.
// It sets the empty flag to false.
// The input lines must contain exactly 3 elements.
void loadFrame(struct Frame* this, char* lines[]){
	for(int i = 0; i < FRAME_SIZE; i++) this->lines[i] = lines[i];
	this->isEmpty = 0;
}

// This method of Frame alerts the user that the current frame is the victim of a page fault.
// It prints an alert as well as the lines stored in the frame.
void alertVictim(struct Frame* this){
	printf("Page fault! Victim page contents:\n%s%s%sEnd of victim page contents.\n", this->lines[0], this->lines[1], this->lines[2]);
}

struct Frame* frameStore[FRAME_STORE_SIZE / FRAME_SIZE];

// This method initializes the frame store.
void frameStore_init(){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		frameStore[i] = malloc(sizeof(struct Frame));
		frameStore[i]->clearFrame = clearFrame;
		frameStore[i]->loadFrame = loadFrame;
		frameStore[i]->clearFrame(frameStore[i]);
		frameStore[i]->alertVictim = alertVictim;
	}
}

// This method looks for the first empty frame in the frame store and returns a pointer to it.
// If no empty frame is found, then it returns null.
struct Frame* findFirstEmptyFrame(){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		if(frameStore[i]->isEmpty) return frameStore[i];
	}
	return NULL;
}
