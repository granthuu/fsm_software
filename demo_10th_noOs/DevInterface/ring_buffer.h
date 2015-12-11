/***************************************************************************
 * Copyright (c) 2015 Koninklijke Philips Electronics N.V.,                *
 * All Rights Reserved.                                                    *
 ***************************************************************************/

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

//#include <asf.h>
#include <inttypes.h>

typedef enum
{
    B_RET_OK,
    B_RET_FULL,
    B_RET_EMPTY,
    B_RET_INVALID,
    B_RET_ERROR
}B_RET;

typedef struct
{
    int16_t front;
    int16_t rear;
    int16_t count;
    uint8_t *store;
    uint8_t entry_size;
    int16_t max_entries;
}str_rbuf;

B_RET rbuf_init(str_rbuf * rbuf, uint8_t * store, uint8_t entry_size, int16_t max_entries);
B_RET rbuf_push(str_rbuf * rbuf, uint8_t * data);
B_RET rbuf_pull(str_rbuf * rbuf, uint8_t * data);
B_RET rbuf_read(str_rbuf * rbuf, uint8_t index, uint8_t * data);
B_RET rbuf_clear(str_rbuf * rbuf);
uint16_t rbuf_nr_of_items(str_rbuf * rbuf);

#endif /* RING_BUFFER_H_ */
