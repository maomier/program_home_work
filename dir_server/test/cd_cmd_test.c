#include <stdio.h>
#include "../header/cd_cmd.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <langinfo.h>
#include <stdlib.h>

void test(){
    //char* parse_path(const char* current_dir, const char* ptr);
    const char* cur_dir = "/work/sub/sub1/";
    char* p = parse_path(cur_dir,NULL);
    assert(strcmp(p, cur_dir)==0);
    free(p);
    printf("test1\r\n");
    p = parse_path(cur_dir,"/");
    assert(strcmp(p, "/")==0);
    free(p);
    printf("test2\r\n");
    p = parse_path(cur_dir,"./");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, cur_dir)==0);
    free(p);
    printf("test3\r\n");
    p = parse_path(cur_dir,".");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, cur_dir)==0);
    free(p);
    printf("test4\r\n");
    p = parse_path(cur_dir,"././");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, cur_dir)==0);
    free(p);
    printf("test5\r\n");
    p = parse_path(cur_dir,"../");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, "/work/sub/")==0);
    free(p);
    printf("test6\r\n");
    p = parse_path(cur_dir,"../../");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, "/work/")==0);
    free(p);
    printf("test7\r\n");
    p = parse_path(cur_dir,"../../../");
    printf("p=%s, cur_dir=%s\r\n",p, cur_dir);
    assert(strcmp(p, "/")==0);
    free(p);
    printf("test8\r\n");
    p = parse_path(cur_dir,"../../../../");
    printf("p=%s, cur_dir=%s",p, cur_dir);
    assert(strcmp(p, "/")==0);
    free(p);
    printf("test9");
    p = parse_path(cur_dir,"./././//\\\\../../");
    printf("p=%s, cur_dir=%s",p, cur_dir);
    assert(strcmp(p, "/work/")==0);
    free(p);
    printf("test10");
    p = parse_path(cur_dir,"./../../../..././");
    printf("p=%s, cur_dir=%s",p, cur_dir);
    assert(strcmp(p, "/.../")==0);
    free(p);
    printf("test11");

}
int main() {
    test();
    printf("Hello, World!\n");
    return 0;
}