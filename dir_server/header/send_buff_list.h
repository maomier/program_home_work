#ifndef _RESPONSE_H
#define _RESPONSE_H

#include <pthread.h>
#include <rpc/types.h>
#include <stdbool.h>
#define CVECTOR_LOGARITHMIC_GROWTH
#include "vector.h"
#include "send_buff.h"

typedef struct{
    pthread_mutex_t mutex;
    size_t cap;
    size_t size;
    cvector_vector_type(send_buff) buff_list;
}send_buff_list;

send_buff_list* create_send_buff_list();
void send_buff_list_delete(send_buff_list*);
int send_buff_list_push(send_buff_list*, send_buff);
int send_buff_list_size(send_buff_list*);
send_buff* send_buff_list_front(send_buff_list*);
void send_buff_list_pop_front(send_buff_list*);



#endif