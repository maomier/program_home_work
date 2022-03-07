#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "../header/funs.h"
#include "../header/cmd.h"
#include "../header/send_buff_list.h"
#include "../header/cmd_str_list.h"
#include "../header/session.h"
#include "../header/server.h"

int create_listen_sock(const char* ip, const uint16_t port, const int listen_limit){
    //init listen sock and set reuseable
    int reuse_opts = 1;
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock == -1){
        return SERVER_ERR_COMMON;
    }
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_opts, sizeof(reuse_opts));

    //prepare address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if(ip){
        inet_pton(AF_INET, ip, &address.sin_addr);
    }else{
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    //start bind address
    if(bind(listen_sock, (struct sockaddr*)(&address),sizeof(address))==-1){
        close(listen_sock);
        return SERVER_ERR_COMMON;
    }

    //start listen
    if(listen(listen_sock, listen_limit)==-1){
        close(listen_sock);
        return SERVER_ERR_COMMON;
    }

    return listen_sock;
}
static int add_sock_to_epoll_fd(int epoll_fd, user_session* session,  int with_write){
    struct epoll_event event;
    event.data.ptr = session;
    if(with_write)
        event.events = EPOLLIN|EPOLLOUT|EPOLLET;
    else
        event.events = EPOLLIN|EPOLLET;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, session->sock_fd, &event)<0){
        return SERVER_ERR_COMMON;
    }
    int old_option = fcntl(session->sock_fd, F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(session->sock_fd, F_SETFL, new_option);
    return SERVER_SUCCESS;
}
static void read_parse_out_fun(char* cmd_str, void* in_param) {
    if(cmd_str && in_param) {
        user_session* session = (user_session*)in_param;
        //keep user_sessiion valid
        ref_count_retain((void*)session);
        cmd_str_list_push(
                session->cmd_list,
                          create_cmd_unit(strdup(cmd_str),in_param)
        );
    }
}
static void read_parse_err(void* in_param) {
    if(in_param) {
        user_session* session= (user_session*)in_param;
        user_session_set_exit_flag(session, exit_flag_err);
    }
}
static int read_socket_to_cache(int read_sock, user_session* session){
    int total_read = 0;
    while(true){
        int ret = recv(read_sock,recv_buff_get_usefule_space(session->recv_buff),
                recv_buff_get_usefule_space_len(session->recv_buff), 0);
        if(ret == -1){
            if(errno == EAGAIN){
                //no data to read, just return
                return total_read;
            }else{
                return -2;//error, need to close socket
            }
        }else if (ret == 0){
            return -1;//client closed
        }else{
            total_read += ret;
            //recv to process data
            recv_buff_parse(session->recv_buff, ret, read_parse_out_fun, read_parse_err, (void*)session);
        }
    }
    return total_read;
}
static int send_session_to_sock(int send_sock, user_session* session, int* p_all_read) {
    if(p_all_read)
        *p_all_read = 0;
    int total_send = 0;
    send_buff* send_buff = send_buff_list_front(session->send_list);
    while(send_buff) {
        int len = 0;
        char* data = send_buff_get_data_ptr(send_buff, &len);
        int ret = send(send_sock, data, len, 0);
        if(ret == -1) {
            if(errno == EAGAIN) {
                return total_send;
            }else {
                return -2;
            }
        }else if(ret == 0) {
            return -1; //client closed
        }else {
            total_send += ret;
            if(send_buff_consume(send_buff, len)==1) {
                send_buff_list_pop_front(session->send_list);
                send_buff = send_buff_list_front(session->send_list);
            }
        }
    }
    if(p_all_read)
        *p_all_read = 1;
    return total_send;
}
static int close_user_session(int epoll_fd, user_session* session, user_session_list* session_list){
    if(epoll_fd>0){
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session->sock_fd, NULL);
    }
    close(session->sock_fd);
    user_session_list_remove(session_list, session);
}

static int process_epoll(struct epoll_event* p_events, user_session_list* session_list, int ready_number, int epoll_fd, int listen_sock,cmd_str_list* recv_cmd_list ){
    int ret;
    int sockfd, accept_fd;
    struct sockaddr_in accept_addr;
    socklen_t accept_addr_len = sizeof(accept_addr);
    for(int i=0;i<ready_number;i++){
        user_session* p_out_user_session = (user_session*)p_events[i].data.ptr;
        sockfd = p_out_user_session->sock_fd;
        if(sockfd == listen_sock){
            printf("sockfd == listen_sock\n");
            accept_fd = accept(listen_sock, (struct sockaddr*)&accept_addr, &accept_addr_len);
            user_session* session = create_user_session(accept_fd, recv_cmd_list);
            user_session_list_push(session_list, session);
            if(add_sock_to_epoll_fd(epoll_fd, session, 1)<0){
                close_user_session(-1,session, session_list);
                continue;
            }
            ref_count_retain((void*)session);
            cmd_str_list_push(
                    session->cmd_list,
                    create_cmd_unit(strdup("pwd"),session)
            );
        }else if(p_events[i].events & EPOLLERR || p_events[i].events & EPOLLHUP ) {
            printf("fd epoll err: %d\n", p_out_user_session->sock_fd);
            close_user_session(epoll_fd, p_out_user_session, session_list);
        } else {
            if(p_events[i].events & EPOLLIN) {
                if(read_socket_to_cache(sockfd, p_out_user_session)<0 || p_out_user_session->exit_flag == exit_flag_err){
                    close_user_session(epoll_fd, p_out_user_session, session_list);
                    continue;
                }
            }
            if(p_events[i].events & EPOLLOUT) {
               int all_read = 0;
               if(send_buff_list_size(p_out_user_session->send_list)==0){
                   continue;
               }
               if(send_session_to_sock(sockfd, p_out_user_session, &all_read)<0) {
                   close_user_session(epoll_fd, p_out_user_session, session_list);
               }else {
                   if(p_out_user_session->exit_flag != exit_flag_no) {
                       close_user_session(epoll_fd, p_out_user_session, session_list);
                   }
               }
            }
        }
    }

    //close all the close flag socket
    cvector_vector_type(user_session*) remove_session_list = NULL;
    if(user_session_list_size(session_list)>0){
        user_session_list_lock(session_list);
        for(int i=0;i<cvector_size(session_list->session_list);i++){
            if(session_list->session_list[i]->exit_flag != exit_flag_no){
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, session_list->session_list[i]->sock_fd, NULL);
                close(session_list->session_list[i]->sock_fd);
                ref_count_release(session_list->session_list[i]);
                cvector_erase(session_list->session_list,i);
            }
        }
        user_session_list_unlock(session_list);
    }
}
typedef struct{
    uint16_t data_len;
    char cmd_ret;
}net_send_header;
char* net_send_header_to_string(net_send_header* header){
    if(header){
        char* pout = malloc(3);
        header->data_len = htons(header->data_len);
        memcpy(pout, (void*)&header->data_len, 2);
        pout[2] = header->cmd_ret;
        return pout;
    }else{
        return NULL;
    }
}
void* command_process_thread(void* in_param){
    printf("enter command_process_thread\n");
    net_send_header send_header;
    cmd_str_list* list = (cmd_str_list*)in_param;
    while(true){
        cmd_str_unit unit = cmd_str_list_pop_front(list);
        if(is_invalid_cmd(&unit)){
            printf("command_process_thread: received invalid item\n");
            break;
        }else{
            user_session* session = (user_session*)unit.handle;
            //cmd_process to free cmd_str
            int dt_len = 0;
            char* dt = cmd_process(unit.cmd_str, unit.handle, &dt_len);
            //send result to session send buff
            if(dt && dt_len > 0 && session->send_list ){
                send_header.data_len = 3 + dt_len;
                send_header.cmd_ret = get_cmd_process_ret_code();
                send_buff_list_push(session->send_list, create_send_buff(net_send_header_to_string(&send_header), 3));
                send_buff_list_push(session->send_list, create_send_buff(dt, dt_len));
            }
            if(send_session_to_sock(session->sock_fd, session,NULL)<0){
                user_session_set_exit_flag(session, exit_flag_err);
            }
            printf("ref_count:%d for socket:%d in cmd_process_thread",
                   ref_count_number((void*)session),
                   session->sock_fd
            );
            ref_count_release((void*)session);
        }
    }
    printf("exit command_process_thread\n");
    return NULL;
}

int start_listen_sock_loop(int listen_sock, int event_cache_step, int max_epoll_number, int* signal_stop_flag){
    int event_cache_number = event_cache_step;
    struct epoll_event* p_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*event_cache_number);
    if(p_events==NULL){
        return SERVER_ERR_COMMON;
    }

    user_session* listen_session = create_user_session(listen_sock, NULL);
    //add listen sock to epoll fd
    int epoll_fd = epoll_create(max_epoll_number);
    assert(epoll_fd != -1);
    if(add_sock_to_epoll_fd(epoll_fd, listen_session, 0)<0){
        close(listen_sock);
        close(epoll_fd);
        ref_count_release(listen_session);
        return SERVER_ERR_COMMON;
    }

    user_session_list* session_list = create_user_session_list();
    cmd_str_list* recv_cmd_list = create_cmd_str_list();
    pthread_t cmd_process_thread;
    int ret = pthread_create(&cmd_process_thread, NULL, command_process_thread, recv_cmd_list );
    while(true){
        if(*signal_stop_flag){
            break;
        }
        int sock_number = user_session_list_size(session_list) + 1;
        if(sock_number > event_cache_number){
            event_cache_number += event_cache_step;
            if(event_cache_number<sock_number){
                event_cache_number = sock_number;
            }
            free(p_events);
            p_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*event_cache_number);
            if(p_events==NULL){
                printf("faield to malloc events cache for number:%d", event_cache_number);
                break;
            }
        }
        int ready_number = epoll_wait(epoll_fd, p_events, sock_number, -1);
        if(ready_number < 0){
            if(errno==EINTR){
                continue;
            }
            printf("failed to epoll wait,errno=%d\n",errno);
            break;
        }
        process_epoll(p_events, session_list, ready_number, epoll_fd, listen_sock, recv_cmd_list);
    }
    //exit command process thread
    cmd_str_list_push(recv_cmd_list, create_invalid_unit());
    pthread_join(cmd_process_thread, NULL);
    free_cmd_str_list(recv_cmd_list);

    free_user_session_list(session_list);
    close(listen_sock);
    close(epoll_fd);
    ref_count_release(listen_session);
    free(p_events);
    p_events = NULL;
    return 0;
}