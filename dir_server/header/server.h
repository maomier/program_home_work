#ifndef _SERVER_H
#define _SERVER_H
#include "session.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "../header/cmd_str_list.h"

enum SERVER_ERR{
    SERVER_SUCCESS = 0,
    SERVER_ERR_COMMON = -1,
};

int create_listen_sock(const char* ip, const uint16_t port, const int listen_limit);
int start_listen_sock_loop(int listen_sock, int max_event_numbers, int max_epoll_number,int* p_stop_signal_flag);

#endif