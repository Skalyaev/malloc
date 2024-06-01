#include "../include/header.h"

extern Memory memory;
extern Mutex lock;

void show_alloc_hist(){
    if (!memory.page_size) _init_memory();
    pthread_mutex_lock(&lock.history);
    pthread_mutex_lock(&lock.print);;
    size_t x = memory.history_start;
    while (YES){
        if (!memory.history[x][0]) break;
        ft_putstr(memory.history[x], STDOUT);
        x++;
        if (x == STACK_BUFF) x = 0;
        if (x == memory.history_start) break;
    }
    pthread_mutex_unlock(&lock.history);
    pthread_mutex_unlock(&lock.print);
}

void add2history(const char* const str, const void* const ptr,
                 const size_t size){
    if (!str || !ptr) return;
    if (!memory.page_size) _init_memory();

    pthread_mutex_lock(&lock.history);
    if (memory.history[STACK_BUFF - 1][0]) memory.history_start++;
    if (memory.history_start == STACK_BUFF) memory.history_start = 0;
    if (memory.history_index == STACK_BUFF) memory.history_index = 0;

    ft_strcpy(memory.history[memory.history_index], GREEN);
    ft_strcat(memory.history[memory.history_index], str);
    ft_strcat(memory.history[memory.history_index], RESET);

    size_t len = ft_strlen(memory.history[memory.history_index]);
    ft_addrcpy(memory.history[memory.history_index] + len, ptr);
    ft_strcat(memory.history[memory.history_index], " ("GREEN);

    len += sizeof(void*) * 2 + 4 + ft_strlen(GREEN);
    ft_intcpy(memory.history[memory.history_index] + len, size);
    ft_strcat(memory.history[memory.history_index],
              RESET" bytes)\n");

    memory.history_index++;
    pthread_mutex_unlock(&lock.history);
}
