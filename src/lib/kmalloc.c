#include <lib/kmalloc.h>
#include <utils/memutils.h>
#include <utils/printf.h>
#include <arm/irq.h>
#include <config.h>

// #define ISALIGNED(mem, size) ((mem % size) == 0)
// #define ALIGN(ptr, alignment) ((uint64_t) ptr + ((uint64_t) ptr % alignment))

static free_mem_header_t* first_free;
static void* heap_start_ptr;
static uintptr_t heap_start_adr;
static uint64_t kheap_size = KERNEL_HEAP_SIZE;

void kmalloc_init(void* heap_start) {
    /* zero heap location */
    first_free = heap_start_ptr = heap_start;
    heap_start_adr = (uintptr_t) heap_start_ptr;
    memzero((uint8_t *) first_free, kheap_size);

    /* init first free block of heap */
    first_free->prev = START_OF_HEAP;
    first_free->next = END_OF_HEAP;
    first_free->size = kheap_size;
}

void* kmalloc(uint64_t size) {
    uint64_t alloc_size = size + KERNEL_MALLOC_HEADER_SIZE;

    free_mem_header_t* free_block = first_free;

    if (first_free == 0) {
        printf("KMALLOC FAILED\n");
        return (void *) 0;
    }

    do {
        if(free_block->size >= alloc_size) {
            uint64_t* new_ptr = (uint64_t *) free_block;

            // /* align to 8 bytes*/
            // new_ptr = ALIGN(new_ptr, 8);

            // /* Check if we can fit newly aligned block into this free block */
            // if ((uint64_t) new_ptr + size > ((uint64_t) free_block + free_block->size)) continue;

            /*
            Possible options
            |---free_block---|
            |---free_block---|---???---|
            |---???---|---free_block---|
            */
           
            free_mem_header_t prev_free_block = *free_block;
            free_block = (free_mem_header_t *)(((uintptr_t) free_block) + alloc_size);
            /* only block */
            if (prev_free_block.prev == START_OF_HEAP && prev_free_block.next == END_OF_HEAP) {
                if(prev_free_block.size - alloc_size == 0) {
                    /* will mean that there is no free space left in heap */
                    first_free = (free_mem_header_t *) 0;
                } else {
                    /* Fill in new free zone block */
                    free_block->prev = START_OF_HEAP;
                    free_block->next = END_OF_HEAP;
                    free_block->size = prev_free_block.size - alloc_size;

                    /* Update first_free if we need to (we need to keep track of where our linked list starts) */
                    first_free = free_block;
                }
            }

            /* first free block (and there is a block after) */
            else if (prev_free_block.prev == START_OF_HEAP) {
                if(prev_free_block.size - alloc_size == 0) {
                    /* This free block will no longer exist */
                    prev_free_block.next->prev = START_OF_HEAP;
                    first_free = prev_free_block.next;
                } else {
                    /* Fill in new free zone block */
                    free_block->next = prev_free_block.next;
                    free_block->next->prev = free_block;
                    free_block->size = prev_free_block.size - alloc_size;
                    free_block->prev = START_OF_HEAP;
                    
                    /* Update first_free if we need to (we need to keep track of where our linked list starts) */
                    first_free = free_block;
                }
            } 

            /* last block (there is a block before) */
            else if (prev_free_block.next == END_OF_HEAP) {
                if(prev_free_block.size - alloc_size == 0) {
                    prev_free_block.prev->next = END_OF_HEAP; 
                } else {
                    free_block->prev = prev_free_block.prev;
                    free_block->prev->next = free_block;
                    free_block->size = prev_free_block.size - alloc_size;
                    free_block->next = END_OF_HEAP;
                }
            }

            /* There are free blocks before and after */
            else {
                if(prev_free_block.size - alloc_size == 0) {
                    prev_free_block.prev->next = prev_free_block.next;
                    prev_free_block.next->prev = prev_free_block.prev;
                } else {
                    free_block->prev = prev_free_block.prev;
                    free_block->prev->next = free_block;

                    free_block->next = prev_free_block.next;
                    free_block->next->prev = free_block;

                    free_block->size = prev_free_block.size - alloc_size;
                }
            }
            /* add size to beginning of new ptr so free knows how big this block is */
            *new_ptr = size;

            /* move ptr along by sizeof(size) so that we return variable to assign */
            new_ptr = (uint64_t *) ((uintptr_t) new_ptr + sizeof(size));

            #if EXTRA_DEBUG == DEBUG_ON
            printf("KMALLOC: 0x%08x%08x, size: 0x%08xB\n", (uint64_t) new_ptr >> 32, ((uint64_t) new_ptr << 32) >> 32, size);
            #endif

            return (void *) new_ptr;
        }
        free_block = free_block->next;
    } while ((free_block != END_OF_HEAP) || (free_block != START_OF_HEAP));
    return (void *) 0;
}

void kfree(void* ptr) {
    if (!ptr) return;
    uint64_t header_adr = (uintptr_t) ptr - KERNEL_MALLOC_SIZE_SIZE;
    uint64_t* header_start = (uint64_t *) header_adr;
    uint64_t size = *header_start + KERNEL_MALLOC_HEADER_SIZE;
    free_mem_header_t* free_block = first_free;

    uint8_t found = 0;

    if (!first_free) {
        printf("KFREE FAILED\n");
        return;
    }


    if (free_block->next == END_OF_HEAP && free_block->prev == START_OF_HEAP) {
        found = 1;
    } else {
        while(free_block->next != END_OF_HEAP) {
            if((uintptr_t) free_block > (uintptr_t) ptr) {
                found = 1;
                break;
            }

            free_block = free_block->next;
        }
        if ((!found) && (free_block->next == END_OF_HEAP) && ((uintptr_t) free_block > (uintptr_t) ptr)) {
            found = 1;
        }
    }

    /* Couldn't find our block in the heap (which means either doesn't exist or it is at the end of the heap)*/
    if (!found) {
        uintptr_t free_block_adr = (uintptr_t) free_block;

        /* Check if the block that needs freeing is between the last free block and the end of the heap */
        if ((free_block->next == END_OF_HEAP) && (free_block_adr <= header_adr)) {

            /* Theres another allocated block between end of the last free block and the block we're freeing */
            if (header_adr > free_block_adr) {
                free_mem_header_t* next_free = (free_mem_header_t *) header_start;
                free_block->next = next_free;
                next_free->prev = free_block;
                next_free->size = size;
            } else {
                /* There is no allocated block between end of last free block and what we're freeing so we can just increase size */
                free_block->size += size;
            }
        }
        return;
    }

    /* Our block was before first free block (so < first_free) */
    else if (free_block->prev == START_OF_HEAP) {
        // |---???---|---header_start---|---???---|---free_block---|
        uintptr_t free_block_adr = (uintptr_t) free_block;

        uint8_t is_contig_with_free_block = (header_adr + size == free_block_adr);

        /* This allocated block is directly before first_free (logic is same whether it's the first allocated block or not) */
        if (is_contig_with_free_block) {
            // |---???---|---header_start---|---free_block---|
            /* move free_block back by size amount */
            free_block_adr -= size;

            free_mem_header_t tmp = *free_block;
            tmp.size += size;

            free_block = (free_mem_header_t *) free_block_adr;
            *free_block = tmp;

            first_free = free_block;
            if (free_block->next != END_OF_HEAP) {
                free_block->next->prev = free_block;
            }
            // |---???---|----------------------free_block---|
        } else {
            // |---???---|---header_start---|---???---|---free_block---|
            /* Create a new free block for this */
            free_mem_header_t* new_free = (free_mem_header_t *) header_start;
            new_free->prev = START_OF_HEAP;
            new_free->next = free_block;
            new_free->size = size;

            free_block->prev = new_free;

            first_free = new_free;
            // |---???---|---new_free---|---???---|---free_block---|
        }
    }

    /* General case (Block that is to be freed isn't at beginning or end) */
    else {
        // |---free_block->prev---|---???---|---header_start---|---???---|---free_block---| 
        free_mem_header_t* prev = free_block->prev;
        uintptr_t prev_adr = (uintptr_t) prev;
        uintptr_t end_of_prev = prev_adr + prev->size;
        uintptr_t header_adr = (uintptr_t) header_start;
        uintptr_t free_block_adr = (uintptr_t) free_block;
        
        /* Is header_start right after free_block->prev */
        char prev_contig_header = (end_of_prev + 1 == header_adr);

        /* Is header right before free_block */
        char header_contig_free_block = (header_adr + size + 1 == free_block_adr);

        /* Can merge prev and free_block into one super block*/
        if (prev_contig_header && header_contig_free_block) {
            // |---free_block->prev---|---header_start---|---free_block---|
            prev->next = free_block->next;
            prev->size += size + free_block->size;
            
            if (free_block->next != END_OF_HEAP) {
                free_block->next->prev = prev;
            }
            // |---free_block---------------------------------------------|
        } else if (prev_contig_header) {
            // |---free_block->prev---|---header_start---|---???---|---free_block---|
            prev->size += size;
            // |---free_block->prev----------------------|---???---|---free_block---|
        } else if (header_contig_free_block) {
            // |---free_block->prev---|---???---|---header_start---|---free_block---|
            free_block_adr -= size;
            
            free_mem_header_t tmp = *free_block;
            tmp.size += size;

            free_block = (free_mem_header_t *) free_block_adr;
            *free_block = tmp;

            free_block->prev->next = free_block;
            // |---free_block_prev---|---???---|-----------------------free_block---|
        } else {
            // |---free_block->prev---|---???---|---header_start---|---???---|---free_block---|
            free_mem_header_t* new_block = (free_mem_header_t *) header_start;
            new_block->size = size;

            new_block->next = free_block;
            free_block->prev = new_block;

            new_block->prev = prev;
            prev->next = new_block;
            // |---free_block->prev---|---???---|---new_block---|---???---|---free_block---|
        }
    }
}