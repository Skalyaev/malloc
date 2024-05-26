#include "../include/header.h"

extern pthread_mutex_t lock;

void* calloc(size_t nmemb, size_t size){
    void* const ptr = malloc(nmemb * size);
    if (!ptr) return NULL;

    pthread_mutex_lock(&lock);
    for (size_t x = 0; x < nmemb * size; x++) ((byte*)ptr)[x] = 0;
    pthread_mutex_unlock(&lock);
    return ptr;
}
