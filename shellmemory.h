void mem_init();
void mem_reset(char* key);
char *mem_get_value(char *key);
int mem_set_value(char *key, char *value);

void frameStore_init();
int loadInFirstEmptyFrame(char* lines[]);
char* getFrameLine(int frame, int line);
void deleteFrame(int frame);