#include "../include/header.h"

extern Memory memory;
extern pthread_mutex_t lock;

int mallopt(int opt, int value){
    pthread_mutex_lock(&lock);
    switch (opt){
        case TINY_SIZE:
            if (value % 2){
                pthread_mutex_unlock(&lock);
                return 0;
            }
            memory.opt.tiny = value;
            break;
        case SMALL_MULT:
            memory.opt.small = memory.opt.tiny * value;
            break;
        case BZERO:
            memory.opt.flag = value ?
                MAP_ANONYMOUS | MAP_PRIVATE :
                MAP_PRIVATE;
            break;
        case PROT:
            memory.opt.prot = value;
            break;
        default:
            pthread_mutex_unlock(&lock);
            return 0;
    }
    pthread_mutex_unlock(&lock);
    return 1;
}
