#include "../include/header.h"

extern Memory memory;

byte free_fixed(const void* const target, Fixed* area,
                const size_t size){
    while (area){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (area->ptr[x] != target) continue;

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
            else if (x < area->next_ptr) area->next_ptr = x;
            return SUCCESS;
        }
        area = area->next;
    }
    return FAILURE;
}

void free(void* ptr){
    if (!ptr) return;
    if (free_fixed(ptr, memory.tiny, TINY) == SUCCESS) return;
    if (free_fixed(ptr, memory.small, SMALL) == SUCCESS) return;
    for (Variable* variable = memory.variable;
        variable; variable = variable->next){
        if (ptr != variable->memory_start) continue;

        ptr->used = 0;
        if (ptr->prev && !ptr->prev->used){
            ptr->memory = ptr->prev->memory;
            ptr->memory_start = ptr->prev->memory_start;
            ptr->size += ptr->prev->size;
            ptr->prev = ptr->prev->prev;
            munmap(ptr->prev, memory.variable_size);
        }
        if (ptr->next && !ptr->next->used){
            ptr->size += ptr->next->size;
            ptr->next = ptr->next->next;
            munmap(ptr->next, memory.variable_size);
        }
        if (!ptr->prev && !ptr->next){
            munmap(ptr->memory, ptr->size);
            munmap(ptr, memory.variable_size);
            memory.variable = NULL;
        }
        break;
    }
}
