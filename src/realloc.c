#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

static byte realloc_fixed(void** const ptr, const size_t size,
                          const byte type){
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
    size_t x, y;
    while (area){
        for (x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != *ptr) continue;

            if ((type == T_TINY && size <= (size_t)area->size)
               || (type == T_SMALL
                  && size <= (size_t)area->size
                  && size > (size_t)memory.opt.tiny)){

                _set_env(IN_USE, size - area->used[x]);
                area->used[x] = size;
                add2history("realloc ", *ptr, size);
                pthread_mutex_unlock(mptr);
                return SUCCESS;
            }
            pthread_mutex_unlock(mptr);
            void* const new_ptr = malloc(size);
            if (!new_ptr) return -1;

            pthread_mutex_t* new_mptr = NULL;
            pthread_mutex_lock(&lock.opt);
            if (size <= (size_t)memory.opt.tiny)
                new_mptr = &lock.tiny;
            else if (size <= (size_t)memory.opt.small)
                new_mptr = &lock.small;
            else new_mptr = &lock.variable;

            pthread_mutex_lock(new_mptr);
            pthread_mutex_lock(mptr);
            for (y = 0; y < area->used[x] && y < size; y++)
                ((byte*)new_ptr)[y] = ((byte*)area->ptr[x])[y];
            if (memory.opt.bzero)
                while (y < size) ((byte*)new_ptr)[y++] = 0;
            *ptr = new_ptr;
            pthread_mutex_unlock(new_mptr);
            pthread_mutex_unlock(&lock.opt);

            add2history("free ", area->ptr[x], area->used[x]);
            _set_env(IN_USE, -area->used[x]);
            if (--area->in_use && area->prev){
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

static void* variable2fixed(Variable* const ptr,
                              const ushort x, size_t size){
    void* const new_ptr = malloc(size);
    if (!new_ptr) return NULL;

    pthread_mutex_t* mptr;
    pthread_mutex_lock(&lock.opt);
    if (size <= (size_t)memory.opt.tiny) mptr = &lock.tiny;
    else mptr = &lock.small;

    pthread_mutex_lock(&lock.variable);
    pthread_mutex_lock(mptr);
    size_t y;
    for (y = 0; y < ptr->used[x] && y < size; y++)
        ((byte*)new_ptr)[y] = ((byte*)ptr->memory[x])[y];
    if (memory.opt.bzero)
        while (y < size) ((byte*)new_ptr)[y++] = 0;
    pthread_mutex_unlock(mptr);
    pthread_mutex_unlock(&lock.opt);

    add2history("free ", ptr->memory[x], ptr->used[x]);
    _set_env(IN_USE, -ptr->used[x]);
    if (!--ptr->in_use && ptr->prev){
        pthread_mutex_unlock(&lock.variable);
        _yes_we_free(ptr);
    }
    else{
        ptr->used[x] = 0;
        if (x < ptr->next_ptr) ptr->next_ptr = x;
        pthread_mutex_unlock(&lock.variable);
    }
    return new_ptr;
}

static void* realloc_variable(Variable* const ptr,
                              const ushort x, size_t size){
    const size_t used = size;
    if (size < (size_t)memory.page_size) size = memory.page_size;
    else size = memory.page_size * (size / memory.page_size + 1);

    pthread_mutex_lock(&lock.opt);
    void* const new_memory = mmap(NULL, size, memory.opt.prot,
                                  MAP_PRIVATE | MAP_ANONYMOUS,
                                  -1, 0);
    if (new_memory == MAP_FAILED){
        pthread_mutex_unlock(&lock.opt);
        pthread_mutex_unlock(&lock.variable);
        return NULL;
    }
    size_t y;
    for (y = 0; y < ptr->used[x] && y < used; y++)
        ((byte*)new_memory)[y] = ((byte*)ptr->memory[x])[y];
    if (memory.opt.bzero)
        while (y < used) ((byte*)new_memory)[y++] = 0;
    pthread_mutex_unlock(&lock.opt);

    add2history("malloc ", new_memory, used);
    _set_env(IN_USE, used - ptr->used[x]);
    _set_env(ALLOC, size - ptr->size[x]);
    _set_env(FREED, ptr->size[x]);
    munmap(ptr->memory[x], ptr->size[x]);
    add2history("free ", ptr->memory[x], ptr->size[x]);

    ptr->memory[x] = new_memory;
    if ((size_t)new_memory % 2)
        ptr->memory_start[x] =
            (void*)(((size_t)new_memory + 0xf) & ~0xf);
    else ptr->memory_start[x] = new_memory;

    ptr->used[x] = used;
    ptr->size[x] = size;
    pthread_mutex_unlock(&lock.variable);
    return ptr->memory_start[x];
}

void* realloc(void* ptr, size_t size){
    if (!ptr) return malloc(size);
    if (!size){
        free(ptr);
        return NULL;
    }
    if (!memory.page_size){
        pthread_mutex_lock(&lock.print);
        ft_putstr("double free or corruption (out)\n", STDERR);
        pthread_mutex_unlock(&lock.print);
    }
    byte code = realloc_fixed(&ptr, size, T_TINY);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    code = realloc_fixed(&ptr, size, T_SMALL);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    pthread_mutex_lock(&lock.variable);
    Variable* variable = memory.variable;
    while (variable){
        for (ushort x = 0; x < BIG_STACK_BUFF; x++){
            if (ptr != variable->memory_start[x]) continue;
            if (size < variable->size[x]){

                pthread_mutex_lock(&lock.opt);
                if (size <= (size_t)memory.opt.small){
                    pthread_mutex_unlock(&lock.opt);
                    pthread_mutex_unlock(&lock.variable);
                    return variable2fixed(variable, x, size);
                }
                pthread_mutex_unlock(&lock.opt);

                _set_env(IN_USE, size - variable->used[x]);
                variable->used[x] = size;
                add2history("realloc ", ptr, size);
                pthread_mutex_unlock(&lock.variable);
                return ptr;
            }
            else if (size == variable->size[x]){
                pthread_mutex_unlock(&lock.variable);
                return ptr;
            }
            return realloc_variable(variable, x, size);
        }
        variable = variable->next;
    }
    pthread_mutex_unlock(&lock.variable);

    pthread_mutex_lock(&lock.print);
    ft_putstr("double free or corruption (out)\n", STDERR);
    pthread_mutex_unlock(&lock.print);
    return NULL;
}
