#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#define SIZE1 4096 * 64 //will test similar cases as simpletest, but will free different block.
#define SIZE0 0 //will test the edge case.
#define SIZE2 20 //will test the case where all bytes of allocated memory is used. 
/* what if smalloc exactly the same size as mem?
 * making sure you never call write_to_mem with null pointer 
 * checking all system calls! 
 */
int freeingFirstBlock(void);
int freeingLastBlock(void);
int allocatingNone(void);
int allocatingAll(void);

int main(void) {
    int flag = 1;
    if (freeingFirstBlock() != 1) {
        printf("failed freeingFirstBlock test!");
        flag = 0;
    }
    if (freeingLastBlock() != 1) {
        printf("failed freeingLastBlock test!");
        flag = 0;
    }
    if (allocatingNone() != 1) {
        printf("failed allocatingNone test!");
        flag = 0;
    }
    if (allocatingAll() != 1) {
        printf("failed allocatingAll test!");
        flag = 0;    
    } 
    if (flag == 0) {
        return 1;
    } else {
        return 0;
    }
}

int freeingFirstBlock(void) {

    mem_init(SIZE1);

    char *ptrs[10];
    int i;

    /* Call smalloc 4 times */
    for(i = 0; i < 4; i++) {
        int num_bytes = (i+1) * 10;

        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    int ret = sfree(ptrs[0]);
    printf("freeing %p result = %d\n", ptrs[0], ret);

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
          
    mem_clean();
    return ret; 
}

int freeingLastBlock(void){

    mem_init(SIZE1);

    char *ptrs[10];
    int i;

    /* Call smalloc 4 times */
    for(i = 0; i < 4; i++) {
        int num_bytes = (i+1) * 10;

        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    int ret = sfree(ptrs[3]);
    printf("freeing %p result = %d\n", ptrs[0], ret);

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
          
    mem_clean();
    return ret;
} 

int allocatingNone(void) {

    mem_init(SIZE0);
    //if
    mem_clean();
}

int allocatingAll(void) {

    mem_init(SIZE2);

    char *ptrs[10];
    int i;

    /* Call smalloc 4 times */
    for(i = 0; i < 4; i++) {
        int num_bytes = (i+1) * 10;

        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
          
    mem_clean();
    return 0;
}
