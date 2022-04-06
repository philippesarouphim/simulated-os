#include "macros.h"

void LRUCache_init();

struct PageTable{
    struct Page* pages[MAX_PAGES];
    void (*createPage)(struct PageTable*, int i, char* lines[]);
    struct Page** (*addPage) (struct PageTable*, struct Page* page, int i);
    void (*clearAllPages) (struct PageTable*);
    char* (*getLineFromPage) (struct PageTable*, int page, int line);
};