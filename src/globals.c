#include "../include/header.h"

Memory memory = { 0 };
Mutex lock = {
    .tiny = PTHREAD_MUTEX_INITIALIZER,
    .small = PTHREAD_MUTEX_INITIALIZER,
    .variable = PTHREAD_MUTEX_INITIALIZER,
    .opt = PTHREAD_MUTEX_INITIALIZER,
    .env = PTHREAD_MUTEX_INITIALIZER,
    .dump = PTHREAD_MUTEX_INITIALIZER,
    .init = PTHREAD_MUTEX_INITIALIZER
};

void _init_memory(){
    pthread_mutex_lock(&lock.init);
    if (memory.page_size){
        pthread_mutex_unlock(&lock.init);
        return;
    }
    memory.page_size = getpagesize();

    static const size_t fixed = sizeof(Fixed);
    if (fixed < (size_t)memory.page_size)
        memory.fixed_size = memory.page_size;
    else
        memory.fixed_size = memory.page_size
                          * (fixed / memory.page_size + 1);

    static const size_t variable = sizeof(Variable);
    if (variable < (size_t)memory.page_size)
        memory.variable_size = memory.page_size;
    else
        memory.variable_size = memory.page_size
                             * (variable / memory.page_size + 1);
    memory.opt.tiny = TINY;
    memory.opt.small = SMALL;
    memory.opt.prot = PROT_READ | PROT_WRITE;
    memory.opt.bzero = NO;

    _set_env(ALLOC, 0);
    _set_env(IN_USE, 0);
    _set_env(FREED, 0);
    _set_env(INTERN_ALLOC, 0);
    _set_env(INTERN_FREED, 0);
    pthread_mutex_unlock(&lock.init);
}
