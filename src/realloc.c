#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

static byte realloc_fixed(void** const ptr, const size_t size,
                          const byte type){
    pthread_mutex_t* const mptr = type == T_TINY ?
                                  &lock.tiny : &lock.small;
    pthread_mutex_lock(mptr);
    Fixed* area = type == T_TINY ?
                  memory.tiny : memory.small;
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != *ptr) continue;

            if ((type == T_TINY && size <= (size_t)area->size)
                || (type == T_SMALL
                    && size <= (size_t)area->size
                    && size > (size_t)memory.opt.tiny)){
                _set_env(IN_USE, size - area->used[x]);
                area->used[x] = size;
                pthread_mutex_unlock(mptr);
                return SUCCESS;
            }
            pthread_mutex_unlock(mptr);
            void* const new_ptr = malloc(size);
            if (!new_ptr) return -1;

            pthread_mutex_t* new_mptr = NULL;
            pthread_mutex_lock(&lock.opt);
            if (size <= (size_t)memory.opt.tiny && type != T_TINY)
                new_mptr = &lock.tiny;
            else if (size <= (size_t)memory.opt.small
                    && type != T_SMALL)
                new_mptr = &lock.small;
            else new_mptr = &lock.variable;
            pthread_mutex_unlock(&lock.opt);

            if (new_mptr) pthread_mutex_lock(new_mptr);
            pthread_mutex_lock(mptr);
            for (size_t y = 0; y < area->used[x]; y++)
                ((byte*)new_ptr)[y] = ((byte*)area->ptr[x])[y];
            *ptr = new_ptr;
            if (new_mptr) pthread_mutex_unlock(new_mptr);

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

static void* variable2fixed(Variable* const variable,
                            const size_t size){
    void* const ptr = malloc(size);
    if (!ptr) return NULL;

    pthread_mutex_t* mptr;
    pthread_mutex_lock(&lock.opt);
    if (size <= (size_t)memory.opt.tiny) mptr = &lock.tiny;
    else if (size <= (size_t)memory.opt.small) mptr = &lock.small;
    else mptr = &lock.variable;
    pthread_mutex_unlock(&lock.opt);

    pthread_mutex_lock(&lock.variable);
    pthread_mutex_lock(mptr);
    for (size_t x = 0; x < variable->used && x < size; x++)
        ((byte*)ptr)[x] = ((byte*)variable->memory)[x];
    pthread_mutex_unlock(mptr);

    _set_env(IN_USE, -variable->used);
    variable->used = 0;
    pthread_mutex_unlock(&lock.variable);
    _yes_we_free(variable);
    return ptr;
}

static void* realloc_variable(Variable* const ptr, size_t size){
    const size_t used = size;
    size += 0xf;
    if (size < (size_t)memory.page_size) size = memory.page_size;
    else size = memory.page_size * (size / memory.page_size + 1);

    pthread_mutex_lock(&lock.opt);
    void* const new_memory = mmap(NULL, size, memory.opt.prot,
                                  MAP_PRIVATE | MAP_ANONYMOUS,
                                  -1, 0);
    pthread_mutex_unlock(&lock.opt);
    if (new_memory == MAP_FAILED) return NULL;
    _set_env(ALLOC, size);

    pthread_mutex_lock(&lock.variable);
    for (size_t x = 0; x < ptr->used && x < used; x++)
        ((byte*)new_memory)[x] = ((byte*)ptr->memory)[x];

    munmap(ptr->memory, ptr->size);
    _set_env(ALLOC, -ptr->size);
    _set_env(FREED, ptr->size);

    ptr->memory = new_memory;
    ptr->memory_start = (void*)(((size_t)new_memory + 0xf)
                      & ~0xf);
    _set_env(IN_USE, used - ptr->used);
    ptr->used = used;
    ptr->size = size;
    pthread_mutex_unlock(&lock.variable);
    return ptr->memory_start;
}

void* realloc(void* ptr, size_t size){
    if (!ptr) return malloc(size);
    if (!size){
        free(ptr);
        return NULL;
    }
    if (!memory.page_size) _init_memory();

    byte code = realloc_fixed(&ptr, size, T_TINY);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    code = realloc_fixed(&ptr, size, T_SMALL);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    pthread_mutex_lock(&lock.variable);
    for (Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        if (size <= variable->size){
            pthread_mutex_lock(&lock.opt);
            if (size <= (size_t)memory.opt.small){
                pthread_mutex_unlock(&lock.opt);
                pthread_mutex_unlock(&lock.variable);
                return variable2fixed(variable, size);
            }
            pthread_mutex_unlock(&lock.opt);

            _set_env(IN_USE, size - variable->used);
            variable->used = size;
            pthread_mutex_unlock(&lock.variable);
            return variable;
        }
        pthread_mutex_unlock(&lock.variable);
        return realloc_variable(ptr, size);
    }
    pthread_mutex_unlock(&lock.variable);
    write(STDERR, "double free or corruption (out)\n", 32);
    return NULL;
}
