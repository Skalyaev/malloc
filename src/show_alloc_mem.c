#include "../include/header.h"

extern Memory memory;

void show_dump(const void* const ptr, const size_t size){
    printf("%s+--------------------------------------+%s\n",
            GRAY, RESET);
    for (size_t x = 0; x < size; x++){
        printf("%s|  %s", GRAY, GREEN);

        for (size_t y = 0; y < 8; y++){
            if (x + y >= size) break;
            printf("%02x ", ((byte*)ptr)[x + y]);
        }
        printf("    ");
        for (size_t y = 0; y < 8; y++){
            if (x + y >= size) break;
            if (((byte*)ptr)[x + y] < 0x20 ||
                ((byte*)ptr)[x + y] > 0x7e) printf(".");
            else printf("%c", ((byte*)ptr)[x + y]);
        }
        printf("%s  |%s\n", GRAY, RESET);
    }
    printf("%s+--------------------------------------+%s\n",
            GRAY, RESET);
}

bool set_dump(const bool set){
    static bool dump = NO;
    if (set > 0) dump = set;
    return dump;
}

Fixed* show_fixed(const Fixed* ptr, const bool dump,
                  const size_t type, size_t* const allocated,
                  size_t* const used){
    while (YES){
        *allocated += type * STACK_BUFF;
        for (size_t x = 0; x < STACK_BUFF; x++){
            if (!ptr->stack[x]) continue;
            *used += ptr->used[x];

            printf("%s| [%s%p%s]----[%s%p%s]----> %s%lu%s\n", GRAY,
                   RESET, ptr->stack[x], GRAY,
                   RESET, ptr->stack[x] + ptr->used[x],
                   GRAY, GREEN, ptr->used[x], RESET);
            if (dump == YES) show_dump(ptr->stack[x], ptr->used[x]);
        }
        if (!ptr->next) break;
        ptr = ptr->next;
    }
    return ptr;
}

void show_alloc_mem(){
    Variable* ptr;
    size_t allocated = 0;
    size_t used = 0;
    const bool dump = set_dump(-1);
    if (memory.tiny){
        printf("%s+--------TINY----[%s%p%s]%s\n",
               GRAY, RESET, memory.tiny.memory, GRAY, RESET);

        ptr = show_fixed(memory.tiny, dump, TINY,
                         &allocated, &used);
        printf("%s+----------------[%s%p%s]%s\n",
               GRAY, RESET, ptr.memory + TINY * STACK_BUFF,
               GRAY, RESET);
    }
    if (memory.small){
        printf("%s+--------SMALL---[%s%p%s]%s\n",
               GRAY, RESET, memory.small.memory, GRAY, RESET);

        ptr = show_fixed(memory.small, dump, SMALL,
                         &allocated, &used);
        printf("%s+----------------[%s%p%s]%s\n",
               GRAY, RESET, ptr.memory + SMALL * STACK_BUFF,
               GRAY, RESET);
    }
    if (memory.variable){
        printf("%s+--------LARGE---[%s%p%s]%s\n",
               GRAY, RESET, memory.variable.memory, GRAY, RESET);

        for (ptr = memory.variable; ptr; ptr = ptr->next){
            allocated += ptr.size;
            used += ptr.used;
            printf("%s| [%s%p%s]----[%s%p%s]----> %s%lu%s\n",
                   GRAY, RESET, ptr.memory, GRAY, RESET,
                   ptr.memory + ptr.used, GRAY,
                   GREEN, ptr.used, RESET);
            if (dump == YES) show_dump(ptr.memory, ptr.used);
        }
        printf("%s+----------------[%s%p%s]%s\n",
               GRAY, RESET, ptr.memory + ptr.size, GRAY, RESET);
    }
    printf("Allocated%s----[%s%lu%s bytes%s]%s\n",
           GRAY, GREEN, allocated, RESET, GRAY, RESET);
    printf("Used%s----------[%s%lu%s bytes%s]%s\n",
           GRAY, GREEN, used, RESET, GRAY, RESET);
    set_dump(NO);
}

void show_alloc_mem_ex(){
    set_dump(YES);
    show_alloc_mem();
}
