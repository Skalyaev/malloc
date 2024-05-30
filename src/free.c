#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

void _yes_we_free_fixed(Fixed* const ptr,
                        pthread_mutex_t* const mptr){
    if (!ptr || !memory.page_size) return;

    pthread_mutex_lock(mptr);
    ptr->prev->next = ptr->next;
    if (ptr->next) ptr->next->prev = ptr->prev;

    _set_env(ALLOC, -ptr->size);
    _set_env(FREED, ptr->size);
    _set_env(INTERN_ALLOC, -memory.fixed_size);
    _set_env(INTERN_FREED, memory.fixed_size);

    munmap(ptr->memory, ptr->size);
    munmap(ptr, memory.fixed_size);
    pthread_mutex_unlock(mptr);
}

static byte free_fixed(const void* const target, const byte type){
    Fixed* area;
    pthread_mutex_t* mptr;
    switch (type){
        case T_TINY:
            mptr = &lock.tiny;
            pthread_mutex_lock(mptr);
            area = memory.tiny;
            break;
        case T_SMALL:
            mptr = &lock.small;
            pthread_mutex_lock(mptr);
            area = memory.small;
            break;
    }
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != target) continue;

            _set_env(IN_USE, -area->used[x]);
            if (!--area->in_use && area->prev){
                pthread_mutex_unlock(mptr);
                _yes_we_free_fixed(area, mptr);
            }
            else{
                area->used[x] = 0;
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
    if (!ptr || !memory.page_size) return;

    pthread_mutex_lock(&lock.variable);
    ptr->prev->next = ptr->next;
    if (ptr->next) ptr->next->prev = ptr->prev;

    size_t total = 0;
    for (ushort y = 0; y < BIG_STACK_BUFF; y++){
        if (!ptr->size[y]) continue;
        total += ptr->size[y];
        munmap(ptr->memory[y], ptr->size[y]);
    }
    _set_env(ALLOC, -total);
    _set_env(FREED, total);
    _set_env(INTERN_ALLOC, -memory.variable_size);
    _set_env(INTERN_FREED, memory.variable_size);

    munmap(ptr, memory.variable_size);
    pthread_mutex_unlock(&lock.variable);
}

void free(void* ptr){
    if (!ptr) return;
    if (!memory.page_size){
        pthread_mutex_lock(&lock.print);
        ft_putstr("free(): invalid pointer\n", STDERR);
        pthread_mutex_unlock(&lock.print);
    }
    if (free_fixed(ptr, T_TINY) == SUCCESS) return;
    if (free_fixed(ptr, T_SMALL) == SUCCESS) return;

    pthread_mutex_lock(&lock.variable);
    Variable* variable = memory.variable;
    while (variable){
        for (ushort x = 0; x < BIG_STACK_BUFF; x++){
            if (ptr != variable->memory_start[x]) continue;

            _set_env(IN_USE, -variable->used[x]);
            if (!--variable->in_use && variable->prev){
                pthread_mutex_unlock(&lock.variable);
                return _yes_we_free(variable);
            }
            else{
                variable->used[x] = 0;
                if (x < variable->next_ptr) variable->next_ptr = x;
                pthread_mutex_unlock(&lock.variable);
            }
            return;
        }
        variable = variable->next;
    }
    pthread_mutex_unlock(&lock.variable);

    pthread_mutex_lock(&lock.print);
    ft_putstr("free(): invalid pointer\n", STDERR);
    pthread_mutex_unlock(&lock.print);
}
