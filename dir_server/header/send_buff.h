#ifndef _SEND_BUFF_H
#define _SEND_BUFF_H
#include <sys/types.h>

typedef struct{
    char* ptr;
    int next_pos;
    int data_len;
}send_buff;

send_buff create_send_buff(char* ptr, int data_len);
char* send_buff_get_data_ptr(send_buff* buff, int* pout_len);
int send_buff_consume(send_buff* buff, int len);
#endif