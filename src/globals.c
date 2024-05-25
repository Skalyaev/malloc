#include "../include/header.h"

Memory memory = { getpagesize(), NULL, NULL, NULL, 0, 0 }
memory.next_variable = &memory.variable;
memory.fixed_size =
    sizeof(Fixed) < memory.page_size ?
    memory.page_size :
    memory.page_size * (sizeof(Fixed) / memory.page_size + 1);
memory.variable_size =
    sizeof(Variable) < memory.page_size ?
    memory.page_size :
    memory.page_size * (sizeof(Variable) / memory.page_size + 1);
