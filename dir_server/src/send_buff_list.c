#include "../header/send_buff_list.h"
#include "../header/session.h"
#include "../header/ref_count.h"
#include "../header/send_buff.h"

send_buff_list* create_send_buff_list(){
    send_buff_list* p_list = (send_buff_list*)calloc(sizeof(send_buff_list),1);
    if (pthread_mutex_init(&p_list->mutex, NULL)) {
        free(p_list);
        return NULL;
    }
    p_list->buff_list = NULL;
}
void send_buff_list_delete(send_buff_list* p_list){
    if(p_list){
        pthread_mutex_destroy(&p_list->mutex);
        for(send_buff* it = cvector_begin(p_list->buff_list); it != cvector_end(p_list->buff_list); ++it){
            send_buff_consume(it, it->data_len);
        }
        cvector_free(p_list->buff_list);
        free(p_list);
        p_list = NULL;
    }
}
int send_buff_list_push(send_buff_list* p_list, send_buff buff){
    int s_size = 0;
    pthread_mutex_lock(&p_list->mutex);
    cvector_push_back(p_list->buff_list, buff);
    s_size = cvector_size(p_list->buff_list);
    pthread_mutex_unlock(&p_list->mutex);
    return s_size;
}
int send_buff_list_size(send_buff_list* p_list){
    int s_size = 0;
    pthread_mutex_lock(&p_list->mutex);
    s_size = cvector_size(p_list->buff_list);
    pthread_mutex_unlock(&p_list->mutex);
    return s_size;
}
send_buff* send_buff_list_front(send_buff_list* p_list){
    send_buff* buff = 0;
    pthread_mutex_lock(&p_list->mutex);
    if(cvector_size(p_list->buff_list)> 0)
        buff = (send_buff*)&p_list->buff_list[0];
    pthread_mutex_unlock(&p_list->mutex);
    return buff;
}
void send_buff_list_pop_front(send_buff_list* p_list){
    pthread_mutex_lock(&p_list->mutex);
    if(!cvector_empty(p_list->buff_list)){
        cvector_erase(p_list->buff_list,0);
    }
    pthread_mutex_unlock(&p_list->mutex);
}
