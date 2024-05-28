#ifndef HEADER_H
#define HEADER_H

#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "define.h"
#include "struct.h"

void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);
int mallopt(int opt, int value);

void show_alloc_mem();
void show_alloc_mem_ex();

void _init_memory();
void _yes_we_free(Variable* const ptr);
void _yes_we_free_fixed(Fixed* const area, const byte type);
void _set_env(const ushort id, size_t value);
char* _get_env(const ushort id);

void ft_putstr(const char* const str, const int fd);
void ft_putnbr(int nbr, const int fd);
void ft_putaddr(const void* const addr, const int fd);
void ft_bzero(void* const ptr, const size_t size);

#endif
