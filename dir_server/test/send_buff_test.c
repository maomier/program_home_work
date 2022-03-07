#include <stdio.h>
#include "../header/send_buff.h"
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <langinfo.h>
#include <stdlib.h>

void test(){
    char* ptr = (char*)malloc(1024);
    strcpy(ptr, "hello, this is a test string\r\n");
    printf("init send_buff \r\n");
    send_buff buff = create_send_buff(ptr);
    assert(buff.next_pos==0);
    assert(buff.data_len==30);

    int len;
    assert(*send_buff_get_data_ptr(&buff, &len)=='h');
    assert(len==30);

    printf("consume 5 \r\n");
    assert(send_buff_consume(&buff, 5)==0);
    char* outp = send_buff_get_data_ptr(&buff, &len);
    printf("outp=%s",outp);
    assert(outp[0]==',');
    assert(len==25);


    printf("consume 24\r\n");
    assert(send_buff_consume(&buff, 24)==0);
    assert(*send_buff_get_data_ptr(&buff, &len)=='\n');
    assert(len==1);


    printf("consume 1\r\n");
    assert(send_buff_consume(&buff, 1)==1);
    assert(send_buff_get_data_ptr(&buff, &len)==NULL);
    assert(len==0);
    assert(buff.ptr==NULL);

}
int main() {
    test();
    printf("Hello, World!\n");
    return 0;
}