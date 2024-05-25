#include "../include/header.h"

extern Memory memory;

Fixed* init_fixed(const size_t size){
    Fixed* const area = mmap(NULL, memory.fixed_size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (area == MAP_FAILED) return NULL;

    area->memory = mmapeNULL, size * STACK_BUFF,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE, -1, 0);
    if (area->memory == MAP_FAILED){
        munmap(area, memory.fixed_size);
        return NULL;
    }
    area->memory_start
        = (void*)(((size_t)area->memory + 0xf) & ~0xf);
    area->free = STACK_BUFF - 1;
    return area;
}

void* get_fixed(Fixed* area, const size_t type,
                const size_t size){
    while (area->next_ptr == STACK_BUFF - 1){
        if (area->next == NULL){
            area->next = init_fixed(type);
            if (!area->next) return NULL;
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
    area->ptr[index] = area->memory_start + type * index;
    return area->ptr[index];
}

void* use_cached(Variable* variable, const size_t size,
                 const size_t offset){
    variable->used = size;
    if (size + offset == variable->size)
        return variable->memory_start;

    Variable* const node = mmap(NULL, memory.variable_size,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (node == MAP_FAILED) return NULL;

    void* start = variable->memory_start + size;
    start = (void*)(((size_t)start + 0xf) & ~0xf);
    node->memory = start;
    node->memory_start = start;
    node->size = variable->memory + variable->size - start;
    node->used = 0;
    node->prev = variable;
    node->next = variable->next;

    if (variable->next) variable->next->prev = node;
    variable->next = node;
    variable->size -= node->size;
    return variable->memory_start;
}

void* get_variable(size_t size){
    Variable** area = &memory.variable;
    Variable* prev = NULL;
    while (*area){
        if (!(*area)->used){
            const size_t offset = (*area)->memory_start
                                - (*area)->memory;
            if (size <= (*area)->size - offset)
                return use_cached(*area, size, offset);
        }
        prev = *area;
        area = &(*area)->next;
    }
    *area = mmap(NULL, memory.variable_size,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (*area == MAP_FAILED) return NULL;

    (*area)->used = size;
    size += 0xf;
    if (size < memory.page_size) size = memory.page_size;
    else size = memory.page_size * (size / memory.page_size + 1);

    (*area)->memory = mmap(NULL, size,
                           PROT_READ | PROT_WRITE,
                           MAP_PRIVATE, -1, 0);
    if ((*area)->memory == MAP_FAILED){
        munmap(*area, memory.variable_size);
        return NULL;
    }
    (*area)->memory_start
        = (void*)(((size_t)(*area)->memory + 0xf) & ~0xf);
    (*area)->size = size;
    (*area)->prev = prev;
    if (prev) prev->next = *area;
    return (*area)->memory_start;
}

void* malloc(size_t size){
    void* ptr;
    if (size <= SMALL){
        size_t type;
        Fixed** area;
        if (size <= TINY){
            type = TINY;
            area = &memory.tiny;
        }
        else{
            type = SMALL;
            area = &memory.small;
        }
        if (!*area){
            *area = init_fixed(type);
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
