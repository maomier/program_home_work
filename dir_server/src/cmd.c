#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../header/cmd.h"
#include "../header/ls_cmd.h"
#include "../header/cd_cmd.h"
#include "../header/session.h"

static int g_cmd_talbe_inited = 0;
static char g_cmd_table[256][256][256] = {0};
static actual_cmd_fun g_cmd_fun_table[5] = {
        err_cmd,
        ls_cmd,
        cd_cmd,
        pwd_cmd,
        bye_cmd,
};
static void init_cmd_table(){
    if(g_cmd_talbe_inited==0){
        //ls = 1
        memset(g_cmd_table['l']['s'], 1, 256);
        //cd = 2
        memset(g_cmd_table['c']['d'], 2, 256);
        g_cmd_table['p']['w']['d'] = 3;
        g_cmd_table['b']['y']['e'] = 4;
        g_cmd_talbe_inited = 1;
    }
}
static actual_cmd_fun check_cmd_table(char first, char second, char third){
    if(first == 0 || second == 0){
        return err_cmd;
    }
    if(g_cmd_talbe_inited==0){
        init_cmd_table();
    }
    int idx = g_cmd_table[first][second][third];
    return g_cmd_fun_table[idx];
}

int cmd_process_result_code = ret_ls;
int get_cmd_process_ret_code(){
    return cmd_process_result_code;
}
char* cmd_process(char* cmd_str_to_free, void* handle, int* p_out_dt_len){
    char* ptr = cmd_str_to_free;
    //skip space
    while(*ptr == ' '){
        ptr++;
    }

    //substract cmd
    char cmd_char[3] = {0};
    int cmd_char_index = 0;
    while(*ptr){
        cmd_char[cmd_char_index] = *ptr;
        if(cmd_char[cmd_char_index] <0)
            cmd_char[cmd_char_index] = 0;
        cmd_char_index++;
        ptr++;
        if(cmd_char_index>=sizeof(cmd_char)){
            break;
        }
    }
    //use actual fun to process dt
    ptr = check_cmd_table(cmd_char[0],cmd_char[1],cmd_char[2])(cmd_str_to_free, ptr,handle);

    //it is malloc by strdup, here, free it
    free(cmd_str_to_free);
    *p_out_dt_len = strlen(ptr);
    return ptr;
}


char* ls_cmd(char* p_entire_str, char* ptr,  void* handle){
    cmd_process_result_code = ret_ls;
    return ls_dir(user_session_get_current_dir((user_session*)handle));
}
char* cd_cmd(char* p_entire_str, char* ptr,  void* handle){
    char* dest_path = cd_parse_path(user_session_get_current_dir((user_session *) handle), ptr);
    if(cd_check_path_exist(dest_path)==0){
        cmd_process_result_code = ret_cd;
        user_session_set_current_dir((user_session*)handle, dest_path);
    }else{
        cmd_process_result_code = ret_cd_err;
        strcat(dest_path,": path not found");
    }
    return dest_path;
}
char* bye_cmd(char* p_entire_str, char* ptr,  void* handle){
    char* msg = malloc(4);
    strcpy(msg,"bye");
    user_session_set_exit_flag((user_session*)handle, exit_flag_bye);
    cmd_process_result_code = ret_bye;
    return msg;
}
char* pwd_cmd(char* p_entire_str, char* ptr,  void* handle){
    const char* p_current_dir = user_session_get_current_dir((user_session*)handle);
    char* msg = calloc(strlen(p_current_dir)+1,1);
    strcpy(msg,p_current_dir);
    cmd_process_result_code = ret_pwd;
    return msg;
}
static char* err_cmd_msg_format = "%s: command not found...,(Only ls, pwd, cd and bye are supported)\n";
char* err_cmd(char* p_entire_str, char* ptr,  void* handle){
    char* msg = calloc(strlen(p_entire_str)+80,1); // 80 for err_cmd_msg addtional length;
    if(msg){
        sprintf(msg,err_cmd_msg_format, p_entire_str);
    }
    cmd_process_result_code = ret_err;
    return msg;
}
