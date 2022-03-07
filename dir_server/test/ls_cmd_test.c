#include <stdio.h>
#include "../header/ls_cmd.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <langinfo.h>
#include <stdlib.h>

void test(){
    char* p = ls_dir("/work/");
    printf("plist = %s",p);
    free(p);
}
int main() {
    test();
    printf("Hello, World!\n");
    return 0;
}