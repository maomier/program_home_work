#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include "../header/server.h"

static int g_signal_enable = 0;
static void catch_signal(int sign)
{
    switch (sign)
    {
        case SIGINT:
        case SIGKILL:
        case SIGTERM:
            g_signal_enable = 1;
            printf("kill signal is enabled...");
            break;
        default:
            printf("entered sign:%d", sign);
            break;
    }
}

int main(int argc, char *argv[])
{
    typedef void (*sighandler_t)(int);
    sighandler_t old=signal(SIGINT, catch_signal);
    if(old==SIG_ERR)
    {
        perror("signal error");
        return -1;
    }
    if(signal(SIGQUIT,catch_signal)==SIG_ERR)
    {
        perror("signal error");
        return -1;
    }
    int port = 5432;
    if(argc>1)
    {
        port = atoi(argv[1]);
        if(port == 0){
            port = 5432;
        }
    }
    int listen_sock = create_listen_sock(NULL, port, 100);
    if(listen_sock<0){
        printf("failed to init listen socket");
        return -2;
    }
    start_listen_sock_loop(listen_sock, 1024, 8192, &g_signal_enable);

    signal(SIGINT, old);
    return 1;
}