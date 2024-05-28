#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

int mallopt(int opt, int value){
    if (!memory.page_size) _init_memory();
    pthread_mutex_lock(&lock.opt);
    switch (opt){
        case TINY_SIZE:
            if (value % 2) break;
            memory.opt.tiny = value;
            break;
        case SMALL_MULT:
            memory.opt.small = memory.opt.tiny * value;
            break;
        case PROT:
            memory.opt.prot = value;
            break;
        case BZERO:
            memory.opt.bzero = value;
            break;
    }
    pthread_mutex_unlock(&lock.opt);
    return 1;
}
