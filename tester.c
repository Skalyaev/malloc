#include "include/header.h"

#define BUFFER_SIZE 4096
#define RED "\033[31m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"

static char cmd[STACK_BUFF];
static char target[STACK_BUFF];
static char value[BUFFER_SIZE];

static char names[STACK_BUFF][STACK_BUFF] = { 0 };
static char* values[STACK_BUFF] = { 0 };

static void print_state(){
    ft_putstr(GRAY"+--------" RESET"VARIABLES:\n", STDOUT);
    short x;
    for (x = 0; x < STACK_BUFF; x++){
        if (!names[x][0]) break;

        if (values[x]) ft_putaddr(values[x], STDOUT);
        else write(STDOUT, "(null)", 6);
        write(STDOUT, ": ", 2);
        ft_putstr(names[x], STDOUT);
        write(STDOUT, " = ", 3);
        if (values[x]){
            write(STDOUT, "\"", 1);
            ft_putstr(values[x], STDOUT);
            write(STDOUT, "\"", 1);
        }
        else write(STDOUT, "(empty)", 7);
        write(STDOUT, "\n", 1);
    }
    if (!x) write(STDOUT, "(empty)\n", 8);
    else show_alloc_mem_ex();
    ft_putstr(GRAY"\n> "RESET, STDOUT);
}

static short ft_strcmp(const char* const str1,
                       const char* const str2){
    for (size_t x = 0; str1[x] || str2[x]; x++)
        if (str1[x] != str2[x]) return 1;
    return 0;
}

static ssize_t ft_atoi(const char* str){
    short neg = 1;
    if (str[0] == '-'){
        neg = -1;
        str++;
    }
    ssize_t nbr = 0;
    for (size_t x = 0; str[x]; x++){
        if (str[x] < '0' || str[x] > '9') break;
        nbr = nbr * 10 + str[x] - '0';
    }
    return nbr * neg;
}

static short find_target(){
    static short offset = -1;
    short x, y;
    for (x = 0; x <= STACK_BUFF; x++){

        if (x == STACK_BUFF){
            offset++;
            if (offset == STACK_BUFF) offset = 0;
            x = offset;
            for (y = 0; target[y]; y++) names[x][y] = target[y];
            break;
        }
        if (!names[x][0]){
            for (y = 0; target[y]; y++) names[x][y] = target[y];
            break;
        }
        if (!ft_strcmp(names[x], target)) break;
    }
    return x;
}

static short test_malloc(){
    short x = find_target();
    values[x] = malloc(ft_atoi(value));
    if (!values[x]){
        ft_putstr("rly nigga ?\n", STDERR);

        for (x = 0; x < STACK_BUFF && values[x]; x++)
            free(values[x]);
        return -1;
    }
    return 0;
}

static short test_realloc(){
    short x = find_target();
    const int nbr = ft_atoi(value);
    values[x] = realloc(values[x], nbr);
    if (nbr && !values[x]){
        ft_putstr("rly nigga ?\n", STDERR);

        for (x = 0; x < STACK_BUFF && values[x]; x++)
            free(values[x]);
        return -1;
    }
    return 0;
}

static void test_free(){
    const short x = find_target();
    free(values[x]);
    values[x] = NULL;
}

static void write_value(){
    const short x = find_target();
    for (short y = 0; value[y]; y++) values[x][y] = value[y];
}

int main(){
    ft_putstr(GRAY"+--------"RESET"MALLOC TESTER"GRAY"--------+"
              RESET"\nUSAGE: "YELLOW"command "GREEN"variable "
              BLUE"[value]"RESET"\nCOMMANDS:"YELLOW
              "\nmalloc\nrealloc\nfree\nwrite\n", STDOUT);
    print_state();
    char buffer[BUFFER_SIZE] = { 0 };
    int x, y, z;
    while (read(0, buffer, BUFFER_SIZE) > 0){
        ft_putstr(GRAY"+--------------------> "RED, STDOUT);
        ft_putstr(buffer, STDOUT);
        ft_putstr(RESET"\n", STDOUT);
        ft_bzero(cmd, STACK_BUFF);
        ft_bzero(target, STACK_BUFF);
        ft_bzero(value, BUFFER_SIZE);

        for (x = 0; buffer[x] && x < STACK_BUFF; x++){
            cmd[x] = buffer[x];
            if (buffer[x] == ' ' || buffer[x] == '\n'){
                cmd[x++] = '\0';
                break;
            }
        }
        for (y = 0; buffer[x + y] && y < STACK_BUFF; y++){
            target[y] = buffer[x + y];
            if (buffer[x + y] == ' ' || buffer[x + y] == '\n'){
                target[y++] = '\0';
                break;
            }
        }
        for (z = 0; buffer[x + y + z] && x + y + z < BUFFER_SIZE;
             z++){
            value[z] = buffer[x + y + z];
            if (buffer[x + y + z] == '\n'){
                value[z] = '\0';
                break;
            }
        }
        if (!ft_strcmp(cmd, "malloc")){
            if (test_malloc() < 0) break;
        }
        else if (!ft_strcmp(cmd, "realloc")){
            if (test_realloc() < 0) break;
        }
        else if (!ft_strcmp(cmd, "free")) test_free();
        else if (!ft_strcmp(cmd, "write")) write_value();
        else ft_putstr(RED"unknown command"RESET"\n", STDOUT);
        print_state();
        ft_bzero(buffer, BUFFER_SIZE);
    }
    ft_putstr(GRAY"+--"RESET"ISN'T IT OUTSTANDING ? :)"
              GRAY"--+\n"RESET, STDOUT);
    for (x = 0; x < STACK_BUFF && values[x]; x++) free(values[x]);
    return 0;
}
