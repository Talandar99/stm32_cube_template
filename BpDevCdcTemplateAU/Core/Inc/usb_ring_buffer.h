#ifndef INC_USB_RING_BUFFER_H_
#define INC_USB_RING_BUFFER_H_
/*
 * usb_ring_buffer.h
 *
 *  Created on: Jan 15, 2026
 *      Author:
 */

#include <stdint.h>
#include <stdbool.h>

#define USB_RX_BUFFER_SIZE 256

typedef struct {
    uint8_t buffer[USB_RX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t overflow;
} RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *rb);
bool RingBuffer_Put(RingBuffer_t *rb, uint8_t data);
bool RingBuffer_Get(RingBuffer_t *rb, uint8_t *data);
bool RingBuffer_IsEmpty(RingBuffer_t *rb);

#endif /* INC_USB_RING_BUFFER_H_ */
