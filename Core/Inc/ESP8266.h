/*
 * ESP8266.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

#include <string.h>
#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"
#include "Common.h"

/* Inside sensor config */
/* ******************************************************************************** */
#define UART_HANDLER huart2
extern UART_HandleTypeDef UART_HANDLER;
#define UART_INSTANCE USART2

#define ESP8266_RECEIVE_BUFFER_SIZE 200

//#define INSIDE_SENSOR_PERIOD 500 /* in milisecond */
#define ESP8266_MAX_STEP_TIME 2500

#define ESP8266_GET_DATA_PERIOD 10U /* in minute */
/* ******************************************************************************** */

/* ******************************************************************************** */
typedef enum _ESP8266_StepStatus_e
{
    ESP_STEP_ERR = 0, ESP_STEP_EXECUTE = 1, ESP_STEP_OK = 2
} ESP8266_StepStatus_e;

typedef enum _ESP_ReceiveDataState_e
{
    ESPReceiveIdle, ESPWaitForData
} ESP_ReceiveDataState_e;

typedef enum _ESP8266_DateFlag_e
{
    ESP_DATEFLAG_WAIT = 0,
    ESP_DATEFLAG_READY,
    ESP_DATEFLAG_SET
}ESP8266_DateFlag_e;
/* ******************************************************************************** */

void ESP8266_Init(void);
void ESP8266_MachineState(SoftwareTimer *SWTimer, SoftwareTimer *StepErrorTimer, SoftwareTimer *DebugTimer);
void ESP8266_SetFirstDataSuccessFlag(ESP8266_DateFlag_e FlagState);
ESP8266_DateFlag_e ESP8266_GetFirstDataSuccessFlag(void);
void ESP8266_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void ESP8266_ToggleDebugFlag(void);

#endif /* INC_ESP8266_H_ */
