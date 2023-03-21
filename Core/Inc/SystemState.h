/*
 * SystemState.h
 *
 *  Created on: Mar 11, 2023
 *      Author: Pietia2
 */

#ifndef INC_SYSTEMSTATE_H_
#define INC_SYSTEMSTATE_H_

#include "main.h"
#include "stm32g0xx_hal.h"

typedef enum _System_State_e
{
    SYSTEM_NORMAL,
    SYSTEM_CONFIG
}System_State_e;


System_State_e SystemState_GetState(void);
void SystemState_SetState(System_State_e NewState);

#endif /* INC_SYSTEMSTATE_H_ */
