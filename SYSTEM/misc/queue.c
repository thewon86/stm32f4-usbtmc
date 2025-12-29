#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"

#define QUEUE_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

int queue_init(queue_t q, const void *pool, int pool_sz, int item_sz)
{
    q->pool_ptr = (uint8_t *)pool;
    q->item_size = item_sz;
    q->item_room = QUEUE_ALIGN(item_sz, 4);
    q->item_numb = pool_sz / q->item_room;

    q->wr_idx = q->rd_idx = q->is_full = 0;

    return 0;
}

bool queue_isempty(queue_t q)
{
    if ((q->wr_idx == q->rd_idx) && (q->is_full == 0)) {
        return true;
    } else {
        return false;
    }
}

bool queue_isfull(queue_t q)
{
    return (q->is_full == 1);
}

int queue_push_item(queue_t q, const void *item)
{
    if (queue_isfull(q)) return -1;

    memcpy(q->pool_ptr + q->wr_idx * q->item_room, item, q->item_size);

    q->wr_idx = (q->wr_idx + 1) % q->item_numb;

    return 0;
}

int queue_pop_item(queue_t q, void *item)
{
    if (queue_isempty(q)) return -1;

    memcpy(item, q->pool_ptr + q->rd_idx * q->item_room, q->item_size);

    q->rd_idx = (q->rd_idx + 1) % q->item_numb;

    return 0;
}
