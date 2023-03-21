/*
 * ESP8266ConfigParser.c
 *
 *  Created on: 12 mar 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "ESP8266ConfigParser.h"
#include <stdio.h>
#include <string.h>
#include "Common.h"
#include "DateAndTime.h"
#include "SDCardConfig.h"
#include <stdlib.h>

static char ESP8266Config_SSID[30] = { 0 };
static char ESP8266Config_PASS[30] = { 0 };
static char ESP8266Config_APIKEY[40] = { 0 };
static char ESP8266Config_CITY[30] = { 0 };
static char ESP8266Config_LATITUDE[30] = { 0 };
static char ESP8266Config_LONGITUDE[30] = { 0 };
static char ESP8266Config_DATE[30] = { 0 };

static void ESP8266ConfigParser_GetParametersValue(const char *InputData, const char *JSONParameterNameIn, char *JSONParamaterValueOut);
static void ESP8266ConfigParser_SetDateAndTime(char *InputDate);


void ESP8266ConfigParser_ParseData(const char *InputData)
{

    if(strstr(InputData, "SSID") != NULL && strstr(InputData, "PASSWORD") != NULL)
    {
        Common_ArrayClean(ESP8266Config_SSID, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "SSID", ESP8266Config_SSID);

        Common_ArrayClean(ESP8266Config_PASS, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "PASSWORD", ESP8266Config_PASS);

        SDCardConfig_WriteNetwork(ESP8266Config_SSID, ESP8266Config_PASS);
    }


    if(strstr(InputData, "APIKEY") != NULL)
    {
        Common_ArrayClean(ESP8266Config_APIKEY, 40);
        ESP8266ConfigParser_GetParametersValue(InputData, "APIKEY", ESP8266Config_APIKEY);

        SDCardConfig_WriteApikey(ESP8266Config_APIKEY);
    }

    if(strstr(InputData, "CITY") != NULL && strstr(InputData, "LATITUDE") != NULL && strstr(InputData, "LONGITUDE") != NULL)
    {
        Common_ArrayClean(ESP8266Config_CITY, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "CITY", ESP8266Config_CITY);

        Common_ArrayClean(ESP8266Config_LATITUDE, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "LATITUDE", ESP8266Config_LATITUDE);

        Common_ArrayClean(ESP8266Config_LONGITUDE, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "LONGITUDE", ESP8266Config_LONGITUDE);

        SDCardConfig_WriteLocalisation(ESP8266Config_CITY, ESP8266Config_LATITUDE, ESP8266Config_LONGITUDE);
    }

    if(strstr(InputData, "DATE") != NULL)
    {
        Common_ArrayClean(ESP8266Config_DATE, 30);
        ESP8266ConfigParser_GetParametersValue(InputData, "DATE", ESP8266Config_DATE);
        ESP8266ConfigParser_SetDateAndTime(ESP8266Config_DATE);
    }

}

static void ESP8266ConfigParser_SetDateAndTime(char *InputDate)
{
    uint8_t Hour = 0;
    uint8_t Minutes = 0;

    uint8_t Day = 0;
    uint8_t DayWeek = 0; //Wzór zellera
    uint8_t Month = 0;
    uint8_t Year = 0;

    Year = atoi(InputDate + 2*sizeof(char));
    Month = atoi(InputDate + 5*sizeof(char));
    Day = atoi(InputDate + 8*sizeof(char));

    Hour = atoi(InputDate + 11*sizeof(char));
    Minutes = atoi(InputDate + 16*sizeof(char));
    DateTime_SetNewTime(Hour, Minutes, 0);
    DateTime_SetNewDate(Day, DayWeek, Month, Year);

}

static void ESP8266ConfigParser_GetParametersValue(const char *InputData, const char *JSONParameterNameIn, char *JSONParamaterValueOut)
{
    char *ParameterPointer = NULL;
    char *StartParameterPointer;
    char *EndParameterPointer1;
    char *EndParameterPointer2;

    ParameterPointer = strstr(InputData, JSONParameterNameIn);
    if(ParameterPointer != NULL)
    {

        StartParameterPointer = strchr(ParameterPointer, '=') + sizeof(char);
        EndParameterPointer1 = strchr(StartParameterPointer, '&');
        EndParameterPointer2 = strchr(StartParameterPointer, 'H');

        char *EndParameterPointer = NULL;

        if(EndParameterPointer1 == NULL)
        {
            EndParameterPointer = (EndParameterPointer2 - sizeof(char));
        }
        else if(EndParameterPointer2 == NULL)
        {
            EndParameterPointer = EndParameterPointer1;
        }
        else if(EndParameterPointer1 != NULL && EndParameterPointer2 != NULL)
        {
            EndParameterPointer = (EndParameterPointer1 <= EndParameterPointer2) ? EndParameterPointer1 : EndParameterPointer2;
        }
        else
        {
            JSONParamaterValueOut = NULL;
            return;
        }

        uint8_t ParameterLen = EndParameterPointer - StartParameterPointer;
        memcpy(JSONParamaterValueOut, StartParameterPointer, ParameterLen);
    }
}
