#include "macros.h"

struct Frame{
	int isEmpty;
	char* lines[FRAME_SIZE];
	void (*clearFrame) (struct Frame*);
	void (*loadFrame) (struct Frame*, char* lines[]);
    void (*alertVictim) (struct Frame* this);
};

struct Frame* findFirstEmptyFrame();