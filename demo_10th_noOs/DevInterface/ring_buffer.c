/***************************************************************************
 * Copyright (c) 2015 Koninklijke Philips Electronics N.V.,                *
 * All Rights Reserved.                                                    *
 ***************************************************************************/

//#include <asf.h>
#include "ring_buffer.h"
#include <stdint.h>

#define B_EMPTY -1

B_RET rbuf_init(str_rbuf * rbuf, uint8_t * store, uint8_t entry_size, int16_t max_entries)
{
    if (0x7FFF == max_entries)
    {
        return (B_RET_INVALID);
    }
    rbuf->store        = store;
    rbuf->entry_size   = entry_size;
    rbuf->max_entries  = max_entries;
    rbuf->front        = B_EMPTY;
    rbuf->rear         = B_EMPTY;
    rbuf->count        = 0;
    return (B_RET_OK);
}

B_RET rbuf_clear(str_rbuf * rbuf)
{
    rbuf->front        = B_EMPTY;
    rbuf->rear         = B_EMPTY;
    rbuf->count        = 0;
    return (B_RET_OK);
}

B_RET rbuf_push(str_rbuf * rbuf, uint8_t * data)
{
    uint8_t i =0;
    
    if (rbuf->count >= (rbuf->max_entries))
    {
        return (B_RET_FULL);
    }
    if (rbuf->front >= (rbuf->max_entries - 1))
    {
        rbuf->front = B_EMPTY;
    }
    rbuf->front++;
    rbuf->count++;
    for (i = 0; i < rbuf->entry_size; i++)
    {
        rbuf->store[((rbuf->front * rbuf->entry_size) + i)] = data[i];
    }
    return (B_RET_OK);
}

B_RET rbuf_pull(str_rbuf * rbuf, uint8_t * data)
{
    uint8_t i = 0;
    
    if (rbuf->count == 0)
    {
        return (B_RET_EMPTY);
    }
    if (rbuf->rear >= (rbuf->max_entries - 1))
    {
        rbuf->rear = B_EMPTY;
    }
    rbuf->rear++;
    rbuf->count--;
    for (i = 0; i < rbuf->entry_size; i++)
    {
        data[i] = rbuf->store[((rbuf->rear * rbuf->entry_size) + i)] ;
    }

    /* FIFO 数据读取完了，则读写指针回到起点，不管FIFO  里面是不是存满了。*/
    if (rbuf->count == 0)
    {
        rbuf->front = B_EMPTY;
        rbuf->rear  = B_EMPTY;
    }
    return (B_RET_OK);
}

/* 实现纯粹读指定地址的数据，不改写读指针参数 */
B_RET rbuf_read(str_rbuf * rbuf, uint8_t index, uint8_t * data)
{
    uint8_t i = 0;
    
    if (rbuf->count == 0)
    {
        return (B_RET_EMPTY);
    }

    for (i = 0; i < rbuf->entry_size; i++)
    {
        data[i] = rbuf->store[((index * rbuf->entry_size) + i)] ;
    }
    return (B_RET_OK);
}

/* 返回buffer 里面数据的长度 */
uint16_t rbuf_nr_of_items(str_rbuf * rbuf)
{
    return ( rbuf->count );
}


