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

void clearPage(struct Page* this, int fromLRUCache){
    this->frame->clearFrame(this->frame);
    *(this->pageInTable) = NULL;
    if(!fromLRUCache) cache->clearNode(cache, this->pageInLRUCache);
}

char* getLine(struct Page* this, int line){
    if(line < 0 || line >= 3) return NULL;
    cache->usePage(cache, this->pageInLRUCache);
    return this->frame->lines[line];
}

void create_Page(struct PageTable* this, int i, char* lines[]){
    struct Page* page = malloc(sizeof(struct Page));
    page->frame = cache->findFirstEmptyFrameOrReplace(cache);
    page->frame->loadFrame(page->frame, lines);
    page->pageInLRUCache = cache->addPage(cache, page);
    page->pageInTable = this->addPage(this, page, i);
    page->clearPage = clearPage;
    page->getLine = getLine;
}

struct Page** addPageToTable(struct PageTable* this, struct Page* page, int i){
    if(this->pages[i]) return NULL;
    this->pages[i] = page;
    return &this->pages[i];
}

void clearAllPages(struct PageTable* this){
    for(int i = 0; i < MAX_PAGES; i++){
        if(this->pages[i]) this->pages[i]->clearPage(this->pages[i], 0);
    }
}

struct Frame* findFirstEmptyFrameOrReplace(struct LRUCache* this){
    struct Frame* frame = findFirstEmptyFrame();
    if(frame) return frame;
    return this->clearLRU(this);
}

char* getLineFromPage(struct PageTable* this, int page, int line){
    if(!this->pages[page]) return PAGE_NOT_FOUND;
    return this->pages[page]->getLine(this->pages[page], line);
}

struct PageTable* create_PageTable(){
    struct PageTable* table = malloc(sizeof(struct PageTable));
    table->addPage = addPageToTable;
    table->createPage = create_Page;
    table->clearAllPages = clearAllPages;
    table->getLineFromPage = getLineFromPage;
    for(int i = 0; i < MAX_PAGES; i++) table->pages[i] = NULL;
    return table;
}

struct LRUCacheNode* create_LRUCacheNode(struct Page* page){
    struct LRUCacheNode* node = malloc(sizeof(struct LRUCacheNode));
    node->page = page;
    node->next = NULL;
    node->previous = NULL;
    return node;
}

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

void LRUCache_init(){
    cache = create_LRUCache();
}