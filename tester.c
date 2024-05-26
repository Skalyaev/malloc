#include "include/header.h"
#include <string.h>

#define BUFFER_SIZE 4096
short offset = -1;

char cmd[STACK_BUFFER] = { 0 };
char target[STACK_BUFFER] = { 0 };
char value[BUFFER_SIZE] = { 0 };

char names[STACK_BUFFER][STACK_BUFFER] = { 0 };
char* values[STACK_BUFFER] = { 0 };

void test_malloc(){
    short x, y;
    for (x = 0; x <= STACK_BUFFER; x++){
        if (x == STACK_BUFFER){
            offset++;
            if (offset < 0) offset = 0;
            x = offset;
            for (y = 0; target[y]; y++)
                names[x][y] = target[y];
            break;
        }
        if (!names[x][0]){
            for (y = 0; target[y]; y++)
                names[x][y] = target[y];
            break;
        }
        if (!strcmp(names[x], target)) break;
    }
    values[x] = malloc(ft_atoi(value));
    if (!values[x]){
        ft_putstr("rly nigga ?", STDERR);
        for (x = 0; x < STACK_BUFFER && values[x]; x++)
            free(values[x]);
        exit(1);
    }
}

void print_state(){
    ft_putstr(GRAY"+-------- " RESET"VARIABLES:\n", STDOUT);
    short x;
    for (x = 0; x < STACK_BUFFER; x++){
        if (!names[x]) break;

        ft_putaddr(values[x], STDOUT);
        write(STDOUT, ": ", 2);
        ft_putstr(names[x], STDOUT);
        write(STDOUT, " = ", 3);
        ft_putstr(values[x], STDOUT);
    }
    if (!x) write(STDOUT, "NOTHING\n", 8);
    else{
        ft_putstr(GRAY"+-------- " RESET"MEMORY:\n", STDOUT);
        show_alloc_mem_ex();
    }
    write(STDOUT, "> ", 2);
}

int main(){
    ft_putstr(GRAY"+-------- "RESET"MALLOC TESTER"
              GRAY" --------+\n"GRAY"+-------- "RESET"USAGE:\n"
              "command variable_name [size]\n"
              GRAY"+-------- "RESET"COMMANDS:\n"
              "malloc\nrealloc\nfree\nwrite\n", STDOUT);
    print_state();
    char buffer[BUFFER_SIZE] = { 0 };
    int x, y, z;
    while (read(0, buffer, BUFFER_SIZE) > 0){

        for (x = 0; buffer[x] && x < STACK_BUFFER; x++){
            cmd[x] = buffer[x];
            if (buffer[x] == ' '){
                cmd[x++] = 0;
                break;
            }
        }
        for (y = 0; buffer[x + y] && y < STACK_BUFFER; y++){
            target[y] = buffer[x + y];
            if (buffer[x + y] == '\n'){
                target[y] = 0;
                break;
            }
        }
        for (z = 0; buffer[x + y + z] && z < BUFFER_SIZE; z++){
            value[z] = buffer[x + y + z];
            if (buffer[x + y + z] == '\n'){
                value[z] = 0;
                break;
            }
        }
        if (!strcmp(cmd, "malloc")) test_malloc();
        else if (!strcmp(cmd, "free")) test_free();
        else if (!strcmp(cmd, "realloc")) test_realloc();
        else if (!strcmp(cmd, "write")) write_value();
        else ft_putstr("UNKNOWN COMMAND\n", STDOUT);
        print_state();
    }
    ft_putstr(GRAY"+-- "RESET"ISN'T IT OUTSTANDING ? :)"
              GRAY" --+\n"RESET, STDOUT);
    return 0;
}
