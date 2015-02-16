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
    struct block *curAllo = allocated_list;
    while (curFree != NULL) {
        if (curFree->size - nbytes > 0) {
            struct block *newBlock = malloc(sizeof(struct block));
            newBlock->addr = curFree->addr;
            newBlock->size = nbytes;
            if (curAllo != NULL) {
                newBlock->next = curAllo;
            } else {
                newBlock->next = NULL;
            }
            allocated_list = newBlock;
            freelist->addr = freelist->addr + nbytes;
            freelist->size = freelist->size - nbytes;
            return allocated_list;
        } else {
            curFree = curFree->next;
        }
    } return NULL;
    // if (freelist->size - nbytes > 0) {
    //     struct block *newBlock = malloc(sizeof(struct block));
    //     newBlock->addr = freelist->addr;
    //     newBlock->size = nbytes;
    //     if (allocated_list == NULL) {
    //         newBlock->next = NULL;
    //     } else {
    //         newBlock->next = &*allocated_list;
    //     }
    //     allocated_list = newBlock;
    //     freelist->addr += nbytes;
    //     freelist->size -= nbytes;
    //     return allocated_list;
    // } else {
    //     return NULL;
    // }
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
    freelist->size = sizeof(*mem);
    freelist->next = NULL;
    allocated_list = NULL;        
} 

void mem_clean(){

}

