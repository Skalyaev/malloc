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
    write(fd, "0x", 2);
    short byte;
    short index;
    for (size_t x = 0; x < sizeof(void*) * 2; x++){
        byte = ((char*)addr)[x / 2];
        index = (x % 2) ? byte & 0x0F : byte >> 4;
        write(fd, &"0123456789abcdef"[index], 1);
    }
}
