#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../header/cd_cmd.h"


int cd_check_path_exist(const char *path){
    return access(path, 0);
}
char* cd_parse_path(const char *current_dir, const char *ptr){
    char* buff = (char*)malloc(PATH_MAX+1);
    if(ptr == NULL){
        strcpy(buff, current_dir);
        return buff;
    }
    //skip space
    while(*ptr==' ') ptr++;

    //init the path
    if(*ptr=='/'||*ptr=='\\' || *ptr == 0){
        buff[0] = '/';
        buff[1] = 0;
    }else{
        strcpy(buff, current_dir);
    }
    int len = strlen(buff);
    //lets it endwiths /
    if(buff[len-1]!='/'){
        buff[len] = '/';
        buff[len+1] = 0;
        len ++;
    }
    int dot_count = 0;
    int only_dot_count = 1;
    int before_is_sep = 1;
    while(1){ //not end
        if(*ptr==0 || *ptr == '/' || *ptr == '\\'){
            if(!before_is_sep){
                if(only_dot_count && dot_count > 0 && dot_count < 3){
                    if(dot_count==1){
                        buff[--len]=0; // len -1 to delete last .
                    }else{// == 2, reback
                        buff[len-1] = 0;
                        buff[len-2] = 0;
                        if(len-3 > 0){
                            buff[len-3] = 0;
                            int idx = 4;
                            while(buff[len-idx]!='/'){
                                buff[len-idx] = 0;
                                idx++;
                            }
                            len -= idx-1;
                        }else{
                           len -= 2;
                        }
                    }
                }else{
                    //have other characters,just add /
                    buff[len++] = '/';
                    buff[len] = 0;
                }
                before_is_sep = 1;
                only_dot_count = 1;
                dot_count = 0;
            }
            if(*ptr == 0) break; else { ptr++;}
        }else{
            if(*ptr == '.'){
                dot_count++;
                before_is_sep = 0;
            }else{
                only_dot_count = 0;
                before_is_sep = 0;
            }
            buff[len++] = *ptr;
            buff[len] = 0;
            ptr++;
        }
    }
    return buff;
}