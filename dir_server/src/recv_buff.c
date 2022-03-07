#include "../header/recv_buff.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

recv_buff* create_recv_buff(){
    int max_len = PATH_MAX + 64;
    recv_buff* buff = (recv_buff*)calloc(sizeof(recv_buff),1);
    buff->ptr = (char*)calloc(max_len + 1 ,1);
    buff->next_pos = 0;
    buff->max_len = max_len;
    buff->sep = "\n";
    return buff;
}
void recv_buff_delete(recv_buff* buff){
    if(buff){
        if(buff->ptr){
            free(buff->ptr);
            buff->ptr = NULL;
        }
        buff->sep = NULL;
        free(buff);
        buff = NULL;
    }
}
void* recv_buff_get_usefule_space(recv_buff* buff){
    if(buff){
        return buff->ptr + buff->next_pos;
    }else{
        return NULL;
    };
}
int recv_buff_get_usefule_space_len(recv_buff* buff){
    if(buff){
        return buff->max_len - buff->next_pos;
    }else{
        return 0;
    }
}

void recv_buff_parse(recv_buff* buff, int increase, parse_out_fun out_fun, parse_err_fun  err_fun, void* out_fun_param) {
    if (buff) {
        char *end_pos = buff->ptr + buff->max_len;
        int sep_len = strlen(buff->sep);
        int next_pos = buff->next_pos + increase;
        buff->ptr[next_pos] = 0; //stop strstr here
        char *next_data_search_ptr = buff->ptr;
        char *data_search_pos = strstr(next_data_search_ptr, buff->sep);
        while (data_search_pos) {
            *data_search_pos = 0; // \r\n => \0\n
            if (data_search_pos>next_data_search_ptr && out_fun) {
                out_fun(next_data_search_ptr, out_fun_param); //process str
            }
            //jump to next, ignore \0\n
            next_data_search_ptr = data_search_pos + sep_len;
            if (next_data_search_ptr < end_pos) {
                data_search_pos = strstr(next_data_search_ptr, buff->sep);
            }else{
                data_search_pos = NULL;
            }
        }
        int left_len = buff->ptr + next_pos - next_data_search_ptr;
        if (left_len == buff->max_len) {
            //data struct error, need to close socket
            err_fun(out_fun_param);
            return;
        } else if (left_len == 0) {
            buff->next_pos = 0;
        } else {
            memcpy(buff->ptr, next_data_search_ptr, left_len);
            buff->next_pos = left_len;
        }
        buff->ptr[buff->next_pos] = 0;
    }
}