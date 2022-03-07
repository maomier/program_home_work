#include "../header/send_buff.h"
#include <string.h>
#include <stdlib.h>

send_buff create_send_buff(char* ptr, int data_len){
    send_buff buff;
    buff.data_len = data_len;
    buff.next_pos = 0;
    buff.ptr = ptr;
    return buff;
}

char* send_buff_get_data_ptr(send_buff* buff, int* pout_len){
    if(buff->next_pos>=buff->data_len){
        *pout_len = 0;
        return NULL;
    }else{
        *pout_len = buff->data_len - buff->next_pos;
        return buff->ptr + buff->next_pos;
    }
}

int send_buff_consume(send_buff* buff, int len){
    if(len>0) {
        buff->next_pos += len;
    }
    if( buff->next_pos >= buff->data_len ){
        if(buff->ptr){
            free(buff->ptr);
            buff->ptr = NULL;
        }
        return 1;
    }else{
        return 0;
    }
}
