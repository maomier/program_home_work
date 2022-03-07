#ifndef _CMD_STR_H
#define _CMD_STR_H

#include <pthread.h>
#include <rpc/types.h>
#include <stdbool.h>
#define CVECTOR_LOGARITHMIC_GROWTH
#include "vector.h"
typedef struct{
    char* cmd_str;
    void* handle;
    int   is_invalid;
}cmd_str_unit;

cmd_str_unit create_cmd_unit(char* str, void* handle);
cmd_str_unit create_invalid_unit();
bool is_invalid_cmd(cmd_str_unit*);


typedef struct _cmd_str_list{
    pthread_mutex_t mutex;
    pthread_cond_t cond_t;
    size_t cap;
    size_t size;
    cvector_vector_type(cmd_str_unit) str_list;
}cmd_str_list;

cmd_str_list* create_cmd_str_list();
void free_cmd_str_list(cmd_str_list* p_list);

int cmd_str_list_push(cmd_str_list* p_list, cmd_str_unit unit);
cmd_str_unit cmd_str_list_pop_front(cmd_str_list* p_list);

#endif