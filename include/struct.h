#ifndef STRUCT_H
#define STRUCT_H

typedef struct s_options{
    int tiny;
    int small;
    int prot;
    int bzero;
}Options;

typedef struct s_variable{
    void* memory;
    void* memory_start;
    size_t size;
    size_t used;
    struct s_variable* prev;
    struct s_variable* next;
}Variable;

typedef struct s_fixed{
    void* memory;
    void* memory_start;
    void* ptr[STACK_BUFF];
    size_t used[STACK_BUFF];
    size_t next_ptr;
    size_t free;
    size_t size;
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
}Memory;

typedef struct s_mutex{
    pthread_mutex_t tiny;
    pthread_mutex_t small;
    pthread_mutex_t variable;
    pthread_mutex_t opt;
    pthread_mutex_t env;
    pthread_mutex_t dump;
    pthread_mutex_t init;
    pthread_mutex_t show;
}Mutex;

#endif
