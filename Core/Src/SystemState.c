/*
 * SystemState.c
 *
 *  Created on: Mar 11, 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SystemState.h"

static System_State_e SystemActualState = SYSTEM_NORMAL;

System_State_e SystemState_GetState(void)
{
    return SystemActualState;
}

void SystemState_SetState(System_State_e NewState)
{
    SystemActualState = NewState;
}
