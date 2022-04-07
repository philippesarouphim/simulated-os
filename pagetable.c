#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "framestore.h"
#include "pagetable.h"

struct Page{
    struct Frame* frame;  
    struct Page** pageInTable;
    struct LRUCacheNode* pageInLRUCache;
    void (*clearPage)(struct Page* this, int fromLRUCache);
    char* (*getLine)(struct Page* this, int line);
};

struct LRUCache{
    struct LRUCacheNode* head;
    struct LRUCacheNode* tail;
    struct LRUCacheNode* (*addPage)(struct LRUCache* this, struct Page* page);
    struct Frame* (*findFirstEmptyFrameOrReplace) (struct LRUCache* this);
    void (*usePage) (struct LRUCache* this, struct LRUCacheNode* node);
    struct Frame* (*clearLRU) (struct LRUCache* this);
    void (*clearNode) (struct LRUCache* this, struct LRUCacheNode* node);
};

struct LRUCacheNode{
    struct Page* page;
    struct LRUCacheNode* next;
    struct LRUCacheNode* previous;
};

struct LRUCache* cache;

// This is a method of Page which clears the content of the page.
// It also upholds referential integrity by removing its pointers in the PageTable and the LRUCache.
// The second input <fromLRUCache> acts as a boolean to determine whether the pointer in the LRUCache should be deleted.
void clearPage(struct Page* this, int fromLRUCache){
    this->frame->clearFrame(this->frame);
    *(this->pageInTable) = NULL;
    if(!fromLRUCache) cache->clearNode(cache, this->pageInLRUCache);
}

// This method of Page fetches a single line from its associated Frame.
// It also alerts the LRUCache that the page has been used.
// If the specified line input is outside of the [0, 2] bounds, then the method returns null.
char* getLine(struct Page* this, int line){
    if(line < 0 || line >= 3) return NULL;
    cache->usePage(cache, this->pageInLRUCache);
    return this->frame->lines[line];
}

// This method of PageTable acts as a constructor for a Page struct.
// It creats the page object and adds a bidirectional association with the given PageTable and the LRUCache.
void create_Page(struct PageTable* this, int i, char* lines[]){
    struct Page* page = malloc(sizeof(struct Page));
    page->frame = cache->findFirstEmptyFrameOrReplace(cache);
    page->frame->loadFrame(page->frame, lines);
    page->pageInLRUCache = cache->addPage(cache, page);
    page->pageInTable = this->addPage(this, page, i);
    page->clearPage = clearPage;
    page->getLine = getLine;
}

// This method of PageTable adds a page to the table and returns a pointer to the pointer to the page in the PageTable.
struct Page** addPageToTable(struct PageTable* this, struct Page* page, int i){
    if(this->pages[i]) return NULL;
    this->pages[i] = page;
    return &this->pages[i];
}

// This method of PageTable clears all the pages in a table.
void clearAllPages(struct PageTable* this){
    for(int i = 0; i < MAX_PAGES; i++){
        if(this->pages[i]) this->pages[i]->clearPage(this->pages[i], 0);
    }
}

// This function finds the first empty frame in the frame store.
// If it does not find any, it makes a call to the LRUCache to free the least recently used frame and returns it.
struct Frame* findFirstEmptyFrameOrReplace(struct LRUCache* this){
    struct Frame* frame = findFirstEmptyFrame();
    if(frame) return frame;
    return this->clearLRU(this);
}

// This method of PageTable gets a certain line from a certain page in the table.
// If the page is not found, it returns PAGE_NOT_FOUND.
// If the line is not found, it returns null.
char* getLineFromPage(struct PageTable* this, int page, int line){
    if(!this->pages[page]) return PAGE_NOT_FOUND;
    return this->pages[page]->getLine(this->pages[page], line);
}

// This function is a constructor for the PageTable struct.
struct PageTable* create_PageTable(){
    struct PageTable* table = malloc(sizeof(struct PageTable));
    table->addPage = addPageToTable;
    table->createPage = create_Page;
    table->clearAllPages = clearAllPages;
    table->getLineFromPage = getLineFromPage;
    for(int i = 0; i < MAX_PAGES; i++) table->pages[i] = NULL;
    return table;
}

// This method is a constructor for the LRUCache's Node struct.
struct LRUCacheNode* create_LRUCacheNode(struct Page* page){
    struct LRUCacheNode* node = malloc(sizeof(struct LRUCacheNode));
    node->page = page;
    node->next = NULL;
    node->previous = NULL;
    return node;
}

// This method of LRUCache adds a page to the LRUCache by wrapping it in an LRUCacheNode.
struct LRUCacheNode* addPage(struct LRUCache* this, struct Page* page){
    struct LRUCacheNode* node = create_LRUCacheNode(page);
    if(this->tail){
        this->tail->next = node;
        node->next = NULL;
        node->previous = this->tail;
        this->tail = node;
    } else {
        this->head = node;
        this->tail = node;
    }
    return node;
}

// This method of LRUCache places the specified node at the tail of the queue.
// It is used for when a page has been used.
void usePage(struct LRUCache* this, struct LRUCacheNode* node){
    if(this->tail == node) return;
    if(this->head == node){
        this->head->next->previous = NULL;
        this->head = this->head->next;
    } else {
        node->previous->next = node->next;
        node->next->previous = node->previous;
    }
    node->next = NULL;
    this->tail->next = node;
    node->previous = this->tail;
    this->tail = this->tail->next;
}


// This method of LRUCache clears the least recently used frame and returns it.
// It also ensures no memory leaks and referential integrity.
struct Frame* clearLRU(struct LRUCache* this){
    if(!this->head) return NULL;
    struct Frame* frame = this->head->page->frame;
    frame->alertVictim(frame);
    this->head->page->clearPage(this->head->page, 1);
    free(this->head->page);
    if(this->head->next) this->head->next->previous = NULL;
    struct LRUCacheNode* head = this->head;
    this->head = this->head->next;
    free(head);
    return frame;
}

// This method of LRUCache removes a node from the LRUCache.
// It ensures no memory leaks.
// NOTE: it does not ensure referential integrity. The responsibility is left to the caller.
void clearNode(struct LRUCache* this, struct LRUCacheNode* node){
    if(node->next){
        node->next->previous = node->previous;
    } else {
        this->tail = this->tail->previous;
    }
    if(node->previous){
        node->previous->next = node->next;
    } else {
        this->head = this->head->next;
    }
    free(node);
}

// This function is a constructor for the LRUCache struct.
struct LRUCache* create_LRUCache(){
    struct LRUCache* cache = malloc(sizeof(struct LRUCache));
    cache->addPage = addPage;
    cache->clearLRU = clearLRU;
    cache->clearNode = clearNode;
    cache->findFirstEmptyFrameOrReplace = findFirstEmptyFrameOrReplace;
    cache->head = NULL;
    cache->tail = NULL;
    cache->usePage = usePage;
    return cache;
}

// This function initializes the shell's instance of the LRUCache.
void LRUCache_init(){
    cache = create_LRUCache();
}