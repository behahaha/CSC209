#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"

#define SIZE1 4096 * 64 //will test similar cases as simpletest, but will free different block.
#define SIZE0 0 //will test the edge case.
#define SIZE2 100  //will test the case where all bytes of allocated memory is used. 

int freeingFirstBlock(void);
int freeingLastBlock(void);
int allocatingNone(void);
int allocatingAll(void);
int allocateSeven(void);

//Runs the tests, will return 1 if there's been an error or one of the tests failed, 0 otherwise.
int main(void) {
    int flag = 1;
    printf("##################### FREEINGFIRSTBLOCK TEST ####################################\n");
    int first = freeingFirstBlock();
    if (first != 0) {
        printf("failed freeingFirstBlock test!\n");
        flag = 0;
    }
    printf("##################### FREEEINGLASTTBLOCK TEST ####################################\n");
    if (freeingLastBlock() != 0) {
        printf("failed freeingLastBlock test!\n");
        flag = 0;
    }
    printf("##################### ALLOCATINGNONE TEST ####################################\n");
    if (allocatingNone() != 0) {
        printf("failed allocatingNone test!\n");
        flag = 0;
    }
    printf("#################### ALLOCATINGALL TEST ####################################\n");
    if (allocatingAll() != 0) {
        printf("failed allocatingAll test!\n");
        flag = 0;    
    } 
    printf("################### ALLOCATINGSEVEN TEST ###################################\n");
    if (allocateSeven() != 0) {
        printf("failed allocateSeven test!\n");
        flag = 0;
    }
    if (flag == 0) {
        return 1;
    } else {
        return 0;
    }
}

//freeing the first block when there are 4 blocks allocated in allocated_list
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

//freeing the last block(most recently allocated) when there are 4 blocks allocated in allocated list
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
    printf("freeing %p result = %d\n", ptrs[3], ret);

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
          
    mem_clean();
    return ret;
} 

//allocating no memory
int allocatingNone(void) {

    mem_init(SIZE0);
    printf("free blocks: \n");
    printf("allocated blocks: \n");
    mem_clean();
    return 0;
}

//allocating all the memory that was put into mem_init and then freeing it all
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
    printf("freeing %p result = %d\n", ptrs[0], sfree(ptrs[0]));
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));
    printf("freeing %p result = %d\n", ptrs[2], sfree(ptrs[2]));
    printf("freeing %p result = %d\n", ptrs[3], sfree(ptrs[3]));
    mem_clean();
    return 0;
}

//allocating 7 bytes of memory since there were some tricks with values under 8
int allocateSeven(void) {
    mem_init(7);
    void *ptr;
    ptr = smalloc(7);
    printf("free: \n");
    print_free();
    printf("allocated: \n");
    print_allocated();
    printf("freeing %p result = %d\n", ptr, sfree(ptr));
    printf("free after free: \n");
    print_free();
    printf("allocated after free: \n");
    print_allocated();    
    mem_clean();
    return 0;
}
