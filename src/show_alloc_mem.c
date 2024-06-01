#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

static void show_dump(const void* const ptr,
                      const size_t used, const size_t size){
    static const char* const base = "0123456789abcdef";
    ft_putstr(GRAY"|\n+--------------------------------------+\n"
              RESET, STDOUT);

    char hex[4] = "   ";
    hex[3] = 0;
    static char* green = GREEN;
    static char* reset = RESET;
    char* color = green;

    for (size_t x = 0; x < size; x += 8){
        if ((x >= used && x < size - 8) || (used > 64 && x == 32)){
            ft_putstr(
                GRAY"|                 ....                 |\n"
                RESET, STDOUT
            );
            if (x >= used && x < size - 8) x = size - 8;
            else x = used - 32;
        }
        ft_putstr(GRAY"|  ", STDOUT);
        for (size_t y = x; y < x + 8; y++){
            if (y >= used && color != reset) color = reset;

            ft_putstr(color, STDOUT);
            hex[0] = base[((byte*)ptr)[y] / 16];
            hex[1] = base[((byte*)ptr)[y] % 16];
            ft_putstr(hex, STDOUT);
        }
        ft_putstr("  ", STDOUT);
        if (x < used) color = green;

        for (size_t y = x; y < x + 8; y++){
            if (y >= used && color != reset) color = reset;

            ft_putstr(color, STDOUT);
            if (((byte*)ptr)[y] < 32 || ((byte*)ptr)[y] > 126)
                ft_putstr(".", STDOUT);
            else write(STDOUT, &((byte*)ptr)[y], 1);
        }
        ft_putstr(GRAY"  |-[ "RESET, STDOUT);
        ft_putaddr(ptr + x, STDOUT);
        ft_putstr(GRAY" ]"RESET"\n", STDOUT);
    }
    ft_putstr(GRAY"+--------------------------------------+\n"
              RESET, STDOUT);
}

static bool set_dump(const byte set){
    static bool dump = NO;
    if (set >= 0) dump = set;
    return dump;
}

static Fixed* show_fixed(Fixed* ptr, const bool dump){
    while (YES){
        for (size_t x = 0; x < STACK_BUFF - 1; x++){
            if (!ptr->ptr[x]) continue;
            ft_putstr(GRAY"|"RESET"\n"GRAY"| [ "RESET, STDOUT);
            ft_putaddr(ptr->ptr[x], STDOUT);
            ft_putstr(GRAY" ]--[ "RESET, STDOUT);
            ft_putaddr(ptr->ptr[x] + ptr->used[x], STDOUT);
            ft_putstr(GRAY" ]--> "GREEN, STDOUT);
            ft_putint(ptr->used[x], STDOUT);
            ft_putstr(RESET" bytes\n", STDOUT);
            if (dump == YES) show_dump(ptr->ptr[x],
                                       ptr->used[x], ptr->size);
        }
        if (!ptr->next) break;
        ptr = ptr->next;
    }
    return ptr;
}

static void show_variable(const bool dump){
    Variable* ptr = memory.variable;
    ft_putstr("\n"GRAY"+--VARIABLE--[ "RESET, STDOUT);
    ft_putaddr(ptr->memory[0], STDOUT);
    ft_putstr(GRAY" ]"RESET"\n", STDOUT);

    ushort last = 0;
    while (YES){
        for (ushort x = 0; x < BIG_STACK_BUFF; x++){
            if (!ptr->size[x]) continue;
            last = x;
            ft_putstr(GRAY"|"RESET"\n"GRAY"| [ "RESET, STDOUT);
            ft_putaddr(ptr->memory_start[x], STDOUT);
            ft_putstr(GRAY" ]--[ "RESET, STDOUT);
            ft_putaddr(ptr->memory_start[x] + ptr->used[x], STDOUT);
            ft_putstr(GRAY" ]--> "GREEN, STDOUT);
            ft_putint(ptr->used[x], STDOUT);
            ft_putstr(RESET" bytes\n", STDOUT);
            if (dump == YES) show_dump(ptr->memory_start[x],
                                       ptr->used[x], ptr->size[x]);
        }
        if (!ptr->next){
            ft_putstr(GRAY"|"RESET"\n"GRAY"+------------[ "
                      RESET, STDOUT);
            ft_putaddr(ptr->memory[last] + ptr->size[last], STDOUT);
            ft_putstr(GRAY" ]--VARIABLE-END"RESET"\n", STDOUT);
            break;
        }
        ptr = ptr->next;
    }
}

void show_alloc_mem(){
    if (!memory.page_size) _init_memory();
    const bool dump = set_dump(-1);

    Fixed* ptr;
    pthread_mutex_lock(&lock.print);
    pthread_mutex_lock(&lock.tiny);
    if (memory.tiny){
        ft_putstr("\n"GRAY"+----TINY----[ "RESET, STDOUT);
        ft_putaddr(memory.tiny->memory, STDOUT);
        ft_putstr(GRAY" ]"RESET"\n", STDOUT);
        ptr = show_fixed(memory.tiny, dump);
        ft_putstr(GRAY"|"RESET"\n"GRAY"+------------[ "
                  RESET, STDOUT);
        ft_putaddr(ptr->memory + ptr->size * STACK_BUFF, STDOUT);
        ft_putstr(GRAY" ]--TINY-END"RESET"\n", STDOUT);
    }
    pthread_mutex_unlock(&lock.tiny);

    pthread_mutex_lock(&lock.small);
    if (memory.small){
        ft_putstr("\n"GRAY"+----SMALL---[ "RESET, STDOUT);
        ft_putaddr(memory.small->memory, STDOUT);
        ft_putstr(GRAY" ]"RESET"\n", STDOUT);
        ptr = show_fixed(memory.small, dump);
        ft_putstr(GRAY"|"RESET"\n"GRAY"+------------[ "
                  RESET, STDOUT);
        ft_putaddr(ptr->memory + ptr->size * STACK_BUFF, STDOUT);
        ft_putstr(GRAY" ]--SMALL-END"RESET"\n", STDOUT);
    }
    pthread_mutex_unlock(&lock.small);

    pthread_mutex_lock(&lock.variable);
    if (memory.variable) show_variable(dump);
    pthread_mutex_unlock(&lock.variable);

    ft_putstr("\nAllocated"GRAY"----[ "GREEN, STDOUT);
    ft_putstr(_get_env(ALLOC), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);

    ft_putstr("Used"GRAY"---------[ "GREEN, STDOUT);
    ft_putstr(_get_env(IN_USE), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);

    ft_putstr("Freed"GRAY"--------[ "GREEN, STDOUT);
    ft_putstr(_get_env(FREED), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);
    pthread_mutex_unlock(&lock.print);
    set_dump(NO);
}

void show_alloc_mem_ex(){
    set_dump(YES);
    show_alloc_mem();
}
