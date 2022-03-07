#ifndef _CD_CMD_H
#define _CD_CMD_H
#define _GNU_SOURCE
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

char* cd_parse_path(const char *current_dir, const char *ptr);
int cd_check_path_exist(const char *path);

#endif