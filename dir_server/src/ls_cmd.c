#include "../header/ls_cmd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/version.h>
#include "../header/statx.h"

const char* fa_to_type(mode_t fa)
{
    const char* type = "link ";
    if(fa == 0){
        return "other";
    }
    if(!S_ISLNK(fa)){
        type = " file";
        if(!S_ISREG(fa)){
            type = "  dir";
            if(!S_ISDIR(fa)){
                type = " pipe";
                if(!S_ISFIFO(fa)){
                    type = "other";
                }
            }
        }
    }
    return type;
}
char* ls_dir(const char* dir_path){
    char path_buff[PATH_MAX+128] = {0};
    struct dirent** namelist;
    int num = scandir(dir_path, &namelist, 0, alphasort);
    int total_len = 0;
    for(int idx=0;idx<num;idx++){
        if(strcmp(namelist[idx]->d_name,".") && strcmp(namelist[idx]->d_name,"..")) {
            total_len += (26 + strlen(namelist[idx]->d_name));
        }
    }

    char* p_out = (char*)calloc(total_len,1);
    struct tm stm;
    for(int idx=0;idx<num;idx++){
        if(strcmp(namelist[idx]->d_name,".") && strcmp(namelist[idx]->d_name,"..")){
            //prepare full path
            strcpy(path_buff, dir_path);
            strcat(path_buff, namelist[idx]->d_name);
            //collect stat
            struct stat64 stat_buf;
            mode_t st_mode = 0;
            if (lstat64(path_buff, &stat_buf) == 0) {
                st_mode = stat_buf.st_mode;
            }
           //creation time
            time_t crtime = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0))
            //kernel 4.11
            struct statx stx = { 0, };
            int rc = statx(0, path_buff, AT_SYMLINK_NOFOLLOW, STATX_BTIME, &stx);
            if (rc == 0) {
                if (stx.stx_btime.tv_sec != 0) {
                    crtime = stx.stx_btime.tv_sec;
                }
            }
#endif
            if(crtime==0) //use the change time instead
                crtime = stat_buf.st_ctim.tv_sec;

            localtime_r(&crtime, &stm);
            sprintf(path_buff, "%s %s %d-%02d-%02d\n", fa_to_type(st_mode), namelist[idx]->d_name, (stm.tm_year+1900),(stm.tm_mon+1),stm.tm_mday);
            strcat(p_out, path_buff);
        }
    }
    free(namelist);

    return p_out;
}