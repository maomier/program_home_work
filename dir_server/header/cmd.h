#ifndef _CMD_H
#define _CMD_H
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

char* cmd_process(char* cmd_str_to_free, void* handle, int* p_out_dt_len);
int get_cmd_process_ret_code();

typedef char* (*actual_cmd_fun)(char*, char*, void*);
char* ls_cmd(char*,char*, void*);
char* cd_cmd(char*,char*, void*);
char* bye_cmd(char*, char*, void*);
char* pwd_cmd(char*, char*, void*);
char* err_cmd(char*, char*, void*);

#endif