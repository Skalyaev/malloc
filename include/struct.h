#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_options{
    ushort tiny;
    ushort small;
    int prot;
    bool bzero;
}Options;

typedef struct s_variable{
    void* memory[BIG_STACK_BUFF];
    void* memory_start[BIG_STACK_BUFF];
    size_t size[BIG_STACK_BUFF];
    size_t used[BIG_STACK_BUFF];
    ushort next_ptr;
    ushort in_use;
    struct s_variable* prev;
    struct s_variable* next;
}Variable;

typedef struct s_fixed{
    void* memory;
    void* memory_start;
    ushort size;
    void* ptr[STACK_BUFF - 1];
    ushort used[STACK_BUFF - 1];
    ushort next_ptr;
    ushort in_use;
    struct s_fixed* prev;
    struct s_fixed* next;
}Fixed;

typedef struct s_memory{
    int page_size;
    Fixed* tiny;
    Fixed* small;
    Variable* variable;
    size_t fixed_size;
    size_t variable_size;
    Options opt;
    char history[STACK_BUFF][STACK_BUFF];
    ushort history_index;
    ushort history_start;
}Memory;

typedef struct s_mutex{
    pthread_mutex_t tiny;
    pthread_mutex_t small;
    pthread_mutex_t variable;
    pthread_mutex_t opt;
    pthread_mutex_t env;
    pthread_mutex_t init;
    pthread_mutex_t print;
    pthread_mutex_t history;
}Mutex;

#endif
