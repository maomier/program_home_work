#include "../header/ref_count.h"
#include <stdbool.h>

static ref_obj_t* ptr_to_obj_t(void* ptr){
    return (ref_obj_t*)((char*)ptr - sizeof(ref_obj_t));
}

void* ref_count_alloc(size_t size,ref_obj_free_func free_func)
{
    ref_obj_t *obj;
    char *ptr;

    obj = (ref_obj_t *)calloc(sizeof(ref_obj_t) + size, 1);
    if (obj == NULL) {
        return NULL;
    } else {
        obj->ref = 1;

        if (pthread_mutex_init(&obj->mutex, NULL)) {
            free(obj);
            return NULL;
        }
        ptr = (char *)obj + sizeof(ref_obj_t);
        obj->data = ptr;
        obj->free_func = free_func;
        return (void *)ptr;
    }
}

void ref_count_retain(void *ptr)
{
    ref_obj_t *obj = ptr_to_obj_t(ptr);
    pthread_mutex_lock(&obj->mutex);
    obj->ref++;
    pthread_mutex_unlock(&obj->mutex);
}

void ref_count_release(void *ptr)
{
    bool real_free= false;
    ref_obj_t *obj = ptr_to_obj_t(ptr);
    pthread_mutex_lock(&obj->mutex);
    if (--obj->ref == 0) {
        real_free = true;
    }
    pthread_mutex_unlock(&obj->mutex);
    if (real_free) {
        if(obj->free_func){
            obj->free_func(obj->data);
            obj->free_func = NULL;
        }
        pthread_mutex_destroy(&obj->mutex);
        free(obj);
    }
}

int ref_count_number(void *ptr)
{
    int value;
    ref_obj_t *obj = ptr_to_obj_t(ptr);

    pthread_mutex_lock(&obj->mutex);
    value = obj->ref;
    pthread_mutex_unlock(&obj->mutex);
    return value;
}