#include "../include/header.h"

extern Memory memory;
extern pthread_mutex_t lock;

static Fixed* init_fixed(size_t size){
    Fixed* const area = mmap(NULL, memory.fixed_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (area == MAP_FAILED) return NULL;
    _set_env(INTERN_ALLOC, memory.fixed_size);

    size *= STACK_BUFF;
    area->memory = mmap(NULL, size, memory.opt.prot,
                        memory.opt.flag, -1, 0);
    if (area->memory == MAP_FAILED){
        munmap(area, memory.fixed_size);
        _set_env(INTERN_FREED, memory.fixed_size);
        return NULL;
    }
    _set_env(ALLOC, size);
    area->memory_start = (void*)(((size_t)area->memory + 0xf)
                       & ~0xf);
    area->free = STACK_BUFF - 1;
    return area;
}

static void* get_fixed(Fixed* area, const size_t type,
                       const size_t size){
    pthread_mutex_lock(&lock);
    while (area->next_ptr == STACK_BUFF - 1){
        if (!area->next){
            area->next = init_fixed(type);
            if (!area->next){
                pthread_mutex_unlock(&lock);
                return NULL;
            }
            area->next->prev = area;
        }
        area = area->next;
    }
    const size_t index = area->next_ptr;
    while (area->ptr[area->next_ptr]
        && area->next_ptr < STACK_BUFF - 1)
        area->next_ptr++

    area->free--;
    area->used[index] = size;
    _set_env(IN_USE, size);

    area->ptr[index] = area->memory_start + area->size * index;
    pthread_mutex_unlock(&lock);
    return area->ptr[index];
}

static void* use_existing(Variable* const variable,
                          const size_t size, const size_t offset){
    variable->used = size;
    _set_env(IN_USE, size);
    if (size + offset == variable->size){
        pthread_mutex_unlock(&lock);
        return variable->memory_start;
    }
    Variable* const node = mmap(NULL, memory.variable_size,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (node == MAP_FAILED){
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    _set_env(INTERN_ALLOC, memory.variable_size);

    node->memory = (void*)(((size_t)variable->memory_start
                 + variable->used + size + 0xf) & ~0xf);
    node->memory_start = node->memory;
    node->size = variable->memory + variable->size - node->memory;
    node->prev = variable;
    node->next = variable->next;

    if (variable->next) variable->next->prev = node;
    variable->next = node;
    variable->size -= node->size;
    pthread_mutex_unlock(&lock);
    return variable->memory_start;
}

static void* get_variable(size_t size){
    pthread_mutex_lock(&lock);
    Variable** area = &memory.variable;
    const Variable* prev = NULL;
    size_t total;
    size_t offset;
    while (*area){
        total = size + (*area)->used + 0xf;
        if (total <= (*area)->size{
            offset = (*area)->memory_start - (*area)->memory;
            if (total <= (*area)->size - offset)
                return use_existing(*area, size, offset);
        }
        prev = *area;
        area = &(*area)->next;
    }
    *area = mmap(NULL, memory.variable_size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*area == MAP_FAILED){
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    _set_env(INTERN_ALLOC, memory.variable_size);

    (*area)->used = size;
    _set_env(IN_USE, size);
    size += 0xf;
    if (size < memory.page_size) size = memory.page_size;
    else size = memory.page_size * (size / memory.page_size + 1);

    (*area)->memory = mmap(NULL, size, memory.opt.prot,
                           memory.opt.flag, -1, 0);
    if ((*area)->memory == MAP_FAILED){
        munmap(*area, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    _set_env(ALLOC, size);
    (*area)->memory_start = (void*)(((size_t)(*area)->memory + 0xf)
                          & ~0xf);
    (*area)->size = size;
    (*area)->prev = prev;
    if (prev) prev->next = *area;
    pthread_mutex_unlock(&lock);
    return (*area)->memory_start;
}

void* malloc(size_t size){
    void* ptr;
    if (size <= memory.opt.small){
        size_t type;
        Fixed** area;
        if (size <= memory.opt.tiny){
            type = memory.opt.tiny;
            area = &memory.tiny;
        }
        else{
            type = memory.opt.small;
            area = &memory.small;
        }
        if (!*area){
            pthread_mutex_lock(&lock);
            *area = init_fixed(type);
            pthread_mutex_unlock(&lock);
            if (!*area){
                errno = ENOMEM;
                return NULL;
            }
        }
        ptr = get_fixed(*area, type, size);
    }
    else ptr = get_variable(size);
    if (!ptr) errno = ENOMEM;
    return ptr;
}
