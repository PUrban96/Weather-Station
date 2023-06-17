/*
 * ESP8266.c
 *
 *  Created on: 27 paź 2022
 *      Author: Pietia2
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stm32g0xx_hal.h"
#include "ESP8266.h"
#include "OpenWeatherData.h"
#include "SoftwareTimers.h"
#include "LCDFrontend.h"
#include "Common.h"
#include "CircularBuffer.h"
#include "SDCardConfig.h"
#include "ESP8266_NTP.h"

/* TypeDef */
/* ******************************************************************************** */
typedef enum _ESP_GetWeatherDataSteps_e
{
    ModuleStart, ClientMode, ConnectNetwork, Cipmux,

    ConnectServer, CipsendAirPollution, GetData,

    ConnectServer2, CipsendCurrentWeather, GetDataCurrentWeather,

    ConnectServer3, CipsendForecast, GetDataForecast,

    ConnectServerNTP, CipsendNTP, GetDataNTP,

    WaitForTimer
} ESP_GetWeatherDataSteps_e;

typedef struct _ESP8266_StateMachineData_s
{
    SoftwareTimer *SWTimer;
    SoftwareTimer *StepErrorTimer;
    char ESPCommandReceiveBuffer[ESP8266_RECEIVE_BUFFER_SIZE];
    uint8_t StepErrorCounter;
    uint16_t TotalErrorCounter;
    ESP_GetWeatherDataSteps_e CurrentState;
    ESP_ReceiveDataState_e RxState;
} ESP8266_StateMachineData_s;
/* ******************************************************************************** */

/* Variable */
/* ******************************************************************************** */
static ESP8266_StateMachineData_s StateMachineData = { 0 };
static char Forecast[17000];
static ESP8266_DateFlag_e ESP8266_FirstDataSuccessFlag = ESP_DATEFLAG_WAIT;
static CircularBuffer_s ESP8266CommandBuffer = { 0 };
static char ESP8266_GetDataSendBuffer[200] = { 0 };
static Common_FlagState_e ESP8266_DebugEnableFlag = FLAG_RESET;

static uint16_t DEBUG_NTCConnectTry = 0;
/* ******************************************************************************** */

/* Variable */
/* ******************************************************************************** */
static char ESP866_OpenWeatherAddress[] = "192.241.167.16";
static char ESP866_OpenWeatherPort[] = "80";
static char ESP866_OpenWeatherProtocol[] = "TCP";
/* ******************************************************************************** */

/* Function pointer - parser callback */
/* ******************************************************************************** */
typedef void (*DataParser_funptr)(const char*);
/* ******************************************************************************** */

/* Function declaration */
/* ******************************************************************************** */
static ESP8266_StepStatus_e ESP826_ModuleStart(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_ClientMode(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_ConnectNetwork(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_Cipmux(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_ConnectServer(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_ConnectServerNTP(ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_GetData(char *InputData, char *DataBuffer, uint16_t DataBufferLen, DataParser_funptr ParserCallback,
        ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_GetDataNTP(char *DataBuffer, uint16_t DataBufferLen, ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_Cipsend(char *InputCipsend, ESP8266_StateMachineData_s *MachineState);
static ESP8266_StepStatus_e ESP8266_WaitForTimer(ESP8266_StateMachineData_s *MachineState);

static void ESP8266_ModuleNexStep(ESP8266_StateMachineData_s *MachineState);
static void ESP8266_TotalError(ESP8266_StateMachineData_s *MachineState);

static uint8_t ESP8266SendData(char *DataToSend, uint8_t *RxDataFlag);
static uint8_t ESP8266SendDataByte(uint8_t *DataToSend, uint16_t DataLenght, uint8_t *RxDataFlag);
static uint8_t ESP8266ReceiveData(char *ReceiveBuffer, uint16_t ByteNumberToReceive);
static void ESPReceiveBufferClean(char *Buffer, uint16_t BufferSize);
/* ******************************************************************************** */

void ESP8266_Init(void)
{
    HAL_StatusTypeDef status = 0;
    status = HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 5);
    status = status;
}

void ESP8266_MachineState(SoftwareTimer *SWTimer, SoftwareTimer *StepErrorTimer, SoftwareTimer *DebugTimer)
{
    StateMachineData.SWTimer = SWTimer;
    StateMachineData.StepErrorTimer = StepErrorTimer;
    static uint16_t CorrectCounter = 0;
    static uint16_t NTCConnectCorrect = 0;

    if(DebugTimer->TimerValue % 200 == 0 && ESP8266_DebugEnableFlag == FLAG_SET)
    {
        FrontEndDrawDebugInfo(StateMachineData.CurrentState, 0, 30);
        FrontEndDrawDebugInfo(StateMachineData.StepErrorCounter, 40, 30);
        FrontEndDrawDebugInfo(StateMachineData.TotalErrorCounter, 180, 30);
        FrontEndDrawDebugInfo(CorrectCounter, 210, 30);
        FrontEndDrawDebugInfo(DEBUG_NTCConnectTry, 180, 45);
        FrontEndDrawDebugInfo(NTCConnectCorrect, 210, 45);
    }

    switch(StateMachineData.CurrentState)
    {
    case ModuleStart:
        ESP826_ModuleStart(&StateMachineData);
        break;

    case ClientMode:
        ESP8266_ClientMode(&StateMachineData);
        break;

    case ConnectNetwork:
        ESP8266_ConnectNetwork(&StateMachineData);
        break;

    case Cipmux:
        ESP8266_Cipmux(&StateMachineData);
        break;

    case ConnectServer:
        ESP8266_ConnectServer(&StateMachineData);
        break;

    case CipsendAirPollution:
        ESP8266_Cipsend("AT+CIPSEND=120\r\n", &StateMachineData);
        break;

    case GetData:
        Common_ArrayClean(ESP8266_GetDataSendBuffer, 200);
        sprintf(ESP8266_GetDataSendBuffer, "GET http://api.openweathermap.org/data/2.5/air_pollution?lat=%s&lon=%s&appid=%s",
                SDCardConfig_GetLocalisationLATITUDE(), SDCardConfig_GetLocalisationLONGITUDE(), SDCardConfig_GetAPIKEY());
        ESP8266_GetData(ESP8266_GetDataSendBuffer, Forecast, 500, &AirPollutionDataParse, &StateMachineData);
        break;

    case ConnectServer2:
        ESP8266_ConnectServer(&StateMachineData);
        break;

    case CipsendCurrentWeather:
        ESP8266_Cipsend("AT+CIPSEND=115\r\n", &StateMachineData);
        break;

    case GetDataCurrentWeather:
        Common_ArrayClean(ESP8266_GetDataSendBuffer, 200);
        sprintf(ESP8266_GetDataSendBuffer, "GET https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric", SDCardConfig_GetLocalisationCITY(),
                SDCardConfig_GetAPIKEY());
        ESP8266_GetData(ESP8266_GetDataSendBuffer, Forecast, 1000, &CurrentWeatherDataParse, &StateMachineData);
        break;

    case ConnectServer3:
        ESP8266_ConnectServer(&StateMachineData);
        break;

    case CipsendForecast:
        ESP8266_Cipsend("AT+CIPSEND=116\r\n", &StateMachineData);
        break;

    case GetDataForecast:
    {
        Common_ArrayClean(ESP8266_GetDataSendBuffer, 200);
        sprintf(ESP8266_GetDataSendBuffer, "GET https://api.openweathermap.org/data/2.5/forecast?q=%s&appid=%s&units=metric",
                SDCardConfig_GetLocalisationCITY(), SDCardConfig_GetAPIKEY());
        ESP8266_StepStatus_e status = ESP8266_GetData(ESP8266_GetDataSendBuffer, Forecast, 17000, &ForecastWeatherDataParse, &StateMachineData);
        if(status == ESP_STEP_OK)
        {
            CorrectCounter++;
            if(ESP8266_FirstDataSuccessFlag == ESP_DATEFLAG_WAIT)
            {
                ESP8266_FirstDataSuccessFlag = ESP_DATEFLAG_READY;
            }

            if(ESP8266NTP_GetTimeRequestFlag() == FLAG_RESET)
            {
                HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
                HAL_UART_MspDeInit(&UART_HANDLER);
                StartAndResetTimer(StateMachineData.SWTimer);
                StateMachineData.CurrentState = WaitForTimer;
            }
        }
    }
        break;

    case ConnectServerNTP:
        ESP8266_ConnectServerNTP(&StateMachineData);
        break;

    case CipsendNTP:
        ESP8266_Cipsend("AT+CIPSEND=48\r\n", &StateMachineData);
        break;

    case GetDataNTP:
    {
        Common_ArrayClean(ESP8266_GetDataSendBuffer, 200);
        ESP8266_StepStatus_e status = ESP8266_GetDataNTP(Forecast, 50, &StateMachineData);
        if(status == ESP_STEP_OK)
        {
            NTCConnectCorrect++;
            HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
            HAL_UART_MspDeInit(&UART_HANDLER);
            StartAndResetTimer(StateMachineData.SWTimer);
            StateMachineData.CurrentState = WaitForTimer;
        }
    }
        break;

    case WaitForTimer:
        ESP8266_WaitForTimer(&StateMachineData);
        break;

    default:
        break;

    }
}

static ESP8266_StepStatus_e ESP826_ModuleStart(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        StopAndResetTimer(MachineState->SWTimer);
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 1);
        ESP8266SendData("AT\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "OK") || strstr(ESP8266CommandBuffer.Buffer, "WIFI") != 0)
        {
            ESP8266_ModuleNexStep(MachineState);
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
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 100);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 1);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_ClientMode(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        ESP8266SendData("AT+CWMODE=1\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "OK") != 0 || strstr(ESP8266CommandBuffer.Buffer, "no change") != 0)
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_ConnectNetwork(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;
    char SendBuffer[100] = { 0 };

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        sprintf(SendBuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", SDCardConfig_GetNetworkSSID(), SDCardConfig_GetNetworkPASS());
        ESP8266SendData(SendBuffer, &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        //HAL_Delay(100);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "OK") != 0)
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_Cipmux(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        ESP8266SendData("AT+CIPMUX=0\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "OK") != 0)
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_ConnectServer(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        HAL_UART_MspInit(&UART_HANDLER);
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
        ESP8266SendData("AT+CIPSTART=\"TCP\",\"192.241.167.16\",80\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "CONNECT") != 0 || strstr(ESP8266CommandBuffer.Buffer, "OK"))
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }
    return status;
}

static ESP8266_StepStatus_e ESP8266_ConnectServerNTP(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        HAL_UART_MspInit(&UART_HANDLER);
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
        ESP8266SendData("AT+CIPSTART=\"UDP\",\"info.cyf-kr.edu.pl\",123\r\n", &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "CONNECT") != 0 || strstr(ESP8266CommandBuffer.Buffer, "OK"))
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            MachineState->RxState = ESPReceiveIdle;
            (MachineState->StepErrorCounter)++;
            status = ESP_STEP_EXECUTE;

            if(MachineState->StepErrorCounter > 5)
            {
                ESP8266_TotalError(MachineState);
                status = ESP_STEP_ERR;
            }
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }
    return status;
}

static ESP8266_StepStatus_e ESP8266_GetData(char *InputData, char *DataBuffer, uint16_t DataBufferLen, DataParser_funptr ParserCallback,
        ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(DataBuffer, 17000);
        ESP8266SendData(InputData, &MachineState->RxState);
        //HAL_Delay(100);
        ESP8266SendData("\r\n", &MachineState->RxState);
        ESP8266ReceiveData(DataBuffer, DataBufferLen);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        ESP8266ReceiveData(DataBuffer, DataBufferLen);
        if(strstr(DataBuffer, "CLOSED") != 0)
        {
            ESP8266SendData("AT+CIPCLOSE\r\n", &MachineState->RxState);
            ParserCallback(DataBuffer);
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
            HAL_UART_MspDeInit(&UART_HANDLER);
            //HAL_Delay(1000);
        }
        else if(MachineState->StepErrorTimer->TimerValue > 3 * ESP8266_MAX_STEP_TIME)
        {
            ESP8266_TotalError(MachineState);
            status = ESP_STEP_ERR;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_GetDataNTP(char *DataBuffer, uint16_t DataBufferLen, ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;
    uint8_t NTP_Packet[ESP8266_NTP_PACKET_SIZE] = { 0 };

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
        ESP8266NTP_PreparePacket(NTP_Packet);
        ESP8266SendDataByte(NTP_Packet, ESP8266_NTP_PACKET_SIZE, &MachineState->RxState);
        ESP8266SendData("\r\n", &MachineState->RxState);

        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
        DEBUG_NTCConnectTry++;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, "IPD") != 0)
        {
            HAL_Delay(50);
            ESP8266SendData("AT+CIPCLOSE\r\n", &MachineState->RxState);
            ESP8266_SetTime(ESP8266CommandBuffer.Buffer);
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
            HAL_UART_MspDeInit(&UART_HANDLER);
            //HAL_Delay(1000);
        }
        else if(MachineState->StepErrorTimer->TimerValue > 3 * ESP8266_MAX_STEP_TIME)
        {
            ESP8266_TotalError(MachineState);
            status = ESP_STEP_ERR;
        }
    }

    return status;
}

static ESP8266_StepStatus_e ESP8266_Cipsend(char *InputCipsend, ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->RxState == ESPReceiveIdle)
    {
        ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
        CircularBuffer_FlushBuffer(&ESP8266CommandBuffer);
        ESP8266SendData(InputCipsend, &MachineState->RxState);
        //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
        StartAndResetTimer(MachineState->StepErrorTimer);
        status = ESP_STEP_EXECUTE;
    }

    if(MachineState->RxState == ESPWaitForData)
    {
        if(strstr(ESP8266CommandBuffer.Buffer, ">") != 0)
        {
            ESP8266_ModuleNexStep(MachineState);
            status = ESP_STEP_OK;
        }
        else if(MachineState->StepErrorTimer->TimerValue > ESP8266_MAX_STEP_TIME)
        {
            HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET);

            (MachineState->CurrentState)--;
            MachineState->RxState = ESPReceiveIdle;
            ESPReceiveBufferClean(MachineState->ESPCommandReceiveBuffer, ESP8266_RECEIVE_BUFFER_SIZE);
            status = ESP_STEP_ERR;
        }
        else
        {
            //ESP8266ReceiveData(MachineState->ESPCommandReceiveBuffer, 200);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 100);
            status = ESP_STEP_EXECUTE;
        }
    }
    return status;
}

static ESP8266_StepStatus_e ESP8266_WaitForTimer(ESP8266_StateMachineData_s *MachineState)
{
    ESP8266_StepStatus_e status = 0;

    if(MachineState->SWTimer->TimerValue >= (uint32_t) (1000 * 60 * ESP8266_GET_DATA_PERIOD))
    {
        MachineState->CurrentState = ModuleStart;
        HAL_UART_MspInit(&UART_HANDLER);
        HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_SET);
        StopAndResetTimer(MachineState->SWTimer);
        status = ESP_STEP_OK;
    }
    else
    {
        HAL_GPIO_WritePin(ESP_SW_GPIO_Port, ESP_SW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ESP_RST_GPIO_Port, ESP_RST_Pin, GPIO_PIN_RESET);
        status = ESP_STEP_EXECUTE;
    }

    return status;
}

static void ESP8266_ModuleNexStep(ESP8266_StateMachineData_s *MachineState)
{
    (MachineState->CurrentState)++;
    StopAndResetTimer(MachineState->StepErrorTimer);
    MachineState->StepErrorCounter = 0;
    MachineState->RxState = ESPReceiveIdle;
}

static void ESP8266_TotalError(ESP8266_StateMachineData_s *MachineState)
{
    MachineState->CurrentState = WaitForTimer;
    StartAndResetTimer(MachineState->SWTimer);
    (MachineState->TotalErrorCounter)++;
    MachineState->RxState = ESPReceiveIdle;
}

static uint8_t ESP8266SendData(char *DataToSend, uint8_t *RxDataFlag)
{
    uint8_t SendDataStatus = 0;
    //SendDataStatus = HAL_UART_Transmit_DMA(&UART_HANDLER, (uint8_t*) DataToSend, strlen(DataToSend));
    SendDataStatus = HAL_UART_Transmit(&UART_HANDLER, (uint8_t*) DataToSend, strlen(DataToSend), 100);
    *RxDataFlag = ESPWaitForData;
    return SendDataStatus;
}

static uint8_t ESP8266SendDataByte(uint8_t *DataToSend, uint16_t DataLenght, uint8_t *RxDataFlag)
{
    uint8_t SendDataStatus = 0;
    //SendDataStatus = HAL_UART_Transmit_DMA(&UART_HANDLER, (uint8_t*) DataToSend, strlen(DataToSend));
    SendDataStatus = HAL_UART_Transmit(&UART_HANDLER, DataToSend, DataLenght, 100);
    *RxDataFlag = ESPWaitForData;
    return SendDataStatus;
}

static uint8_t ESP8266ReceiveData(char *ReceiveBuffer, uint16_t ByteNumberToReceive)
{
    uint8_t ReceiveDataStatus = 0;
    //ESPReceiveBufferClean(ReceiveBuffer , ESP8266_RECEIVE_BUFFER_SIZE);
    ReceiveDataStatus = HAL_UARTEx_ReceiveToIdle_IT(&UART_HANDLER, (uint8_t*) ReceiveBuffer, ByteNumberToReceive);
    return ReceiveDataStatus;
}

static void ESPReceiveBufferClean(char *Buffer, uint16_t BufferSize)
{
    for(uint16_t i = 0; i < BufferSize; i++)
    {
        Buffer[i] = 'a';
    }

}

ESP8266_DateFlag_e ESP8266_GetFirstDataSuccessFlag(void)
{
    return ESP8266_FirstDataSuccessFlag;
}

void ESP8266_SetFirstDataSuccessFlag(ESP8266_DateFlag_e FlagState)
{
    ESP8266_FirstDataSuccessFlag = FlagState;
}

void ESP8266_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance == USART2)
    {
        if(StateMachineData.CurrentState != GetData && StateMachineData.CurrentState != GetDataCurrentWeather
                && StateMachineData.CurrentState != GetDataForecast)
        {
            CircularBuffer_AddToBuffer(&ESP8266CommandBuffer, StateMachineData.ESPCommandReceiveBuffer, Size);
            HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t*) StateMachineData.ESPCommandReceiveBuffer, 1);
        }
        else
        {
            //status =  HAL_UARTEx_ReceiveToIdle_DMA(&UART_HANDLER, (uint8_t *)StateMachineData.ESPCommandReceiveBuffer, 1);
            ;
        }
    }
}

void ESP8266_ToggleDebugFlag(void)
{
    if(ESP8266_DebugEnableFlag == FLAG_RESET)
    {
        ESP8266_DebugEnableFlag = FLAG_SET;
    }
    else if(ESP8266_DebugEnableFlag == FLAG_SET)
    {
        ESP8266_DebugEnableFlag = FLAG_RESET;
    }
}

