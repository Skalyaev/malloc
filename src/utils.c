#include "../include/header.h"

ssize_t ft_putstr(const char* const str, const int fd){
    size_t x = 0;
    while (str[x]) x++;
    return write(fd, str, x);
}

ssize_t ft_putint(int nbr, const int fd){
    ssize_t neg = 0;
    if (nbr < 0){
        if (nbr == -2147483648)
            return write(fd, "-2147483648", 11);
        neg = write(fd, "-", 1);
        nbr *= -1;
    }
    if (nbr > 9) ft_putint(nbr / 10, fd);
    return write(fd, &"0123456789"[nbr % 10], 1) + neg;
}

ssize_t ft_putaddr(const void* const addr, const int fd){
    static const size_t len = sizeof(void*) * 2 + 2;
    size_t nbr = (size_t)addr;
    char str[len];
    str[0] = '0';
    str[1] = 'x';

    size_t x = len;
    while (nbr){
        str[--x] = "0123456789abcdef"[nbr % 16];
        nbr /= 16;
    }
    while (x > 2) str[--x] = '0';
    return write(fd, str, len);
}

void ft_strcpy(char* const dst, const char* const src){
    size_t x = 0;
    while (src[x]){
        dst[x] = src[x];
        x++;
    }
    dst[x] = 0;
}

void ft_intcpy(char* const dst, int nbr){
    short y = 0;
    if (nbr < 0){
        if (nbr == -2147483648){
            dst[0] = '-';
            dst[1] = '2';
            nbr = 147483648;
        }
        else nbr *= -1;
        y = 2;
    }
    char tmp[12] = {0};
    short x = 11;
    while (nbr){
        tmp[--x] = "0123456789"[nbr % 10];
        nbr /= 10;
    }
    while (tmp[x]) dst[y++] = tmp[x++];
}

void ft_addrcpy(char* const dst, const void* const src){
    static const size_t len = sizeof(void*) * 2 + 2;
    size_t nbr = (size_t)src;
    dst[0] = '0';
    dst[1] = 'x';

    size_t x = len;
    dst[len] = 0;
    while (nbr){
        dst[--x] = "0123456789abcdef"[nbr % 16];
        nbr /= 16;
    }
    while (x > 2) dst[--x] = '0';
}

void ft_strcat(char* const dst, const char* const src){
    size_t x = 0;
    while (dst[x]) x++;
    size_t y = 0;
    while (src[y]){
        dst[x + y] = src[y];
        y++;
    }
    dst[x + y] = 0;
}

size_t ft_strlen(const char* const str){
    size_t x = 0;
    while (str[x]) x++;
    return x;
}

void ft_bzero(void* const ptr, const size_t size){
    for (size_t x = 0; x < size; x++) ((byte*)ptr)[x] = 0;
}
