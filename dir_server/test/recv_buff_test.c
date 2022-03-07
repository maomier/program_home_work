#include <stdio.h>
#include "../header/recv_buff.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

char* g_dt = NULL;
int parse_out_count = 0;
int err_count = 0;
void init_out_params(){
    g_dt = NULL;
    parse_out_count = 0;
    err_count  = 0;
}
void parse_out(char* dt, void* ptr){
    //printf("dt=%s\r\n",dt);
    if(g_dt){
        free(g_dt);
        g_dt = NULL;
    }
    g_dt = strdup(dt);
    printf("gdt=%s\r\n",g_dt);
    parse_out_count++;
}
void err_out(void* ptr){
    err_count++;
}
void test(){
    recv_buff* buff = create_recv_buff();
    //1 |\r\n..........|\0|
    buff->ptr[0] = '\r';
    buff->ptr[1] = '\n';
    init_out_params();
    recv_buff_parse(buff, 2, parse_out, err_out,NULL);
    assert(parse_out_count==0);
    assert(err_count==0);
    assert(buff->next_pos == 0);
    printf("test case 1 complete\r\n");
    //2 |ab\r\n..........|\0|
    buff->ptr[0] = 'a';
    buff->ptr[1] = 'b';
    buff->ptr[2] = '\r';
    buff->ptr[3] = '\n';
    init_out_params();
    recv_buff_parse(buff, 4, parse_out, err_out,NULL);
    assert(parse_out_count==1);
    assert(err_count==0);
    assert(strcmp(g_dt,"ab")==0);
    assert(buff->next_pos == 0);
    printf("test case 2 complete\r\n");
    //3 |ab\r\n\r\n..........|\0|
    buff->ptr[0] = 'a';
    buff->ptr[1] = 'b';
    buff->ptr[2] = '\r';
    buff->ptr[3] = '\n';
    buff->ptr[4] = '\r';
    buff->ptr[5] = '\n';
    init_out_params();
    recv_buff_parse(buff, 6, parse_out, err_out,NULL);
    assert(parse_out_count==1);
    assert(err_count==0);
    assert(strcmp(g_dt,"ab")==0);
    assert(buff->next_pos == 0);
    printf("test case 3 complete\r\n");
    //4 |ab\r\nabcde\r\n..........|\0|
    buff->ptr[0] = 'a';
    buff->ptr[1] = 'b';
    buff->ptr[2] = '\r';
    buff->ptr[3] = '\n';
    buff->ptr[4] = 'a';
    buff->ptr[5] = 'b';
    buff->ptr[6] = 'c';
    buff->ptr[7] = 'd';
    buff->ptr[8] = 'e';
    buff->ptr[9] = '\r';
    buff->ptr[10] = '\n';
    init_out_params();
    recv_buff_parse(buff, 11, parse_out, err_out,NULL);
    assert(parse_out_count==2);
    assert(err_count==0);
    assert(strcmp(g_dt,"abcde")==0);
    assert(buff->next_pos == 0);
    printf("test case 4 complete\r\n");
    //5 |ab\r\nabcde\r\n\r\n..........|\0|
    buff->ptr[0] = 'a';
    buff->ptr[1] = 'b';
    buff->ptr[2] = '\r';
    buff->ptr[3] = '\n';
    buff->ptr[4] = 'a';
    buff->ptr[5] = 'b';
    buff->ptr[6] = 'c';
    buff->ptr[7] = 'd';
    buff->ptr[8] = 'e';
    buff->ptr[9] = '\r';
    buff->ptr[10] = '\n';
    buff->ptr[11] = '\r';
    buff->ptr[12] = '\n';
    init_out_params();
    recv_buff_parse(buff, 13, parse_out, err_out,NULL);
    assert(parse_out_count==2);
    assert(err_count==0);
    assert(strcmp(g_dt,"abcde")==0);
    assert(buff->next_pos == 0);
    printf("test case 5 complete\r\n");
    //6 |aaaaaaaaaaaaaaaaaaaaa|\0|
    char* pdest = recv_buff_get_usefule_space(buff);
    int dest_len = recv_buff_get_usefule_space_len(buff);
    memset(pdest, 'a', dest_len);
    assert(strlen(pdest)==dest_len);
    assert(strlen((char*)buff->ptr)==dest_len);
    init_out_params();
    buff->next_pos = 0;
    recv_buff_parse(buff, dest_len, parse_out, err_out,NULL);
    assert(parse_out_count==0);
    assert(err_count==1);
    printf("test case 6 complete\r\n");
    //7 |aaaaaaaaaaaaaaaaaaaa\r|\0|
    pdest = recv_buff_get_usefule_space(buff);
    dest_len = recv_buff_get_usefule_space_len(buff);
    memset(pdest, 'a', dest_len);
    pdest[dest_len-1]='\r';
    printf("init data for 7=%s", pdest);
    assert(strlen(pdest)==dest_len);
    assert(strlen((char*)buff->ptr)==dest_len);
    init_out_params();
    buff->next_pos = 0;
    recv_buff_parse(buff, dest_len, parse_out, err_out,NULL);
    assert(parse_out_count==0);
    assert(err_count==1);
    printf("test case 7 complete\r\n");
    //8 |aaaaaaaaaaaaaaaaaa\r\n|\0|
    pdest = recv_buff_get_usefule_space(buff);
    dest_len = recv_buff_get_usefule_space_len(buff);
    memset(pdest, 'a', dest_len);
    pdest[dest_len-2]='\r';
    pdest[dest_len-1]='\n';
    assert(strlen(pdest)==dest_len);
    assert(strlen((char*)buff->ptr)==dest_len);
    init_out_params();
    buff->next_pos = 0;
    printf("step81\r\n");
    recv_buff_parse(buff, dest_len, parse_out, err_out,NULL);
    printf("step 82\r\n");
    assert(parse_out_count==1);
    printf("step 83\r\n");
    char* dt = calloc(dest_len+1,1);
    memset(dt, 'a', dest_len);
    dt[dest_len-2]= 0;
    dt[dest_len-1]='\n';
    printf("step 84\r\n");
    printf("g_dt=%s\r\n",g_dt);
    printf("dddt=%s\r\n",dt);
    assert(strcmp(g_dt,dt)==0);
    free(dt);
    assert(err_count==0);
    printf("test case 8 complete\r\n");
    //9 |aaaaaaaaaaaaaaaaaa\r\nx|\0|
    pdest = recv_buff_get_usefule_space(buff);
    dest_len = recv_buff_get_usefule_space_len(buff);
    memset(pdest, 'a', dest_len);
    pdest[dest_len-3]='\r';
    pdest[dest_len-2]='\n';
    pdest[dest_len-1]='x';
    assert(strlen(pdest)==dest_len);
    assert(strlen((char*)buff->ptr)==dest_len);
    init_out_params();
    buff->next_pos = 0;
    printf("step91\r\n");
    recv_buff_parse(buff, dest_len, parse_out, err_out,NULL);
    printf("step 92\r\n");
    assert(parse_out_count==1);
    printf("step 93\r\n");
    dt = calloc(dest_len+1,1);
    memset(dt, 'a', dest_len);
    dt[dest_len-3]= 0;
    dt[dest_len-1]='\n';
    printf("step 94\r\n");
    printf("g_dt=%s\r\n",g_dt);
    printf("dddt=%s\r\n",dt);
    assert(strcmp(g_dt,dt)==0);
    free(dt);
    assert(err_count==0);
    printf("test case 9 complete\r\n");
}
int main() {
    test();
    printf("Hello, World!\n");
    return 0;
}