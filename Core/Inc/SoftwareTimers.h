/*
 * SoftwareTimers.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_SOFTWARETIMERS_H_
#define INC_SOFTWARETIMERS_H_

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

void StartAndResetTimer(SoftwareTimer *SWTimer);
void StopAndResetTimer(SoftwareTimer *SWTimer);
void StartTimer(SoftwareTimer *SWTimer);
void StopTimer(SoftwareTimer *SWTimer);
void ResetTimer(SoftwareTimer *SWTimer);
void CheckActiveAndIncrementTimer(SoftwareTimer *SWTimer);

#endif /* INC_SOFTWARETIMERS_H_ */
