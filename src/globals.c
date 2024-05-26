#include "../include/header.h"

Memory memory = { getpagesize(), NULL, NULL, NULL, 0, 0, {0} }
memory.next_variable = &memory.variable;

memory.fixed_size = sizeof(Fixed) < memory.page_size ?
    memory.page_size :
    memory.page_size * (sizeof(Fixed) / memory.page_size + 1);

memory.variable_size = sizeof(Variable) < memory.page_size ?
    memory.page_size :
    memory.page_size * (sizeof(Variable) / memory.page_size + 1);

memory.opt.tiny = TINY;
memory.opt.small = SMALL;
memory.opt.flag = MAP_PRIVATE;
memory.opt.prot = PROT_READ | PROT_WRITE;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
