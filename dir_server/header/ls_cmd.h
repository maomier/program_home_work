#ifndef _LS_CMD_H
#define _LS_CMD_H
#define _GNU_SOURCE
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

typedef struct{
    mode_t type;
    char* name;
    time_t crtime;
}file_info;


const char* fa_to_type(mode_t fa);
char* ls_dir(const char* dir_path);


#endif