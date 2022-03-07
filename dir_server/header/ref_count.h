#ifndef _REF_COUNT_H
#define _REF_COUNT_H

#include <pthread.h>
#include <rpc/types.h>
typedef void (*ref_obj_free_func)(void*);

typedef struct {
    void *data;
    ref_obj_free_func free_func;
    pthread_mutex_t mutex;
    int ref;
} ref_obj_t;

void* ref_count_alloc(size_t size, ref_obj_free_func free_func);

void ref_count_retain(void *ptr);

/*ref_count_alloc, ref_count_release pair
 * ref_count_retain, ref_count_release pair
 * */
void ref_count_release(void *ptr);

int ref_count_number(void *ptr);

#endif