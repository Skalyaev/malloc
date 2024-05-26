#include "../include/header.h"

extern char** environ;
extern pthread_mutex_t lock;

void _set_env(const byte id, size_t value){
    static const byte count = 5;
    if (id < 0 || id >= count) return;

    static const char* const names[] = {
        "MEM_ALLOCATED", "MEM_IN_USE", "MEM_FREED",
        "INTERN_MEM_ALLOCATED", "INTERN_MEM_FREED"
    };
    static size_t values[count] = { 0, 0, 0, 0, 0 };
    static char alloc[STACK_BUFF] = { 0 };
    static char in_use[STACK_BUFF] = { 0 };
    static char freed[STACK_BUFF] = { 0 };
    static char intern_alloc[STACK_BUFF] = { 0 };
    static char intern_freed[STACK_BUFF] = { 0 };
    static byte offset[count] = { 0, 0, 0, 0, 0 };
    static byte init = 0;
    char* ptr;
    switch (id){
        case ALLOC:
            ptr = alloc;
            break;
        case IN_USE:
            ptr = in_use;
            break;
        case FREED:
            ptr = freed;
            break;
        case INTERN_ALLOC:
            ptr = intern_alloc;
            break;
        case INTERN_FREED:
            ptr = intern_freed;
            break;
    }
    size_t x;
    size_t y;
    pthread_mutex_lock(&lock);
    if (!init){
        x = 0;
        while (environ[x]) x++;
        environ[x++] = alloc;
        environ[x++] = in_use;
        environ[x++] = freed;
        environ[x++] = intern_alloc;
        environ[x++] = intern_freed;
        environ[x] = NULL;
        for (x = 0; x < count; x++){
            y = 0;
            while (names[x][y]) ptr[y] = names[x][y++];
            ptr[y++] = '=';
            offset[x] = y;
        }
        init++;
    }
    x = 0;
    char buffer[STACK_BUFF - offset[id]] = { 0 };
    char* start = &buffer[x];
    values[id] += value;
    value = values[id];
    do{
        buffer[x++] = '0' + (value % 10);
        value /= 10;
    }while (value);

    char* end = &buffer[x - 1];
    char tmp;
    while (start < end){
        tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
    for (x = 0, y = offset[id]; buffer[x]; x++, y++)
        ptr[y] = buffer[x];
    pthread_mutex_unlock(&lock);
}
_set_env(ALLOC, 0);
_set_env(IN_USE, 0);
_set_env(FREED, 0);
_set_env(INTERN_ALLOC, 0);
_set_env(INTERN_FREED, 0);
