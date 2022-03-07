#ifndef _RECV_BUFF_H
#define  _RECV_BUFF_H
#include <sys/types.h>

typedef struct{
    char* ptr;
    int next_pos;
    int max_len;
    char* sep;
}recv_buff;

recv_buff* create_recv_buff();
void recv_buff_delete(recv_buff* buff);

void* recv_buff_get_usefule_space(recv_buff* buff);
int recv_buff_get_usefule_space_len(recv_buff* buff);

typedef void (*parse_out_fun)(char*, void*);
typedef void (*parse_err_fun)(void*);
void recv_buff_parse(recv_buff* buff, int increase, parse_out_fun out_fun, parse_err_fun  err_fun, void* out_fun_param);
#endif