#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

void* calloc(size_t nmemb, size_t size){
    void* const ptr = malloc(nmemb * size);
    if (!ptr) return NULL;

    pthread_mutex_t* mptr;
    pthread_mutex_lock(&lock.opt);
    if (size <= (size_t)memory.opt.tiny) mptr = &lock.tiny;
    else if (size <= (size_t)memory.opt.small) mptr = &lock.small;
    else mptr = &lock.variable;
    pthread_mutex_unlock(&lock.opt);

    pthread_mutex_lock(mptr);
    for (size_t x = 0; x < nmemb * size; x++) ((byte*)ptr)[x] = 0;
    pthread_mutex_unlock(mptr);
    return ptr;
}
