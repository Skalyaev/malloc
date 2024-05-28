#include "../include/header.h"

extern Memory memory;
extern Mutex lock;
#include <stdio.h>

static Fixed* init_fixed(size_t size){
    Fixed* const area = mmap(NULL, memory.fixed_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (area == MAP_FAILED) return NULL;
    _set_env(INTERN_ALLOC, memory.fixed_size);

    area->size = size;
    size *= STACK_BUFF;
    pthread_mutex_lock(&lock.opt);
    area->memory = mmap(NULL, size, memory.opt.prot,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    pthread_mutex_unlock(&lock.opt);

    if (area->memory == MAP_FAILED){
        perror("mmap");
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
    while (area->next_ptr == STACK_BUFF - 1){
        if (!area->next){
            area->next = init_fixed(type);
            if (!area->next) return NULL;
            area->next->prev = area;
        }
        area = area->next;
    }
    const size_t index = area->next_ptr;
    area->next_ptr++;
    while (area->used[area->next_ptr]
        && area->next_ptr < STACK_BUFF - 1)
        area->next_ptr++;

    area->free--;
    area->used[index] = size;
    _set_env(IN_USE, size);

    if (!area->ptr[index])
        area->ptr[index] = area->memory_start + area->size * index;
    pthread_mutex_lock(&lock.opt);
    if (memory.opt.bzero) ft_bzero(area->ptr[index], size);
    pthread_mutex_unlock(&lock.opt);
    return area->ptr[index];
}

static void* use_existing(Variable* const variable,
                          const size_t size){
    _set_env(IN_USE, size);
    if (!variable->used){
        variable->used = size;
        return variable->memory_start;
    }
    Variable* const node = mmap(NULL, memory.variable_size,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (node == MAP_FAILED) return NULL;
    _set_env(INTERN_ALLOC, memory.variable_size);

    node->memory = (void*)(((size_t)variable->memory_start
                 + variable->used + 0xf) & ~0xf);
    node->memory_start = node->memory;
    node->size = variable->memory + variable->size - node->memory;
    node->used = size;
    node->prev = variable;
    node->next = variable->next;

    if (variable->next) variable->next->prev = node;
    variable->next = node;
    variable->size -= node->size;
    pthread_mutex_lock(&lock.opt);
    if (memory.opt.bzero) ft_bzero(node->memory_start, node->size);
    pthread_mutex_unlock(&lock.opt);
    return node->memory_start;
}

static void* get_variable(size_t size){
    Variable** area = &memory.variable;
    Variable* prev = NULL;
    while (*area){
        if ((*area)->used + size + 0xf <= (*area)->size
            - ((*area)->memory_start - (*area)->memory))
            return use_existing(*area, size);
        prev = *area;
        area = &(*area)->next;
    }
    *area = mmap(NULL, memory.variable_size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*area == MAP_FAILED) return NULL;
    _set_env(INTERN_ALLOC, memory.variable_size);

    (*area)->used = size;
    _set_env(IN_USE, size);
    size += 0xf;
    if (size < (size_t)memory.page_size) size = memory.page_size;
    else if (size > (size_t)memory.page_size)
        size = memory.page_size * (size / memory.page_size + 1);

    pthread_mutex_lock(&lock.opt);
    (*area)->memory = mmap(NULL, size, memory.opt.prot,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    pthread_mutex_unlock(&lock.opt);

    if ((*area)->memory == MAP_FAILED){
        munmap(*area, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
        return NULL;
    }
    _set_env(ALLOC, size);
    (*area)->memory_start = (void*)(((size_t)(*area)->memory + 0xf)
                          & ~0xf);
    (*area)->size = size;
    (*area)->prev = prev;
    if (prev) prev->next = *area;
    return (*area)->memory_start;
}

void* malloc(size_t size){
    if (!memory.page_size) _init_memory();

    void* ptr;
    pthread_mutex_lock(&lock.opt);
    if (size <= (size_t)memory.opt.small){
        size_t type;
        Fixed** area;
        pthread_mutex_t* mptr;
        if (size <= (size_t)memory.opt.tiny){
            type = memory.opt.tiny;
            area = &memory.tiny;
            mptr = &lock.tiny;
        }
        else{
            type = memory.opt.small;
            area = &memory.small;
            mptr = &lock.small;
        }
        pthread_mutex_unlock(&lock.opt);
        pthread_mutex_lock(mptr);
        if (!*area){
            *area = init_fixed(type);
            if (!*area){
                pthread_mutex_unlock(mptr);
                errno = ENOMEM;
                return NULL;
            }
        }
        ptr = get_fixed(*area, type, size);
        pthread_mutex_unlock(mptr);
    }
    else{
        pthread_mutex_unlock(&lock.opt);
        pthread_mutex_lock(&lock.variable);
        ptr = get_variable(size);
        pthread_mutex_unlock(&lock.variable);
    }
    if (!ptr) errno = ENOMEM;
    return ptr;
}
