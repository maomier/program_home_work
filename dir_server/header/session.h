#ifndef _SESSION_H
#define _SESSION_H
#include "../header/cmd_str_list.h"
#include "send_buff_list.h"
#include "../header/ref_count.h"
#include "../header/recv_buff.h"

enum e_exit_flag{
    exit_flag_no = 0,
    exit_flag_bye = 1,
    exit_flag_err = 2,
};

typedef struct{
   cmd_str_list* cmd_list;
   int   sock_fd;
   int   exit_flag; //0: no 1: bye exit 2: error exit
   char* cur_dir;
   recv_buff* recv_buff;
   send_buff_list* send_list;
}user_session;

//no delete this
void user_session_destroy_members(void* session);
user_session* create_user_session(int sock_fd, cmd_str_list* cmd_list);

const char* user_session_get_current_dir(user_session* session);
void user_session_set_current_dir(user_session* session, const char* dir);
void user_session_set_exit_flag(user_session* session, int exit_flag);
const char* get_session_default_dir();

typedef struct{
    size_t  size;
    size_t  cap;
    cvector_vector_type(user_session*) session_list;
    pthread_mutex_t mutex;
}user_session_list;
user_session_list* create_user_session_list();
void free_user_session_list(user_session_list* list);
int user_session_list_push(user_session_list* list, user_session* session);
int user_session_list_remove(user_session_list* list, user_session* session);
int user_session_list_size(user_session_list* list);
int user_session_list_lock(user_session_list* list);
int user_session_list_unlock(user_session_list* list);
#endif