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
void _yes_we_free_fixed(Fixed* const ptr,
                        pthread_mutex_t* const mptr);
void _set_env(const ushort id, size_t value);
char* _get_env(const ushort id);

ssize_t ft_putstr(const char* const str, const int fd);
ssize_t ft_putnstr(const int size, const char* const str,
                   const int fd);
ssize_t ft_putint(int nbr, const int fd);
ssize_t ft_putaddr(const void* const addr, const int fd);
void ft_strcpy(char* const dst, const char* const src);
void ft_strcat(char* const dst, const char* const src);
void ft_intcpy(char* const dst, int nbr);
void ft_addrcpy(char* const dst, const void* const src);
size_t ft_strlen(const char* const str);
void ft_bzero(void* const ptr, const size_t size);

#endif
