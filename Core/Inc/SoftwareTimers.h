/*
 * SoftwareTimers.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_SOFTWARETIMERS_H_
#define INC_SOFTWARETIMERS_H_

#include "main.h"
#include "stm32g0xx_hal.h"

/* Software timers - hardware handler */
/* ******************************************************************************** */
#define SOFTWARETIMER_HANDLER htim6
extern TIM_HandleTypeDef SOFTWARETIMER_HANDLER;
/* ******************************************************************************** */

typedef struct
{
    uint8_t TimerActiveFlag;
    uint32_t TimerValue;
}SoftwareTimer;

enum TimerStatus
{
    TimerInActive,
    TimerActive
};

void SoftwareTimer_Init(void);
void StartAndResetTimer(SoftwareTimer *SWTimer);
void StopAndResetTimer(SoftwareTimer *SWTimer);
void StartTimer(SoftwareTimer *SWTimer);
void StopTimer(SoftwareTimer *SWTimer);
void ResetTimer(SoftwareTimer *SWTimer);
void CheckActiveAndIncrementTimer(SoftwareTimer *SWTimer);

#endif /* INC_SOFTWARETIMERS_H_ */
