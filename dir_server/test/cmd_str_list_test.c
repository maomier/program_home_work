#include <stdio.h>
#include "../header/cmd_str_list.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <langinfo.h>

void* read_thread(void* in_param){
    cmd_str_list* list = (cmd_str_list*)in_param;
    int count = 0;
    while(true){
        cmd_str_unit unit = cmd_str_list_pop_front(list);
        if(is_invalid_cmd(&unit)){
            printf("%d: received invalid item\r\n",++count);
            break;
        }else{
            printf("%d: received the unit:%s\r\n", ++count, unit.cmd_str);
        }
    }
    return NULL;
}
void test(){
    cmd_str_list* cmd_list = create_cmd_str_list();
    pthread_t th;
    int ret = pthread_create(&th, NULL, read_thread, cmd_list );
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("abcdefg",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_cmd_unit("skskskksksksksksskskks",NULL));
    cmd_str_list_push(cmd_list, create_invalid_unit());
    pthread_join(th, NULL);
}
int main() {
    printf("CODESET:%s\r\n", nl_langinfo(CODESET));
    //printf("\033[1;31;40mmessage\033[0m");
    test();
    printf("Hello, World!\n");
    return 0;
}