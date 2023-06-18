/*
 * Common.c
 *
 *  Created on: Feb 16, 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"
#include <time.h>
#include <stdlib.h>
#include "DateAndTime.h"

void UnixTimeToHours(char *UinxTimestamp, char *OutputTime)
{
    uint32_t TimeStamp = atoi(UinxTimestamp);
    TimeStamp += (DateAndTime_GetTimeTypeOffset() * 3600);
    struct tm ts;
    time_t default_time = TimeStamp;

    (void) localtime_r(&default_time, &ts);
    strftime(OutputTime, 6 * sizeof(char), "%H:%M", &ts);
    ;
}

void UnixTimeToDate(char *UinxTimestamp, uint8_t *Day, uint8_t *DayWeek, uint8_t *Month, uint8_t *Year)
{
    uint32_t TimeStamp = atoi(UinxTimestamp);
    TimeStamp += (DateAndTime_GetTimeTypeOffset() * 3600);
    struct tm ts;
    time_t default_time = TimeStamp;

    (void) localtime_r(&default_time, &ts);
    *Day = ts.tm_mday;
    *DayWeek = ts.tm_wday;
    *Month = ts.tm_mon;
    *Year = ts.tm_year;
}

void Common_ArrayClean(char *Buffer, uint16_t BufferSize)
{
    for(uint16_t i = 0; i < BufferSize; i++)
    {
        Buffer[i] = 0;
    }

}
