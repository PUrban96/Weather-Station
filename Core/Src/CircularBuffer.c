/*
 * CircularBuffer.c
 *
 *  Created on: 19 lut 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "Common.h"
#include "CircularBuffer.h"

static void CircularBuffer_CleanBuffer(CircularBuffer_s *Buffer, uint16_t BufferSize);

void CircularBuffer_AddToBuffer(CircularBuffer_s *Buffer, char *InputData, uint8_t DataLenght)
{
    uint16_t NextHead;
    if(Buffer->Overflow == 0)
    {
        for(int i = 0; i < DataLenght; i++)
        {
            NextHead = (Buffer->Head + 1);
            if(NextHead > (CIRCULAR_BUFFER_SIZE - 1))
            {
                NextHead = 0;
            }

            if(Buffer->Head > (CIRCULAR_BUFFER_SIZE - 1))
            {
                Buffer->Head = 0;
            }
            if(NextHead == Buffer->Tail)
            {
                Buffer->Overflow = 1;
            }
            Buffer->Buffer[Buffer->Head] = InputData[i];
            (Buffer->Head)++;
            if(Buffer->Head == Buffer->Tail)
            {
                break;
            }
        }
        (Buffer->Amount)++;
    }
}

void CircularBuffer_FlushBuffer(CircularBuffer_s *Buffer)
{
    Buffer->Head = 0;
    Buffer->Tail = 0;
    CircularBuffer_CleanBuffer(Buffer, CIRCULAR_BUFFER_SIZE);
}

static void CircularBuffer_CleanBuffer(CircularBuffer_s *Buffer, uint16_t BufferSize)
{
    for(uint16_t i = 0; i < BufferSize; i++)
    {
        Buffer->Buffer[i] = 'a';
    }

}
