/*
 * SoftwareTimers.c
 *
 *  Created on: 3 lis 2022
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"

void SoftwareTimer_Init(void)
{
    HAL_TIM_Base_Start_IT(&SOFTWARETIMER_HANDLER);
}

void StartAndResetTimer(SoftwareTimer *SWTimer)
{
    SWTimer->TimerActiveFlag = TimerActive;
    SWTimer->TimerValue = 0;
}

void StopAndResetTimer(SoftwareTimer *SWTimer)
{
    SWTimer->TimerActiveFlag = TimerInActive;
    SWTimer->TimerValue = 0;
}

void StartTimer(SoftwareTimer *SWTimer)
{
    SWTimer->TimerActiveFlag = TimerActive;
}

void StopTimer(SoftwareTimer *SWTimer)
{
    SWTimer->TimerActiveFlag = TimerInActive;
}

void ResetTimer(SoftwareTimer *SWTimer)
{
    SWTimer->TimerValue = 0;
}

void CheckActiveAndIncrementTimer(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerActiveFlag == TimerActive)
    {
        SWTimer->TimerValue++;
    }
}
