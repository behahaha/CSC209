#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {
    struct block *curFree = freelist;
    printf("%p and %p\n", curFree, freelist);
    while (curFree != NULL) {
        if (curFree->size - nbytes > 0) {
            struct block *newBlock = malloc(sizeof(struct block));
            newBlock->addr = curFree->addr;
            printf("%p newBlock addr and %p curFree addr\n", newBlock->addr, curFree->addr);
            newBlock->size = nbytes;
            printf("size of newBlock: %d\n", newBlock->size);
            if (allocated_list != NULL) {
                printf("%p\n", allocated_list);
                newBlock->next = allocated_list;
                allocated_list = newBlock;
                printf("allocated: %p, %d, %p ||  newBlock: %p, %d, %p\n", allocated_list->addr, allocated_list->size, allocated_list->next, newBlock->addr, newBlock->size, newBlock->next);
            } else {
                newBlock->next = NULL;
                allocated_list = newBlock;
                printf("NULL: allocated: %p, %d, %p ||  newBlock: %p, %d, %p\n", allocated_list->addr, allocated_list->size, allocated_list->next, newBlock->addr, newBlock->size, newBlock->next);
            }
            printf("\n");
            freelist->addr = freelist->addr + nbytes;
            freelist->size = freelist->size - nbytes;
            return newBlock;
        } else {
            curFree = curFree->next;
        }
    }
     return NULL;
}

int sfree(void *addr) {
    return -1;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }
    freelist = malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;
    allocated_list = NULL;        
} 

void mem_clean(){

}

