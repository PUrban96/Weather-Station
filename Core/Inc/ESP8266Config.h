/*
 * ESP8266Config.h
 *
 *  Created on: Mar 11, 2023
 *      Author: Pietia2
 */

#ifndef INC_ESP8266CONFIG_H_
#define INC_ESP8266CONFIG_H_

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"
#include "Common.h"

typedef enum _ESP8266Config_PassType
{
    ESP_AP_OPEN = 0,
    ESP_AP_WEP = 1,
    ESP_AP_WPA_PSK = 2,
    ESP_AP_WPA2_PSK = 3,
    ESP_AP_WPA_WPA2_PSK = 4
}ESP8266Config_PassType;

#define ESP8266CONFIG_RECEIVE_BUFFER_SIZE 500

void ESP8266Config_MachineState(SoftwareTimer *SWTimer, SoftwareTimer *StepErrorTimer, SoftwareTimer *DebugTimer);
void ESP8266Config_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

char *ESP8266Config_GetIPAddress(void);
char *ESP8266Config_GetSSID(void);
char *ESP8266Config_GetPassword(void);

#endif /* INC_ESP8266CONFIG_H_ */
