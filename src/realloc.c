#include "../include/header.h"

extern Memory memory;

byte realloc_fixed(void** const ptr, Fixed* area,
                   const size_t size, const size_t new_size){
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != *ptr) continue;

            if (new_size <= size){
                area->used[x] = new_size;
                return SUCCESS;
            }
            void* const new_ptr = malloc(size);
            if (!new_ptr) return -1;

            for (size_t y = 0; y < area->used[x]; y++)
                ((byte*)new_ptr)[y] = ((byte*)area->ptr[x])[y];
            *ptr = new_ptr;

            area->ptr[x] = NULL;
            if (++area->free == STACK_BUFF - 1){
                if (area->next) area->next->prev = area->prev;
                if (area->prev) area->prev->next = area->next;
                if (size == TINY){
                    if (memory.tiny == area)
                        memory.tiny = area->next;
                }
                else if (memory.small == area)
                    memory.small = area->next;
                munmap(area->memory, size * STACK_BUFF);
                munmap(area, memory.fixed_size);
            }
            return SUCCESS;
        }
        area = area->next;
    }
    return FAILURE;
}

void* variable2fixed(Variable* const variable, const size_t size){
    void* const ptr = malloc(size);
    if (!ptr) return NULL;

    for (size_t x = 0; x < variable->used && x < size; x++)
        ((byte*)ptr)[x] = ((byte*)variable->memory)[x];

    munmap(variable->memory, variable->size);
    if (variable->prev) variable->prev->next = variable->next;
    if (variable->next) variable->next->prev = variable->prev;
    if (memory.variable == variable)
        memory.variable = variable->next;
    munmap(variable, memory.variable_size);
    return ptr;
}

void* realloc(void* ptr, size_t size){
    byte code = realloc_fixed(&ptr, memory.tiny, TINY, size);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    code = realloc_fixed(&ptr, memory.small, SMALL, size);
    if (code == SUCCESS) return ptr;
    if (code < 0) return NULL;

    for (Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        if (size <= variable->size){
            if (size <= SMALL) return variable2fixed(variable, size);
            variable->used = size;
            break;
        }
        const size_t used = size;
        size += 0xf;
        if (size < memory.page_size) size = memory.page_size;
        else size = memory.page_size * (size / memory.page_size + 1);

        void* const new_memory = mmap(NULL, size,
                                      PROT_READ | PROT_WRITE,
                                      MAP_PRIVATE, -1, 0);
        if (new_memory == MAP_FAILED) return NULL;

        for (size_t x = 0; x < variable->used; x++)
            ((byte*)new_memory)[x] = ((byte*)variable->memory)[x];

        munmap(variable->memory, variable->size);
        variable->memory = new_memory;
        variable->memory_start =
            (void*)(((size_t)new_memory + 0xf) & ~0xf);
        variable->used = used;
        variable->size = size;
        return variable->memory_start;
    }
    return ptr;
}
