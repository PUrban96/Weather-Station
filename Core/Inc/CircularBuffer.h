/*
 * CircularBuffer.h
 *
 *  Created on: 19 lut 2023
 *      Author: Pietia2
 */

#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include "main.h"
#include "stm32g0xx_hal.h"

#define CIRCULAR_BUFFER_SIZE    1100

typedef struct _CircularBuffer_s
{
        char Buffer[CIRCULAR_BUFFER_SIZE];
        uint8_t Overflow;
        uint16_t Head;
        uint16_t Tail;
        uint16_t Amount;

}CircularBuffer_s;

void CircularBuffer_AddToBuffer(CircularBuffer_s *Buffer, char *InputData, uint8_t DataLenght);
void CircularBuffer_FlushBuffer(CircularBuffer_s *Buffer);

#endif /* INC_CIRCULARBUFFER_H_ */
