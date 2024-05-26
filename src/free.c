#include "../include/header.h"

extern Memory memory;
extern pthread_mutex_t lock;

void _yes_we_free_fixed(Fixed* const area, size_t size,
                        const byte type){
    if (area->next) area->next->prev = area->prev;
    if (area->prev) area->prev->next = area->next;
    if (type == T_TINY){
        if (memory.tiny == area)
            memory.tiny = area->next;
    }
    else if (memory.small == area) memory.small = area->next;
    size *= STACK_BUFF;
    munmap(area->memory, size);
    munmap(area, memory.fixed_size);
    _set_env(FREED, size);
    _set_env(INTERN_FREED, memory.fixed_size);
}

static byte free_fixed(const void* const target, Fixed* area,
                const size_t size, const byte type){
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != target) continue;

            area->ptr[x] = NULL;
            _set_env(IN_USE, -area->used[x]);
            area->used[x] = 0;

            if (++area->free == STACK_BUFF - 1)
                _yes_we_free_fixed(area, size, type);
            else if (x < area->next_ptr) area->next_ptr = x;
            pthread_mutex_unlock(&lock);
            return SUCCESS;
        }
        area = area->next;
    }
    return FAILURE;
}

void _yes_we_free(Variable* const ptr){
    if (ptr->prev && !ptr->prev->used){
        ptr->memory = ptr->prev->memory;
        ptr->memory_start = ptr->prev->memory_start;
        ptr->size += ptr->prev->size;
        ptr->prev = ptr->prev->prev;
        munmap(ptr->prev, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
    }
    if (ptr->next && !ptr->next->used){
        ptr->size += ptr->next->size;
        ptr->next = ptr->next->next;
        munmap(ptr->next, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
    }
    if (!ptr->prev && !ptr->next){
        munmap(ptr->memory, ptr->size);
        munmap(ptr, memory.variable_size);
        memory.variable = NULL;
        _set_env(FREED, ptr->size);
        _set_env(INTERN_FREED, memory.variable_size);
    }
}

void free(void* ptr){
    if (!ptr) return;
    pthread_mutex_lock(&lock);

    if (free_fixed(ptr, memory.tiny, ptr->size, T_TINY) == SUCCESS)
        return;
    if (free_fixed(ptr, memory.small, ptr->size, T_SMALL) == SUCCESS)
        return;

    for (const Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        _set_env(IN_USE, -variable->used);
        variable->used = 0;
        _yes_we_free(variable);
        pthread_mutex_unlock(&lock);
        return;
    }
    pthread_mutex_unlock(&lock);
    static const char* const err = "free(): invalid pointer\n";
    write(STDERR, err, 25);
}
