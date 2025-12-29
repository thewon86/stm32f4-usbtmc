#ifndef __QUEUE_H__
#define __QUEUE_H__

/**
 * queue structure
 */
typedef struct queue_type
{
    uint8_t *pool_ptr;                  /* start address of queue */

    size_t item_size;                   /* size of each item */
    size_t item_room;                   /* actully size of each item */
    uint16_t item_numb;                 /* max number of items */

    uint16_t wr_idx, rd_idx;

    int16_t is_full;                    /* queue is full */
}queue, *queue_t;

int queue_init(queue_t q, const void *pool, int pool_sz, int item_sz);
bool queue_isempty(queue_t q);
bool queue_isfull(queue_t q);
int queue_push_item(queue_t q, const void *item);
int queue_pop_item(queue_t q, void *item);

#endif // __QUEUE_H__
