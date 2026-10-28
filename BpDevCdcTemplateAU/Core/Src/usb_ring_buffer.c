/*
 * usb_ring_buffer.c
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */

#include "usb_ring_buffer.h"

void RingBuffer_Init(RingBuffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->overflow = 0;
}

bool RingBuffer_Put(RingBuffer_t *rb, uint8_t data)
{
    uint16_t next = (rb->head + 1) % USB_RX_BUFFER_SIZE;

    if (next == rb->tail) {
        rb->overflow++;   // Ochrona przed przepełnieniem
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next;
    return true;
}

bool RingBuffer_Get(RingBuffer_t *rb, uint8_t *data)
{
    if (rb->head == rb->tail) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % USB_RX_BUFFER_SIZE;
    return true;
}

bool RingBuffer_IsEmpty(RingBuffer_t *rb)
{
    return (rb->head == rb->tail);
}
