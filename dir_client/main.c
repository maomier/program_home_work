#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <assert.h>
#include <errno.h>

enum e_cmd_ret{
    ret_ls = 1,
    ret_cd = 2,
    ret_pwd = 3,
    ret_bye = 4,
    ret_err = 15,
    ret_ls_err = 11,
    ret_cd_err = 12,
    ret_pwd_err = 13,
    ret_bye_err = 14
};

static char e_print_buff[2048];
#define eprintfln(fmt, ...) sprintf(e_print_buff, fmt, ##__VA_ARGS__); printf("\033[;31m%s\n\033[0m", e_print_buff);
#define eprintf(fmt, ...) sprintf(e_print_buff, fmt, ##__VA_ARGS__); printf("\033[;31m%s\033[0m", e_print_buff);
#define printfln(fmt, ...) sprintf(e_print_buff, fmt, ##__VA_ARGS__); printf("%s\n", e_print_buff);

typedef struct{
    char* cur_dir;
    char* input_buff[8192];
}state_machine;

typedef  struct{
    uint16_t data_len;
    char cmd_ret;
    char data[0];
}net_data_struct;

char* parse_buff(char* dt, int dt_len, char* p_out_ret, int* p_out_consume_len){
    *p_out_consume_len = 0;
    if(dt_len<3){
        return NULL;
    }else{
        uint16_t total_len = *(uint16_t*)dt;
        total_len = ntohs(total_len);
        if(dt_len < total_len - 3){
            //not complete
            return NULL;
        }else{
            *p_out_consume_len = total_len;
            *p_out_ret = dt[2];
            return dt+3;
        }
    }
}
void prompt_out(state_machine* sm){
    printf("\033[;32m%s$ \033[0m", sm->cur_dir);
}
int state_machine_on_response(state_machine* sm, char cmd_ret, char* data){
    if(cmd_ret> 10){
        eprintfln(data);
        free(data);
    }else{
        switch(cmd_ret){
            case ret_ls:
                printf(data);
                free(data);
                break;
            case ret_cd:
                if(sm->cur_dir){
                    free(sm->cur_dir);
                }
                sm->cur_dir = data;
                break;
            case ret_pwd:
                if(sm->cur_dir){
                    free(sm->cur_dir);
                    printfln(data);
                }
                sm->cur_dir = data;
                break;
            case ret_bye:
                printfln(data);
                free(data);
                return -1;
                break;
        }
    }
    prompt_out(sm);
    return 0;
}
int on_cmd(int send_sock, char* cmd, state_machine* sm){
    int cmd_len = strlen(cmd);
    if(cmd_len < 2){
        eprintf("%s command not found ...",cmd);
        prompt_out(sm);
        return 0;
    }
    if(strcmp(cmd,"pwd")==0 && sm->cur_dir){
        printfln(sm->cur_dir);
        prompt_out(sm);
        return 0;
    }else{
        return send(send_sock, cmd, cmd_len, 0);
    }
}
int main(int argc, char* argv[]) {
    if(argc < 3){
        eprintfln("app usage: ./client ip port");
        return -1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd==-1){
        return -2;
    }
    //define the state machine
    state_machine sm;
    sm.cur_dir = NULL;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if(connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr))==-1){
        close(sock_fd);
        return -3;
    }
    char cmd_input[1024] = {0};
    char net_recv[8192] = {0};
    int next_recv_pos = 0;
    char cmd_ret;
    int consume_len;
    int recv_len = recv(sock_fd, net_recv+next_recv_pos, 8192-next_recv_pos, 0);
    while(recv_len > 0){
        next_recv_pos += recv_len;
        char* data = parse_buff(net_recv, next_recv_pos, &cmd_ret, &consume_len);
        if(data){
            char* dt_use = (char*)calloc(consume_len-3+1,1);
            memcpy(dt_use, data, consume_len-3);

           if(next_recv_pos > consume_len)
               memcpy(net_recv, net_recv+ consume_len, next_recv_pos-consume_len);
           next_recv_pos -= consume_len;
           if(state_machine_on_response(&sm, cmd_ret, dt_use)<0){
               break;
           }
           int on_cmd_ret = 0;
           while(on_cmd_ret==0){
               fgets(cmd_input, sizeof(cmd_input), stdin);
               on_cmd_ret = on_cmd(sock_fd, cmd_input,&sm);
           }
           if(on_cmd_ret<0){
               break;
           }
        }
        recv_len = recv(sock_fd, net_recv+next_recv_pos, 8192-next_recv_pos,0);
    }
    close(sock_fd);
    return 0;
}


