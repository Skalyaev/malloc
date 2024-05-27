#include "../include/header.h"

void ft_putstr(const char* const str, const int fd){
    size_t x = 0;
    while (str[x]) x++;
    write(fd, str, x);
}

void ft_putnbr(int nbr, const int fd){
    if (nbr < 0){
        write(fd, "-", 1);
        if (nbr == -2147483648){
            write(fd, "2147483648", 10);
            return;
        }
        nbr *= -1;
    }
    if (nbr > 9) ft_putnbr(nbr / 10, fd);
    write(fd, &"0123456789"[nbr % 10], 1);
}

void ft_putaddr(const void* const addr, const int fd){
    static const size_t len = sizeof(void*) * 2;
    static const char* const base = "0123456789abcdef";

    size_t nbr = (size_t)addr;
    char str[len + 1];
    str[0] = '0';
    str[1] = 'x';
    str[len] = '\0';
    size_t x = len;
    while (nbr){
        str[x--] = base[nbr % 16];
        nbr /= 16;
    }
    while (x >= 2) str[x--] = '0';
    write(fd, str, len);
}
