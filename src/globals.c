#include "../include/header.h"

Memory memory = { 0 };
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void _init_memory(){
    memory.page_size = getpagesize();

    const size_t fixed = sizeof(Fixed);
    if (fixed < (size_t)memory.page_size)
        memory.fixed_size = memory.page_size;
    else
        memory.fixed_size = memory.page_size
                          * (fixed / memory.page_size + 1);

    const size_t variable = sizeof(Variable);
    if (variable < (size_t)memory.page_size)
        memory.variable_size = memory.page_size;
    else
        memory.variable_size = memory.page_size
                             * (variable / memory.page_size + 1);
    memory.opt.tiny = TINY;
    memory.opt.small = SMALL;
    memory.opt.flag = MAP_PRIVATE;
    memory.opt.prot = PROT_READ | PROT_WRITE;

    _set_env(ALLOC, 0);
    _set_env(IN_USE, 0);
    _set_env(FREED, 0);
    _set_env(INTERN_ALLOC, 0);
    _set_env(INTERN_FREED, 0);
}
