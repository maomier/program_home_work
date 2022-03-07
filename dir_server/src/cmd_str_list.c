#include "../header/cmd_str_list.h"
#include "../header/ref_count.h"
#include <unistd.h>
#include <stdio.h>

cmd_str_unit create_cmd_unit(char* str, void* handle){
    cmd_str_unit unit;
    unit.cmd_str = str;
    unit.handle = handle;
    unit.is_invalid = 0;
    return unit;
}
cmd_str_unit create_invalid_unit(){
    cmd_str_unit unit;
    unit.cmd_str = NULL;
    unit.handle = NULL;
    unit.is_invalid = 1;
    return unit;
}
bool is_invalid_cmd(cmd_str_unit* unit){
    return unit->is_invalid != 0;
}

cmd_str_list* create_cmd_str_list(){
    cmd_str_list* p_list = (cmd_str_list*)calloc(sizeof(cmd_str_list),1);
    if (pthread_mutex_init(&p_list->mutex, NULL)) {
        free(p_list);
        return NULL;
    }
    if(pthread_cond_init(&p_list->cond_t, NULL)){
        pthread_mutex_destroy(&p_list->mutex);
        free(p_list);
        return NULL;
    }
    return p_list;
}
void free_cmd_str_list(cmd_str_list* p_list){
    if(p_list){
        pthread_cond_destroy(&p_list->cond_t);
        pthread_mutex_destroy(&p_list->mutex);
        for(cmd_str_unit* it = cvector_begin(p_list->str_list);
                it != cvector_end(p_list->str_list); ++it){
            if(it->cmd_str){
                free(it->cmd_str);
                it->cmd_str = NULL;
            }
            if(it->handle){
                ref_count_retain(it->handle);
                it->handle = NULL;
            }
        }
        cvector_free(p_list->str_list);
        free(p_list);
        p_list = NULL;
    }
}

int cmd_str_list_push(cmd_str_list* p_list, cmd_str_unit unit){
    int s_size = 0;
    pthread_mutex_lock(&p_list->mutex);
    cvector_push_back(p_list->str_list, unit);
    s_size = cvector_size(p_list->str_list);
    pthread_cond_signal(&p_list->cond_t);
    pthread_mutex_unlock(&p_list->mutex);
    return s_size;
}
cmd_str_unit cmd_str_list_pop_front(cmd_str_list* p_list){
    cmd_str_unit unit;
    pthread_mutex_lock(&p_list->mutex);
    while(cvector_empty(p_list->str_list)){
        pthread_cond_wait(&p_list->cond_t, &p_list->mutex);
    }
        unit = p_list->str_list[0];
        cvector_erase(p_list->str_list,0);
    pthread_mutex_unlock(&p_list->mutex);
    return unit;
}

