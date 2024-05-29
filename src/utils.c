#include "../include/header.h"

void ft_putstr(const char* const str, const int fd){
    size_t x = 0;
    while (str[x]) x++;
    const ssize_t useless42norm
        = write(fd, str, x);
    (void)useless42norm;
}

void ft_putnbr(int nbr, const int fd){
    ssize_t useless42norm;
    if (nbr < 0){
        if (nbr == -2147483648){
            useless42norm = write(fd, "-2147483648", 11);
            return;
        }
        useless42norm = write(fd, "-", 1);
        nbr *= -1;
    }
    if (nbr > 9) ft_putnbr(nbr / 10, fd);
    useless42norm = write(fd, &"0123456789"[nbr % 10], 1);
    (void)useless42norm;
}

void ft_putaddr(const void* const addr, const int fd){
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
    const ssize_t useless42norm
        = write(fd, str, len);
    (void)useless42norm;
}

void ft_bzero(void* const ptr, const size_t size){
    for (size_t x = 0; x < size; x++) ((byte*)ptr)[x] = 0;
}
