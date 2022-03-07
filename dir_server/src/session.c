#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include "../header/session.h"
#include "../header/funs.h"
//no delete this
void user_session_destroy_members(void* input_session){
    user_session* session = (user_session*)input_session;
    if(session){
        if(session->cur_dir){
            free(session->cur_dir);
            session->cur_dir = NULL;
        }
        if(session->recv_buff){
            recv_buff_delete(session->recv_buff);
            session->recv_buff = NULL;
        }
        if(session->send_list){
            send_buff_list_delete(session->send_list);
            session->send_list = NULL;
        }
        if(session->cmd_list){
            session->cmd_list = NULL;
        }
        session = NULL;
    }
}

user_session* create_user_session(int sock_fd, cmd_str_list* cmd_list) {
    user_session *session =
            (user_session *) ref_count_alloc(sizeof(user_session), user_session_destroy_members);
    session->cmd_list = cmd_list;
    session->sock_fd = sock_fd;
    session->exit_flag = 0;
    session->cur_dir = NULL;
    session->recv_buff = create_recv_buff();
    session->send_list = create_send_buff_list();
    return session;
}

const char* user_session_get_current_dir(user_session* session){
    if(session==NULL || session->cur_dir == NULL){
        return get_session_default_dir();
    }else{
        return session->cur_dir;
    };
}
void user_session_set_current_dir(user_session* session, const char* const_dir){
    if(session){
        if(session->cur_dir){
            free(session->cur_dir);
            session->cur_dir = NULL;
        }
        if(const_dir && strcmp(const_dir,get_session_default_dir())){
            session->cur_dir = strdup(const_dir);
        }
    }
}
void user_session_set_exit_flag(user_session* session, int exit_flag){
    if(session){
        session->exit_flag = exit_flag;
    }
}

static const char* g_init_home_dir = NULL;
static char* init_cur_dir(){
    char* local_path = calloc(PATH_MAX+1,1);
    if(local_path == NULL){
        return local_path;
    }
    if(readlink("proc/self/exe",local_path, PATH_MAX) != -1){
        dirname(local_path);
    }else{
        getcwd(local_path, PATH_MAX);
    }
    strcat(local_path,"/");
    printf("init current dir:%s\n", local_path);
    return local_path;
}
const char* get_session_default_dir(){
    if(g_init_home_dir==NULL){
        g_init_home_dir = init_cur_dir();
    }
    return g_init_home_dir;

}


user_session_list* create_user_session_list(){
    user_session_list* list = (user_session_list*)calloc(sizeof(user_session_list),1);
    if (pthread_mutex_init(&list->mutex, NULL)) {
        free(list);
        return NULL;
    }
    return list;
}
void free_user_session_list(user_session_list* list){
    if(list){
        pthread_mutex_destroy(&list->mutex);
        for(int i=0;i<cvector_size(list->session_list);i++){
            close(list->session_list[i]->sock_fd);
            ref_count_release(list->session_list[i]);
        }
        cvector_free(list->session_list);
    }
}
int user_session_list_push(user_session_list* list, user_session* session){
    int size = 0;
    pthread_mutex_lock(&list->mutex);
    cvector_push_back(list->session_list, session);
    size = cvector_size(list->session_list);
    pthread_mutex_unlock(&list->mutex);
    return size;
}
int user_session_list_remove(user_session_list* list, user_session* session){
    int size = 0;
    pthread_mutex_lock(&list->mutex);
    for(int i=0;i<cvector_size(list->session_list);i++){
        if(session == list->session_list[i]){
            ref_count_release(session);
            cvector_erase(list->session_list,i);
        }
    }
    size = cvector_size(list->session_list);
    pthread_mutex_unlock(&list->mutex);
    return size;
}
int user_session_list_size(user_session_list* list){
    int size = 0;
    pthread_mutex_lock(&list->mutex);
    size = cvector_size(list->session_list);
    pthread_mutex_unlock(&list->mutex);
    return size;
}
int user_session_list_lock(user_session_list* list){
    pthread_mutex_lock(&list->mutex);
}
int user_session_list_unlock(user_session_list* list){
    pthread_mutex_unlock(&list->mutex);
}
