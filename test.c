//#include "include/header.h"
#include <stdlib.h>
#include <stdio.h>

int main(int ac, char **av)
{
    (void)ac;
    (void)av;
    printf("%p\n", realloc(NULL, 0));
    return 0;
}
