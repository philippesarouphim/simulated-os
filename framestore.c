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

struct Frame* findFirstEmptyFrame(){
	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
		if(frameStore[i]->isEmpty) return frameStore[i];
	}
	return NULL;
}

// This method loads a page in the first empty frame of the store.
// The input lines must contain exactly 3 elements.
// struct Frame* loadInFirstEmptyFrame(char* lines[]){
// 	for(int i = 0; i < FRAME_STORE_SIZE / FRAME_SIZE; i++){
// 		if(frameStore[i]->isEmpty){
// 			frameStore[i]->loadFrame(frameStore[i], lines);
// 			return frameStore[i];
// 		}
// 	}
// 	return NULL;
// }

// Get a certain line from a certain frame.s
// char* getFrameLine(int frame, int line){
// 	if(line >= 3 || line < 0 || frame < 0 || frame >= FRAME_STORE_SIZE / FRAME_SIZE) return NULL;
// 	return frameStore[frame]->lines[line];
// }

// // Delete a certain frame.
// void deleteFrame(int frame){
// 	if(frame < 0 || frame >= FRAME_STORE_SIZE / FRAME_SIZE) return;
// 	frameStore[frame]->clearFrame(frameStore[frame]);
// }