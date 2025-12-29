#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "user_memheap.h"

#define MEMHEAP_ALIGN_SIZE                  4
#define MEMHEAP_ALIGN(size, align)          (((size) + (align) - 1) & ~((align) - 1))
#define MEMHEAP_ALIGN_DOWN(size, align)     ((size) & ~((align) - 1))

/* dynamic pool magic and mask */
#define MEMHEAP_MAGIC           0xAEA5AEA5
#define MEMHEAP_MASK            0xFFFFFFFE
#define MEMHEAP_USED            0x01
#define MEMHEAP_FREED           0x00

#define MEMHEAP_IS_USED(i)      ((i)->magic & MEMHEAP_USED)
#define MEMHEAP_MINIALLOC       12

#define MEMHEAP_SIZE            MEMHEAP_ALIGN(sizeof(struct memheap_item), MEMHEAP_ALIGN_SIZE)
#define MEMITEM_SIZE(item)      ((uint32_t)item->next - (uint32_t)item - MEMHEAP_SIZE)
#define MEMITEM(ptr)            (struct memheap_item*)((uint8_t*)ptr - MEMHEAP_SIZE)

struct memheap_item
{
    uint32_t             magic;                      /**< magic number for memheap */
    struct memheap      *pool_ptr;                   /**< point of pool */

    struct memheap_item *next;                       /**< next memheap item */
    struct memheap_item *prev;                       /**< prev memheap item */

    struct memheap_item *next_free;                  /**< next free memheap item */
    struct memheap_item *prev_free;                  /**< prev free memheap item */
};

struct memheap
{
    void                *addr;                       /**< pool start address and size */

    uint32_t             pool_size;                  /**< pool size */
    uint32_t             available_size;             /**< available size */
    uint32_t             max_used_size;              /**< maximum allocated size */

    struct memheap_item *block_list;                 /**< used block list */

    struct memheap_item *free_list;                  /**< free block list */
    struct memheap_item  free_header;                /**< free block list header */
};

static MemHeap _heap, sdramHeap;

/**
 * @brief   This function initializes a piece of memory called memheap.
 *
 * @note    The initialized memory pool will be:
 *          +-----------------------------------+--------------------------+
 *          | whole freed memory block          | Used Memory Block Tailer |
 *          +-----------------------------------+--------------------------+
 *
 *          block_list --> whole freed memory block
 *
 *          The length of Used Memory Block Tailer is 0,
 *          which is prevents block merging across list
 *
 * @param   memheap is a pointer of the memheap object.
 *
 * @param   name is the name of the memheap.
 *
 * @param   addr is the start address of the memheap.
 *
 * @param   size is the size of the memheap.
 *
 * @return  0
 */
int memheap_init(MemHeap *memheap, void* addr, size_t size)
{
    struct memheap_item *item;

    memheap->addr           = addr;
    memheap->pool_size      = MEMHEAP_ALIGN_DOWN(size, MEMHEAP_ALIGN_SIZE);
    memheap->available_size = memheap->pool_size - (2 * MEMHEAP_SIZE);
    memheap->max_used_size  = memheap->pool_size - memheap->available_size;

    /* initialize the free list header */
    item            = &(memheap->free_header);
    item->magic     = (MEMHEAP_MAGIC | MEMHEAP_FREED);
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    /* set the free list to free list header */
    memheap->free_list = item;

    /* initialize the first big memory block */
    item            = (struct memheap_item *)addr;
    item->magic     = (MEMHEAP_MAGIC | MEMHEAP_FREED);
    item->pool_ptr  = memheap;
    item->next      = NULL;
    item->prev      = NULL;
    item->next_free = item;
    item->prev_free = item;

    item->next = (struct memheap_item *)
                 ((uint8_t *)item + memheap->available_size + MEMHEAP_SIZE);
    item->prev = item->next;

    /* block list header */
    memheap->block_list = item;

    /* place the big memory block to free list */
    item->next_free = memheap->free_list->next_free;
    item->prev_free = memheap->free_list;
    memheap->free_list->next_free->prev_free = item;
    memheap->free_list->next_free            = item;

    /* move to the end of memory pool to build a small tailer block,
     * which prevents block merging
     */
    item = item->next;
    /* it's a used memory block */
    item->magic     = (MEMHEAP_MAGIC | MEMHEAP_USED);
    item->pool_ptr  = memheap;
    item->next      = (struct memheap_item *)addr;
    item->prev      = (struct memheap_item *)addr;
    /* not in free list */
    item->next_free = item->prev_free = NULL;

    return 0;
}

/**
 * @brief  Allocate a block of memory with a minimum of 'size' bytes on memheap.
 *
 * @param   heap is a pointer for memheap object.
 *
 * @param   size is the minimum size of the requested block in bytes.
 *
 * @return  the pointer to allocated memory or NULL if no free memory was found.
 */
void *memheap_alloc(struct memheap *heap, size_t size)
{
    int result;
    uint32_t free_size;
    struct memheap_item *header_ptr;

    /* align allocated size */
    size = MEMHEAP_ALIGN(size, MEMHEAP_ALIGN_SIZE);
    if (size < MEMHEAP_MINIALLOC)
        size = MEMHEAP_MINIALLOC;

    if (size < heap->available_size)
    {
        /* search on free list */
        free_size = 0;

        /* get the first free memory block */
        header_ptr = heap->free_list->next_free;
        while (header_ptr != heap->free_list && free_size < size)
        {
            /* get current freed memory block size */
            free_size = MEMITEM_SIZE(header_ptr);
            if (free_size < size)
            {
                /* move to next free memory block */
                header_ptr = header_ptr->next_free;
            }
        }

        /* determine if the memory is available. */
        if (free_size >= size)
        {
            /* a block that satisfies the request has been found. */

            /* determine if the block needs to be split. */
            if (free_size >= (size + MEMHEAP_SIZE + MEMHEAP_MINIALLOC))
            {
                struct memheap_item *new_ptr;

                /* split the block. */
                new_ptr = (struct memheap_item *)
                          (((uint8_t *)header_ptr) + size + MEMHEAP_SIZE);

                /* mark the new block as a memory block and freed. */
                new_ptr->magic = (MEMHEAP_MAGIC | MEMHEAP_FREED);

                /* put the pool pointer into the new block. */
                new_ptr->pool_ptr = heap;

                /* break down the block list */
                new_ptr->prev          = header_ptr;
                new_ptr->next          = header_ptr->next;
                header_ptr->next->prev = new_ptr;
                header_ptr->next       = new_ptr;

                /* remove header ptr from free list */
                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;

                /* insert new_ptr to free list */
                new_ptr->next_free = heap->free_list->next_free;
                new_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = new_ptr;
                heap->free_list->next_free            = new_ptr;

                /* decrement the available byte count.  */
                heap->available_size = heap->available_size -
                                       size -
                                       MEMHEAP_SIZE;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;
            }
            else
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - free_size;
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                header_ptr->next_free->prev_free = header_ptr->prev_free;
                header_ptr->prev_free->next_free = header_ptr->next_free;
                header_ptr->next_free = NULL;
                header_ptr->prev_free = NULL;
            }

            /* Mark the allocated block as not available. */
            header_ptr->magic = (MEMHEAP_MAGIC | MEMHEAP_USED);

            return (void *)((uint8_t *)header_ptr + MEMHEAP_SIZE);
        }
    }

    /* Return the completion status.  */
    return NULL;
}

/**
 * @brief This function will change the size of previously allocated memory block.
 *
 * @param heap is a pointer to the memheap object, which will reallocate
 *             memory from the block
 *
 * @param ptr is a pointer to start address of memory.
 *
 * @param newsize is the required new size.
 *
 * @return the changed memory block address.
 */
void *memheap_realloc(struct memheap *heap, void *ptr, size_t newsize)
{
    int result;
    size_t oldsize;
    struct memheap_item *header_ptr;
    struct memheap_item *new_ptr;

    if (newsize == 0)
    {
        memheap_free(ptr);

        return NULL;
    }
    /* align allocated size */
    newsize = MEMHEAP_ALIGN(newsize, MEMHEAP_ALIGN_SIZE);
    if (newsize < MEMHEAP_MINIALLOC)
        newsize = MEMHEAP_MINIALLOC;

    if (ptr == NULL)
    {
        return memheap_alloc(heap, newsize);
    }

    /* get memory block header and get the size of memory block */
    header_ptr = (struct memheap_item *)
                 ((uint8_t *)ptr - MEMHEAP_SIZE);
    oldsize = MEMITEM_SIZE(header_ptr);
    /* re-allocate memory */
    if (newsize > oldsize)
    {
        void *new_ptr;
        /* Fix the crash problem after opening Oz optimization on ac6  */
        struct memheap_item *next_ptr;

        next_ptr = header_ptr->next;

        /* check whether the following free space is enough to expand */
        if (!MEMHEAP_IS_USED(next_ptr))
        {
            int32_t nextsize;

            nextsize = MEMITEM_SIZE(next_ptr);

            /* Here is the ASCII art of the situation that we can make use of
             * the next free node without alloc/memcpy, |*| is the control
             * block:
             *
             *      oldsize           free node
             * |*|-----------|*|----------------------|*|
             *         newsize          >= minialloc
             * |*|----------------|*|-----------------|*|
             */
            if (nextsize + oldsize > newsize + MEMHEAP_MINIALLOC)
            {
                /* decrement the entire free size from the available bytes count. */
                heap->available_size = heap->available_size - (newsize - oldsize);
                if (heap->pool_size - heap->available_size > heap->max_used_size)
                    heap->max_used_size = heap->pool_size - heap->available_size;

                next_ptr->next_free->prev_free = next_ptr->prev_free;
                next_ptr->prev_free->next_free = next_ptr->next_free;
                next_ptr->next->prev = next_ptr->prev;
                next_ptr->prev->next = next_ptr->next;

                /* build a new one on the right place */
                next_ptr = (struct memheap_item *)((char *)ptr + newsize);

                /* mark the new block as a memory block and freed. */
                next_ptr->magic = (MEMHEAP_MAGIC | MEMHEAP_FREED);

                /* put the pool pointer into the new block. */
                next_ptr->pool_ptr = heap;

                next_ptr->prev          = header_ptr;
                next_ptr->next          = header_ptr->next;
                header_ptr->next->prev = (struct memheap_item *)next_ptr;
                header_ptr->next       = (struct memheap_item *)next_ptr;

                /* insert next_ptr to free list */
                next_ptr->next_free = heap->free_list->next_free;
                next_ptr->prev_free = heap->free_list;
                heap->free_list->next_free->prev_free = (struct memheap_item *)next_ptr;
                heap->free_list->next_free            = (struct memheap_item *)next_ptr;

                return ptr;
            }
        }

        /* re-allocate a memory block */
        new_ptr = (void *)memheap_alloc(heap, newsize);
        if (new_ptr != NULL)
        {
            memcpy(new_ptr, ptr, oldsize < newsize ? oldsize : newsize);
            memheap_free(ptr);
        }

        return new_ptr;
    }

    /* don't split when there is less than one node space left */
    if (newsize + MEMHEAP_SIZE + MEMHEAP_MINIALLOC >= oldsize)
        return ptr;

    /* split the block. */
    new_ptr = (struct memheap_item *)
              (((uint8_t *)header_ptr) + newsize + MEMHEAP_SIZE);

    /* mark the new block as a memory block and freed. */
    new_ptr->magic = (MEMHEAP_MAGIC | MEMHEAP_FREED);
    /* put the pool pointer into the new block. */
    new_ptr->pool_ptr = heap;

    /* break down the block list */
    new_ptr->prev          = header_ptr;
    new_ptr->next          = header_ptr->next;
    header_ptr->next->prev = new_ptr;
    header_ptr->next       = new_ptr;

    /* determine if the block can be merged with the next neighbor. */
    if (!MEMHEAP_IS_USED(new_ptr->next))
    {
        struct memheap_item *free_ptr;

        /* merge block with next neighbor. */
        free_ptr = new_ptr->next;
        heap->available_size = heap->available_size - MEMITEM_SIZE(free_ptr);

        free_ptr->next->prev = new_ptr;
        new_ptr->next   = free_ptr->next;

        /* remove free ptr from free list */
        free_ptr->next_free->prev_free = free_ptr->prev_free;
        free_ptr->prev_free->next_free = free_ptr->next_free;
    }

    /* insert the split block to free list */
    new_ptr->next_free = heap->free_list->next_free;
    new_ptr->prev_free = heap->free_list;
    heap->free_list->next_free->prev_free = new_ptr;
    heap->free_list->next_free            = new_ptr;

    /* increment the available byte count.  */
    heap->available_size = heap->available_size + MEMITEM_SIZE(new_ptr);

    /* return the old memory block */
    return ptr;
}

/**
 * @brief This function will release the allocated memory block by
 *        malloc. The released memory block is taken back to system heap.
 *
 * @param ptr the address of memory which will be released.
 */
void memheap_free(void *ptr)
{
    int result;
    struct memheap *heap;
    struct memheap_item *header_ptr, *new_ptr;
    uint32_t inseheader;

    /* NULL check */
    if (ptr == NULL) return;

    /* set initial status as OK */
    inseheader = 1;
    new_ptr       = NULL;
    header_ptr    = (struct memheap_item *)
                    ((uint8_t *)ptr - MEMHEAP_SIZE);

    /* get pool ptr */
    heap = header_ptr->pool_ptr;

    /* Mark the memory as available. */
    header_ptr->magic = (MEMHEAP_MAGIC | MEMHEAP_FREED);
    /* Adjust the available number of bytes. */
    heap->available_size += MEMITEM_SIZE(header_ptr);

    /* Determine if the block can be merged with the previous neighbor. */
    if (!MEMHEAP_IS_USED(header_ptr->prev))
    {
        /* adjust the available number of bytes. */
        heap->available_size += MEMHEAP_SIZE;

        /* yes, merge block with previous neighbor. */
        (header_ptr->prev)->next = header_ptr->next;
        (header_ptr->next)->prev = header_ptr->prev;

        /* move header pointer to previous. */
        header_ptr = header_ptr->prev;
        /* don't insert header to free list */
        inseheader = 0;
    }

    /* determine if the block can be merged with the next neighbor. */
    if (!MEMHEAP_IS_USED(header_ptr->next))
    {
        /* adjust the available number of bytes. */
        heap->available_size += MEMHEAP_SIZE;

        /* merge block with next neighbor. */
        new_ptr = header_ptr->next;

        new_ptr->next->prev = header_ptr;
        header_ptr->next    = new_ptr->next;

        /* remove new ptr from free list */
        new_ptr->next_free->prev_free = new_ptr->prev_free;
        new_ptr->prev_free->next_free = new_ptr->next_free;
    }

    if (inseheader)
    {
        /* no left merge, insert to free list */
        header_ptr->next_free = heap->free_list->next_free;
        header_ptr->prev_free = heap->free_list;
        heap->free_list->next_free->prev_free = header_ptr;
        heap->free_list->next_free            = header_ptr;
    }

}


void initOnChipMemoryHeap(void* addr, size_t size)
{
    memheap_init(&_heap, addr, size);
}

void initSdramMemoryHeap(void* addr, size_t size)
{
    memheap_init(&sdramHeap, addr, size);
}

void *mallocE(size_t size)
{
    void *ptr;

    ptr = memheap_alloc(&sdramHeap, size);
    return ptr;
}

void *callocE(size_t count, size_t size)
{
    void *ptr;
    size_t total_size;

    total_size = count * size;
    ptr = memheap_alloc(&sdramHeap, size);
    if (ptr != NULL) {
        /* clean memory */
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void *reallocE(void *addr, size_t size)
{
    if (size == 0 && addr == NULL) return NULL;
    if (size == 0 && addr != NULL) {freeE(addr);return NULL;}
    if (addr == NULL) return mallocE(size);

    return memheap_realloc(&sdramHeap, addr, size);
}

void freeE(void *addr)
{
    memheap_free(addr);
}
