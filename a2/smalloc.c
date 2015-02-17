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
    while (curFree != NULL) {
        if (curFree->size - nbytes > 0) {
            struct block *newBlock = malloc(sizeof(struct block));
            newBlock->addr = curFree->addr;
            newBlock->size = nbytes;
            if (allocated_list != NULL) {
                newBlock->next = allocated_list;
                allocated_list = newBlock;
            } else {
                newBlock->next = NULL;
                allocated_list = newBlock;
            }
            freelist->addr += nbytes;
            freelist->size -= nbytes;
            return newBlock->addr;
        } else {
            curFree = curFree->next;
        }
    }
     return NULL;
}

int sfree(void *addr) {
    struct block *cur;
    struct block *rightblock = NULL;
    struct block *before = NULL;
    struct block *after = NULL;
    for (cur = allocated_list; cur != NULL; cur = cur->next) {
        if (cur->addr == addr) {
            printf("cur:%d , cur_next: %p\n", cur->size, cur->next);
            rightblock = cur;
            after = cur->next;
            rightblock->next = NULL;
        } else {
            before = cur;
        }
    }
    if (rightblock == NULL) {
        return -1;
    } else {
        if (before == NULL && after == NULL) {
            allocated_list = NULL;
        } else if (before == NULL) {
            allocated_list = after;
        } else if (after == NULL) {
            before->next = NULL;
        } else if (after != NULL && before != NULL) {
            before->next = after;
        } else {
            return -1;
        }           
    }
    cur = NULL; //we want to use this variable again for searching the freelist
    before = NULL; //we want to use this variable again for searching the freelist
    after = NULL;
    for (cur = freelist; cur != NULL; cur = cur->next) {
        if (rightblock->addr < cur->addr) {
            if (before == NULL && cur->next != NULL) {
                freelist = rightblock;
                rightblock->next = cur->next;
            } else if (before != NULL && cur->next != NULL) {
                before->next = rightblock;
                rightblock->next = cur->next;
            } else if (before == NULL && cur->next == NULL) {
                freelist = rightblock;
                rightblock->next = cur;
            } else {
                return -1;
            }
        } else if (rightblock->addr > cur->addr) {
            before = cur;
        }  
    }
    if (freelist == NULL) {
        freelist = rightblock;
    } 
    return 0;
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

void mem_clean() {
    struct block *freed = freelist;
    struct block *ncur;
    struct block *allo = allocated_list;
    while (freed != NULL) {
        ncur = freed->next;
        free(freed);
        freed = ncur;
    }
    while (allo != NULL) {
        ncur = allo->next;
        free(allo);
        allo = ncur;
    }
}

