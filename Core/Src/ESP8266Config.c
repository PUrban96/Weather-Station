/*
 * ESP8266Config.c
 *
 *  Created on: Mar 11, 2023
 *      Author: Pietia2
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stm32g0xx_hal.h"
#include "ESP8266Config.h"
#include "Common.h"
#include "CircularBuffer.h"
#include "ESP8266.h"
#include "ESP8266ConfigPage.h"
#include "SoftwareTimers.h"
#include "SystemState.h"
#include "ESP8266ConfigParser.h"
#include "LCDFrontend.h"

/* TypeDef */
/* ******************************************************************************** */
typedef enum _ESP8266Config_MachineState_e
{
    CFG_ModuleStart,
    CFG_AccesPoint,
    CFG_SetIP,
    CFG_NetworkParam,
    CFG_MultiConnect,
    CFG_StartServer,
    CFG_LimitTime,
    CFG_WaitForConnect,
    CFG_SendServer,
    CFG_SendConfigPage,
    CFG_GetData,
    CFG_Return_ClientMode,
    CFG_Return_Reset
} ESP8266Config_MachineState_e;

typedef struct _ESP8266_StateMachineData_s
{
    SoftwareTimer *SWTimer;
    SoftwareTimer *StepErrorTimer;
    char ESPCommandReceiveBuffer[ESP8266CONFIG_RECEIVE_BUFFER_SIZE];
    uint8_t StepErrorCounter;
    uint16_t TotalErrorCounter;
    ESP8266Config_MachineState_e CurrentState;
    ESP_ReceiveDataState_e RxState;
} ESP8266Config_StateMachineData_s;
/* ******************************************************************************** */

/* Variable */
/* ******************************************************************************** */
static ESP8266Config_StateMachineData_s ESP8266Config_StateMachineData = { 0 };
static CircularBuffer_s ESP8266Config_CommandBuffer = { 0 };
/* ******************************************************************************** */

/* Config variable */
/* ******************************************************************************** */
static uint16_t ESP8266Config_ServerResponseTimeout = 350; /* in seconds */
static char ESP8266Config_IPAddress[] = "192.168.8.1";
static char ESP8266Config_WifiSSID[] = "WeatherStation";
static char ESP8266Config_WifiPassword[] = "WeatherConfig";
static uint8_t ESP8266Config_Channel = 5;
static ESP8266Config_PassType ESP8266Config_PasswordType = ESP_AP_WPA_WPA2_PSK;
/* ******************************************************************************** */

/* Function declaration */
/* ******************************************************************************** */
static ESP8266_StepStatus_e ESP8266Config_ModuleStart(ESP8266Config_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266Config_NextStateProcess(ESP8266Config_StateMachineData_s *MachineState, char *InputData);
static ESP8266_StepStatus_e ESP8266Config_WaitForConnect(ESP8266Config_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_Cipsend(ESP8266Config_StateMachineData_s *MachineState, char *InputCipsend);
static ESP8266_StepStatus_e ESP8266Config_SendConfigPage(ESP8266Config_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266Config_GetData(ESP8266Config_StateMachineData_s *MachineState);

static void ESP8266Config_ModuleNexStep(ESP8266Config_StateMachineData_s *MachineState);
static void ESP8266Config_TotalError(ESP8266Config_StateMachineData_s *MachineState);
static void ESP8266Config_ExitConfig(ESP8266Config_StateMachineData_s *MachineState);
static HAL_StatusTypeDef ESP8266Config_SendData(const char *DataToSend, uint8_t *RxDataFlag);
static void ESP8266Config_ReceiveBufferClean(char *Buffer, uint16_t BufferSize);
/* ******************************************************************************** */

void ESP8266Config_MachineState(SoftwareTimer *SWTimer, SoftwareTimer *StepErrorTimer, SoftwareTimer *DebugTimer)
{
    ESP8266Config_StateMachineData.SWTimer = SWTimer;
    ESP8266Config_StateMachineData.StepErrorTimer = StepErrorTimer;

    switch(ESP8266Config_StateMachineData.CurrentState)
    {
    case CFG_ModuleStart:
    {
        ESP8266Config_ModuleStart(&ESP8266Config_StateMachineData);
        //HAL_UART_Transmit(&UART_HANDLER, (uint8_t*) "TEST\r\n", strlen("TEST\r\n"), 100);
        break;
    }

    case CFG_AccesPoint:
    {
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, "AT+CWMODE=2\r\n");
        break;
    }

    case CFG_SetIP:
    {
        char DataToSend[50] = { 0 };
        sprintf(DataToSend, "AT+CIPAP=\"%s\"\r\n", ESP8266Config_IPAddress);
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, DataToSend);
        break;
    }

    case CFG_NetworkParam:
    {
        char DataToSend[50] = { 0 };
        sprintf(DataToSend, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ESP8266Config_WifiSSID, ESP8266Config_WifiPassword, ESP8266Config_Channel,
                ESP8266Config_PasswordType);
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, DataToSend);
        break;
    }

    case CFG_MultiConnect:
    {
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, "AT+CIPMUX=1\r\n");
        break;
    }

    case CFG_StartServer:
    {
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, "AT+CIPSERVER=1,80\r\n");
        break;
    }

    case CFG_LimitTime:
    {
        char DataToSend[50] = { 0 };
        sprintf(DataToSend, "AT+CIPSTO=%d\r\n", (int) ESP8266Config_ServerResponseTimeout);
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, DataToSend);
        break;
    }

    case CFG_WaitForConnect:
    {
        ESP8266Config_WaitForConnect(&ESP8266Config_StateMachineData);
        break;
    }

    case CFG_SendServer:
    {
        char DataToSend[50] = { 0 };
        sprintf(DataToSend, "AT+CIPSEND=0,%d\r\n", strlen(ESP8266ConfigPage));
        ESP8266_Cipsend(&ESP8266Config_StateMachineData, DataToSend);
        break;
    }

    case CFG_SendConfigPage:
    {
        ESP8266Config_SendConfigPage(&ESP8266Config_StateMachineData);
        break;
    }

    case CFG_GetData:
    {
        ESP8266Config_GetData(&ESP8266Config_StateMachineData);
        break;
    }

    case CFG_Return_ClientMode:
    {
        ESP8266Config_NextStateProcess(&ESP8266Config_StateMachineData, "AT+CWMODE=1\r\n");
        break;
    }

    case CFG_Return_Reset:
    {
        ESP8266Config_SendData("AT+RST\r\n", NULL);
        SystemState_SetState(SYSTEM_NORMAL);
        ESP8266Config_StateMachineData.CurrentState = CFG_ModuleStart;
        StartAndResetTimer(ESP8266Config_StateMachineData.SWTimer);
        ResetTimer(StepErrorTimer);
        LCDFrontend_ExternalRefresh();
        break;
    }
    }
    ESP8266Config_ExitConfig(&ESP8266Config_StateMachineData);
}

static ESP8266_StepStatus_e ESP8266Config_ModuleStart(ESP8266Config_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        HAL_UART_MspInit(&UART_HANDLER);
        ESP8266Config_ReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266CONFIG_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
        HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        StopAndResetTimer(MachineState->SWTimer);
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 1);
        ESP8266Config_SendData("AT\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266Config_CommandBuffer.Buffer, "OK") || strstr(ESP8266Config_CommandBuffer.Buffer, "WIFI") != 0)
        {
            ESP8266Config_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
            HAL_Delay(10);
            MachineState->RxState = ESPReceiveIdle;
            status = ESP_STEP_ERR;
        }
        else
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 1);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266Config_NextStateProcess(ESP8266Config_StateMachineData_s *MachineState, char *InputData)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESP8266Config_ReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266CONFIG_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
        ESP8266Config_SendData(InputData, &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266Config_CommandBuffer.Buffer, "OK") != 0 || strstr(ESP8266Config_CommandBuffer.Buffer, "no change") != 0)
        {
            ESP8266Config_ModuleNexStep(MachineState);
            CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266Config_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266Config_WaitForConnect(ESP8266Config_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(strstr(ESP8266Config_CommandBuffer.Buffer, "HTTP") != 0)
    {
        ESP8266Config_ModuleNexStep(MachineState);
        HAL_Delay(3000);
        status = ESP_STEP_OK;
    }
    else if(MachineState->StepErrorTimer->TimerValue > (uint32_t) (250 * 1000)) // wait max 250 second
    {
        MachineState->RxState = ESPReceiveIdle;
        (MachineState->StepErrorCounter)++;
        status = ESP_STEP_EXECUTE;
        ESP8266Config_TotalError(MachineState);
        status = ESP_STEP_ERR;
    }
    else
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 100);
        status = ESP_STEP_EXECUTE;
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_Cipsend(ESP8266Config_StateMachineData_s *MachineState, char *InputCipsend)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESP8266Config_ReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266CONFIG_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
        ESP8266Config_SendData(InputCipsend, &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266Config_CommandBuffer.Buffer, ">") != 0)
        {
            ESP8266Config_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            ESP8266Config_ReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266CONFIG_RECEIVE_BUFFER_SIZE);
            status = ESP_STEP_ERR;
        }
        else
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }
    return status;
}

static ESP8266_StepStatus_e ESP8266Config_SendConfigPage(ESP8266Config_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESP8266Config_ReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266CONFIG_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
        ESP8266Config_SendData(ESP8266ConfigPage, &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266Config_CommandBuffer.Buffer, "OK") != 0 || strstr(ESP8266Config_CommandBuffer.Buffer, "no change") != 0)
        {
            ESP8266Config_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            status = ESP_STEP_EXECUTE;
        }
        else
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266Config_GetData(ESP8266Config_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(strstr(ESP8266Config_CommandBuffer.Buffer, "Connection") != 0 || strstr(ESP8266Config_CommandBuffer.Buffer, "HTTP") != 0)
    {
        ESP8266ConfigParser_ParseData(ESP8266Config_CommandBuffer.Buffer);
        CircularBuffer_FlushBuffer(&ESP8266Config_CommandBuffer);
        status = ESP_STEP_OK;
    }
    else if(MachineState->StepErrorTimer->TimerValue > 100 * ESP8266_MAX_STEP_TIME) //TESTOnly
    {
        MachineState->RxState = ESPReceiveIdle;
        (MachineState->StepErrorCounter)++;
        status = ESP_STEP_EXECUTE;

    }
    else if(HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin) == GPIO_PIN_RESET)
    {
        (MachineState->CurrentState)++;
    }
    else
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) MachineState->ESPCommandReceiveBuffer, 100);
        status = ESP_STEP_EXECUTE;
    }

    return status;
}

static void ESP8266Config_ModuleNexStep(ESP8266Config_StateMachineData_s *MachineState)
{
    (MachineState->CurrentState)++;
    StopAndResetTimer(MachineState->StepErrorTimer);
    MachineState->StepErrorCounter = 0;
    MachineState->RxState = ESPReceiveIdle;
}

static void ESP8266Config_TotalError(ESP8266Config_StateMachineData_s *MachineState)
{
//MachineState->CurrentState = WaitForTimer;
    StartAndResetTimer(MachineState->SWTimer);
    (MachineState->TotalErrorCounter)++;
    MachineState->RxState = ESPReceiveIdle;
}

static void ESP8266Config_ExitConfig(ESP8266Config_StateMachineData_s *MachineState)
{
    if(HAL_GPIO_ReadPin(BUTTON_2_GPIO_Port, BUTTON_2_Pin) == GPIO_PIN_RESET)
    {
        (MachineState->CurrentState) = CFG_Return_Reset;
    }
}

static HAL_StatusTypeDef ESP8266Config_SendData(const char *DataToSend, uint8_t *RxDataFlag)
{
    HAL_StatusTypeDef SendDataStatus = 0;
//SendDataStatus = HAL_UART_Transmit_DMA(&UART_HANDLER, (uint8_t*) DataToSend, strlen(DataToSend));
    SendDataStatus = HAL_UART_Transmit(&UART_HANDLER, (uint8_t*) DataToSend, strlen(DataToSend), 100);
    if(RxDataFlag != NULL)
    {
        *RxDataFlag = ESPWaitForData;
    }
    return SendDataStatus;
}

static void ESP8266Config_ReceiveBufferClean(char *Buffer, uint16_t BufferSize)
{
    for(uint16_t i = 0; i < BufferSize; i++)
    {
        Buffer[i] = 'a';
    }

}

void ESP8266Config_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    HAL_StatusTypeDef status = 0;

    CircularBuffer_AddToBuffer(&ESP8266Config_CommandBuffer, ESP8266Config_StateMachineData.ESPCommandReceiveBuffer, Size);
    status = HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) ESP8266Config_StateMachineData.ESPCommandReceiveBuffer, 1);

    status = status;
}

char *ESP8266Config_GetIPAddress(void)
{
    return ESP8266Config_IPAddress;
}

char *ESP8266Config_GetSSID(void)
{
    return ESP8266Config_WifiSSID;
}
char *ESP8266Config_GetPassword(void)
{
    return ESP8266Config_WifiPassword;
}
