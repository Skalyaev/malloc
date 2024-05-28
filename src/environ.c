#include "../include/header.h"

extern char** environ;
extern Mutex lock;

void _set_env(const ushort id, size_t value){
    if (id >= ENV_SIZE) return;

    static const char* const names[] = {
        "MEM_ALLOCATED", "MEM_IN_USE", "MEM_FREED",
        "INTERN_MEM_ALLOCATED", "INTERN_MEM_FREED"
    };
    static size_t values[ENV_SIZE] = { 0 };
    static char alloc[STACK_BUFF] = { 0 };
    static char in_use[STACK_BUFF] = { 0 };
    static char freed[STACK_BUFF] = { 0 };
    static char intern_alloc[STACK_BUFF] = { 0 };
    static char intern_freed[STACK_BUFF] = { 0 };
    static byte offset[ENV_SIZE] = { 0 };
    static byte init = 0;
    char* ptr;
    size_t x;
    size_t y;
    if (!init){
        x = 0;
        pthread_mutex_lock(&lock.env);
        while (environ[x]) x++;
        environ[x++] = alloc;
        environ[x++] = in_use;
        environ[x++] = freed;
        environ[x++] = intern_alloc;
        environ[x++] = intern_freed;
        environ[x] = NULL;
        for (x = 0; x < ENV_SIZE; x++){
            y = -1;
            switch (x){
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
            while (names[x][++y]) ptr[y] = names[x][y];
            ptr[y++] = '=';
            offset[x] = y;
        }
        pthread_mutex_unlock(&lock.env);
        init++;
    }
    values[id] += value;
    value = values[id];
    const size_t size = STACK_BUFF - offset[id];
    char buffer[size];
    char tmp[size];
    x = 0;
    do{
        tmp[x++] = '0' + (value % 10);
        value /= 10;
    }while (value);

    for (y = 0; x; y++, x--) buffer[y] = tmp[x - 1];
    buffer[y] = '\0';
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
    pthread_mutex_lock(&lock.env);
    for (x = 0, y = offset[id]; buffer[x]; x++, y++)
        ptr[y] = buffer[x];
    ptr[y] = '\0';
    pthread_mutex_unlock(&lock.env);
}

char* _get_env(const ushort id){
    if (id >= ENV_SIZE) return NULL;

    static const char* const names[] = {
        "MEM_ALLOCATED", "MEM_IN_USE", "MEM_FREED",
        "INTERN_MEM_ALLOCATED", "INTERN_MEM_FREED"
    };
    size_t x, y;
    pthread_mutex_lock(&lock.env);
    for (x = 0; environ[x]; x++){
        y = 0;
        while (names[id][y] && environ[x][y]
               && environ[x][y] == names[id][y]) y++;
        if (!names[id][y] && environ[x][y] == '='){
            pthread_mutex_unlock(&lock.env);
            return environ[x] + y + 1;
        }
    }
    pthread_mutex_unlock(&lock.env);
    return NULL;
}
