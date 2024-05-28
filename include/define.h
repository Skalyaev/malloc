#ifndef DEFINE_H
#define DEFINE_H

#define TINY 64
#define SMALL (TINY * 4)
#define STACK_BUFF 128

#define T_TINY 0
#define T_SMALL 1

#define ENV_SIZE 5
#define ALLOC 0
#define IN_USE 1
#define FREED 2
#define INTERN_ALLOC 3
#define INTERN_FREED 4

#define TINY_SIZE 0
#define SMALL_MULT 1
#define PROT 2
#define BZERO 3

#define SUCCESS 0
#define FAILURE 1

#define YES 1
#define NO 0

#define STDOUT 1
#define STDERR 2

#define GRAY "\033[37m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

typedef char byte;
typedef unsigned char bool;
typedef unsigned short ushort;

#endif
