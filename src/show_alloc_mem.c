#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

static void show_dump(const void* const ptr, const size_t size){
    static const char* const base = "0123456789abcdef";
    ft_putstr(GRAY"|\n+--------------------------------------+\n"
              RESET, STDOUT);

    char hex[3] = "   ";
    for (size_t x = 0; x < size; x += 8){
        if (size > 64 && x == 32){
            ft_putstr(
                GRAY"|                 ....                 |\n"
                RESET, STDOUT
            );
            x = size - 32;
        }
        ft_putstr(GRAY"|  "GREEN, STDOUT);
        for (size_t y = x; y < x + 8; y++){
            if (y >= size){
                write(STDOUT, "   ", 3);
                continue;
            }
            hex[0] = base[((byte*)ptr)[y] / 16];
            hex[1] = base[((byte*)ptr)[y] % 16];
            write(STDOUT, hex, 3);
        }
        write(STDOUT, "  ", 2);

        for (size_t y = x; y < x + 8; y++){
            if (y >= size){
                write(STDOUT, " ", 1);
                continue;
            }
            if (((byte*)ptr)[y] < 32 || ((byte*)ptr)[y] > 126)
                write(STDOUT, ".", 1);
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
        for (size_t x = 0; x < STACK_BUFF; x++){
            if (!ptr->ptr[x]) continue;
            ft_putstr(GRAY"|"RESET"\n"GRAY"| [ "RESET, STDOUT);
            ft_putaddr(ptr->ptr[x], STDOUT);
            ft_putstr(GRAY" ]--[ "RESET, STDOUT);
            ft_putaddr(ptr->ptr[x] + ptr->used[x], STDOUT);
            ft_putstr(GRAY" ]--> "GREEN, STDOUT);
            ft_putnbr(ptr->used[x], STDOUT);
            ft_putstr(RESET" bytes\n", STDOUT);
            if (dump == YES) show_dump(ptr->ptr[x], ptr->used[x]);
        }
        if (!ptr->next) break;
        ptr = ptr->next;
    }
    return ptr;
}

static void show_variable(){
    ft_putstr("\n"GRAY"+--VARIABLE--[ "RESET, STDOUT);
    ft_putaddr(memory.variable->memory, STDOUT);
    ft_putstr(GRAY" ]"RESET"\n", STDOUT);

    Variable* ptr = memory.variable;
    Variable* prev = ptr;
    while (ptr){
        ft_putstr(GRAY"|"RESET"\n"GRAY"| [ "RESET, STDOUT);
        ft_putaddr(ptr->memory_start, STDOUT);
        ft_putstr(GRAY" ]--[ "RESET, STDOUT);
        ft_putaddr(ptr->memory_start + ptr->used, STDOUT);
        ft_putstr(GRAY" ]--> "GREEN, STDOUT);
        ft_putnbr(ptr->used, STDOUT);
        ft_putstr(RESET" bytes\n", STDOUT);
        if (set_dump(-1) == YES) show_dump(ptr->memory, ptr->used);
        prev = ptr;
        ptr = ptr->next;
    }
    ft_putstr(GRAY"|"RESET"\n"GRAY"+------------[ "
              RESET, STDOUT);
    ft_putaddr(prev->memory + prev->size, STDOUT);
    ft_putstr(GRAY" ]--VARIABLE-END"RESET"\n", STDOUT);
}

void show_alloc_mem(){
    if (!memory.page_size) _init_memory();
    pthread_mutex_lock(&lock.dump);
    const bool dump = set_dump(-1);

    Fixed* ptr;
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
    if (memory.variable) show_variable();
    pthread_mutex_unlock(&lock.variable);

    set_dump(NO);
    pthread_mutex_unlock(&lock.dump);

    ft_putstr("\nAllocated"GRAY"----[ "GREEN, STDOUT);
    ft_putstr(_get_env(ALLOC), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);

    ft_putstr("Used"GRAY"---------[ "GREEN, STDOUT);
    ft_putstr(_get_env(IN_USE), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);

    ft_putstr("Freed"GRAY"--------[ "GREEN, STDOUT);
    ft_putstr(_get_env(FREED), STDOUT);
    ft_putstr(RESET" bytes"GRAY" ]"RESET"\n", STDOUT);
}

void show_alloc_mem_ex(){
    pthread_mutex_lock(&lock.dump);
    set_dump(YES);
    pthread_mutex_unlock(&lock.dump);
    show_alloc_mem();
}
