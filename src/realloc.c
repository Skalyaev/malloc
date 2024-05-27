#include "../include/header.h"

extern Memory memory;
extern pthread_mutex_t lock;

static byte realloc_fixed(void** const ptr, Fixed* area,
                          const size_t size, const byte type){
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != *ptr) continue;

            if (size <= (size_t)area->size){
                _set_env(IN_USE, size - area->used[x]);
                area->used[x] = size;
                pthread_mutex_unlock(&lock);
                return SUCCESS;
            }
            pthread_mutex_unlock(&lock);
            void* const new_ptr = malloc(size);
            if (!new_ptr) return -1;

            pthread_mutex_lock(&lock);
            for (size_t y = 0; y < area->used[x]; y++)
                ((byte*)new_ptr)[y] = ((byte*)area->ptr[x])[y];
            *ptr = new_ptr;

            area->ptr[x] = NULL;
            _set_env(IN_USE, -area->used[x]);
            area->used[x] = 0;

            if (++area->free == STACK_BUFF - 1)
                _yes_we_free_fixed(area, area->size, type);
            else if (x < area->next_ptr) area->next_ptr = x;
            pthread_mutex_unlock(&lock);
            return SUCCESS;
        }
        area = area->next;
    }
    return FAILURE;
}

static void* variable2fixed(Variable* const variable,
                            const size_t size){
    pthread_mutex_unlock(&lock);
    void* const ptr = malloc(size);
    if (!ptr) return NULL;

    pthread_mutex_lock(&lock);
    for (size_t x = 0; x < variable->used && x < size; x++)
        ((byte*)ptr)[x] = ((byte*)variable->memory)[x];

    _set_env(IN_USE, -variable->used);
    variable->used = 0;
    _yes_we_free(variable);
    pthread_mutex_unlock(&lock);
    return ptr;
}

static void* realloc_variable(Variable* const ptr, size_t size){
    const size_t used = size;
    size += 0xf;
    if (size < (size_t)memory.page_size) size = memory.page_size;
    else size = memory.page_size * (size / memory.page_size + 1);

    void* const new_memory = mmap(NULL, size, memory.opt.prot,
                                  memory.opt.flag, -1, 0);
    if (new_memory == MAP_FAILED){
        pthread_mutex_unlock(&lock);
        return NULL;
    }
    _set_env(ALLOC, size);

    for (size_t x = 0; x < ptr->used && x < used; x++)
        ((byte*)new_memory)[x] = ((byte*)ptr->memory)[x];

    munmap(ptr->memory, ptr->size);
    _set_env(FREED, ptr->size);

    ptr->memory = new_memory;
    ptr->memory_start = (void*)(((size_t)new_memory + 0xf)
                      & ~0xf);
    _set_env(IN_USE, used - ptr->used);
    ptr->used = used;
    ptr->size = size;
    pthread_mutex_unlock(&lock);
    return ptr->memory_start;
}

void* realloc(void* ptr, size_t size){
    if (!memory.page_size) _init_memory();
    if (!ptr) return malloc(size);
    if (!size){
        free(ptr);
        return NULL;
    }
    pthread_mutex_lock(&lock);
    byte code = realloc_fixed(&ptr, memory.tiny, size, T_TINY);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    code = realloc_fixed(&ptr, memory.small, size, T_SMALL);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    for (Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        if (size <= variable->size){
            if (size <= (size_t)memory.opt.small)
                return variable2fixed(variable, size);

            _set_env(IN_USE, size - variable->used);
            variable->used = size;
            pthread_mutex_unlock(&lock);
            return variable;
        }
        return realloc_variable(ptr, size);
    }
    pthread_mutex_unlock(&lock);
    static const char* const err
        = "double free or corruption (out)\n";
    write(STDERR, err, 32);
    return NULL;
}
