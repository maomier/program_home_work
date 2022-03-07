#include <stdio.h>
#include "../header/ref_count.h"
#include <assert.h>
#include <string.h>

void ref_free(void* data){
    printf("ready to release ref_data=%s\r\n",(char*)data);
}

void test_ref_count(){
    char* pd = ref_count_alloc(16, ref_free);
    strcpy(pd, "abcdefg");
    printf("pd out = %s\r\n", pd);
    assert(ref_count_number((void*)pd) == 1);
    ref_count_retain((void*)pd);
    assert(ref_count_number((void*)pd) == 2);
    ref_count_release((void*)pd);
    assert(ref_count_number((void*)pd) == 1);
    ref_count_release((void*)pd);
}
int main() {
    test_ref_count();
    printf("Hello, World!\n");
    return 0;
}