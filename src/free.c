#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

void _yes_we_free_fixed(Fixed* const area, const byte type){
    if (!area) return;
    if (!memory.page_size) _init_memory();

    pthread_mutex_t* const mptr = type == T_TINY ?
                                  &lock.tiny : &lock.small;
    pthread_mutex_lock(mptr);
    Fixed** const tptr = type == T_TINY ?
                         &memory.tiny : &memory.small;
    if (area->next) area->next->prev = area->prev;
    if (area->prev) area->prev->next = area->next;
    if (*tptr == area) *tptr = area->next;

    const size_t size = area->size * STACK_BUFF;
    munmap(area->memory, size);
    munmap(area, memory.fixed_size);
    pthread_mutex_unlock(mptr);
    _set_env(ALLOC, -size);
    _set_env(FREED, size);
    _set_env(INTERN_FREED, memory.fixed_size);
}

static byte free_fixed(const void* const target, const byte type){
    pthread_mutex_t* const mptr = type == T_TINY ?
                                  &lock.tiny : &lock.small;
    pthread_mutex_lock(mptr);
    Fixed* area = type == T_TINY ?
                  memory.tiny : memory.small;
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != target) continue;

            _set_env(IN_USE, -area->used[x]);
            area->used[x] = 0;

            if (++area->free == STACK_BUFF - 1){
                pthread_mutex_unlock(mptr);
                _yes_we_free_fixed(area, type);
            }
            else{
                if (x < area->next_ptr) area->next_ptr = x;
                pthread_mutex_unlock(mptr);
            }
            return SUCCESS;
        }
        area = area->next;
    }
    pthread_mutex_unlock(mptr);
    return FAILURE;
}

void _yes_we_free(Variable* const ptr){
    if (!ptr) return;
    if (!memory.page_size) _init_memory();

    Variable* tmp;
    pthread_mutex_lock(&lock.variable);
    if (ptr->prev
        && ptr->prev->memory + ptr->prev->size == ptr->memory){

        ptr->memory = ptr->prev->memory;
        ptr->memory_start = ptr->prev->memory_start;
        ptr->size += ptr->prev->size;
        tmp = ptr->prev;
        ptr->prev = ptr->prev->prev;
        if (ptr->prev) ptr->prev->next = ptr;
        munmap(tmp, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
    }
    if (ptr->next && !ptr->next->used
        && ptr->memory + ptr->size == ptr->next->memory){

        ptr->size += ptr->next->size;
        tmp = ptr->next;
        ptr->next = ptr->next->next;
        if (ptr->next) ptr->next->prev = ptr;
        munmap(tmp, memory.variable_size);
        _set_env(INTERN_FREED, memory.variable_size);
    }
    for (tmp = memory.variable; tmp; tmp = tmp->next){
    }
    _set_env(ALLOC, -ptr->size);
    _set_env(FREED, ptr->size);
    _set_env(INTERN_FREED, memory.variable_size);
    munmap(ptr->memory, ptr->size);
    munmap(ptr, memory.variable_size);
    memory.variable = NULL;
    pthread_mutex_unlock(&lock.variable);
}

void free(void* ptr){
    if (!ptr) return;
    if (!memory.page_size) _init_memory();

    if (free_fixed(ptr, T_TINY) == SUCCESS) return;
    if (free_fixed(ptr, T_SMALL) == SUCCESS) return;

    pthread_mutex_lock(&lock.variable);
    for (Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        _set_env(IN_USE, -variable->used);
        variable->used = 0;
        pthread_mutex_unlock(&lock.variable);
        _yes_we_free(variable);
        return;
    }
    pthread_mutex_unlock(&lock.variable);
    write(STDERR, "free(): invalid pointer\n", 24);
}
