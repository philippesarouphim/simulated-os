#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "macros.h"
#include "framestore.h"

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

struct Frame* findFirstEmptyFrame(){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		if(frameStore[i]->isEmpty) return frameStore[i];
	}
	return NULL;
}
