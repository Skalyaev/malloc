#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#define TINY 64
#define SMALL (TINY * 4)
#define STACK_BUFF 128

#define SUCCESS 0
#define FAILURE 1

#define YES 1
#define NO 0

#define GRAY "\033[1;30m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

typedef char byte;
typedef struct s_memory Memory;
typedef struct s_fixed Fixed;
typedef struct s_variable Variable;

struct{
    const int page_size;
    Fixed* tiny;
    Fixed* small;
    Variable* variable;
    size_t fixed_size;
    size_t variable_size;
}s_memory;

struct{
    void* memory;
    void* memory_start;
    void* ptr[STACK_BUFF];
    size_t used[STACK_BUFF];
    size_t next_ptr;
    size_t free;
    Fixed* prev;
    Fixed* next;
}s_fixed;

struct{
    void* memory;
    void* memory_start;
    size_t size;
    size_t used;
    Variable* prev;
    Variable* next;
}s_variable;

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
void show_alloc_mem();
void show_alloc_mem_ex();

#endif
