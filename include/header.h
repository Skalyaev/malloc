#ifndef HEADER_H
#define HEADER_H

#include <sys/mman.h>
#include <pthread.h>

#define SUCCESS 0
#define FAILURE 1

#define YES 1
#define NO 0

#define STDOUT 1
#define STDERR 2

#define GRAY "\033[1;30m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

#define TINY 64
#define SMALL (TINY * 4)
#define STACK_BUFF 128

#define T_TINY 0
#define T_SMALL 1

#define ALLOC 0
#define IN_USE 1
#define FREED 2
#define INTERN_ALLOC 3
#define INTERN_FREED 4

#define TINY_SIZE 0
#define SMALL_MULT 1
#define BZERO 2
#define PROT 3

typedef char byte;
typedef struct s_memory Memory;
typedef struct s_fixed Fixed;
typedef struct s_variable Variable;
typedef struct s_options Options;

struct{
    const int page_size;
    Fixed* tiny;
    Fixed* small;
    Variable* variable;
    size_t fixed_size;
    size_t variable_size;
    Options opt;
}s_memory;

struct{
    void* memory;
    void* memory_start;
    void* ptr[STACK_BUFF];
    size_t used[STACK_BUFF];
    size_t next_ptr;
    size_t free;
    size_t size;
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

struct{
    int tiny;
    int small;
    int flag;
    int prot;
}s_options;

void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
int mallopt(int opt, int value);

void _yes_we_free(Variable* const ptr);
void _yes_we_free_fixed(Fixed* const area, size_t size,
                        const byte type);
void _set_env(const byte id, size_t value);

void show_alloc_mem();
void show_alloc_mem_ex();

void ft_putstr(const char* const str, const int fd);
void ft_putnbr(int nbr, const int fd);
void ft_putaddr(const void* const addr, const int fd);

#endif
