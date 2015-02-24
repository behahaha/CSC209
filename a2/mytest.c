#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#define SIZE 100  //will test the case where all bytes of allocated memory is used. 

//This is an interesting case because we are seeing what will occur when we allocate all the 
//memory that is available to us and then we are also making sure to see that our sfree function 
//works when we free the blocks out of order.
int main(void) {
    mem_init(SIZE);

    char *ptrs[10];
    int i;

    /* Call smalloc 4 times */
    for(i = 0; i < 4; i++) {
        int num_bytes = (i+1) * 10;
        
        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
    print_allocated();
    printf("freeing %p result = %d\n", ptrs[0], sfree(ptrs[0]));
    print_free();
    print_allocated();
    printf("freeing %p result = %d\n", ptrs[3], sfree(ptrs[3]));
    print_free();
    print_allocated();
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));
    print_free();
    print_allocated();
    printf("freeing %p result = %d\n", ptrs[2], sfree(ptrs[2]));
    print_free();
    mem_clean();
    return 0;
}

