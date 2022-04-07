struct pcb{
    int pid;
    int score;
    int counter;

    char* code_file_name;
    FILE* code_file;
    int pageCounter;
    struct PageTable* pageTable;
    int endReached;

    int (*load_next_page_into_memory) (struct pcb* this);
    void (*load_all_pages_into_memory) (struct pcb* this);
    void (*load_next_n_pages_into_memory) (struct pcb* this, int n);
    int (*execute_next) (struct pcb* this);
    int (*execute_next_n) (struct pcb* this, int num);
    void (*execute_until_end) (struct pcb* this);
    void (*free_memory) (struct pcb* this);
};

struct pcb* create_pcb(int i, char* code_file);