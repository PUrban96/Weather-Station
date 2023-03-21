/*
 * JSONParseSimple.c
 *
 *  Created on: 27 paź 2022
 *      Author: Pietia2
 */

#include <string.h>
#include "main.h"
#include "stm32g0xx_hal.h"
#include "JSONParseSimple.h"
#include "DateAndTime.h"
#include <stdio.h>

void GetParametersValue(const char *InputJSON, const char *JSONParameterNameIn, char *JSONParamaterValueOut, uint8_t DataType)
{
    char *ParameterPointer = NULL;
    char *StartParameterPointer;
    char *EndParameterPointer1;
    char *EndParameterPointer2;

    ParameterPointer = strstr(InputJSON, JSONParameterNameIn);
    if(ParameterPointer != NULL)
    {
        if(DataType == Int)
        {
            StartParameterPointer = strchr(ParameterPointer, ':') + sizeof(char);
            EndParameterPointer1 = strchr(StartParameterPointer, '}');
            EndParameterPointer2 = strchr(StartParameterPointer, ',');
        }
        else if(DataType == String)
        {
            StartParameterPointer = strchr(ParameterPointer, ':') + (2 * sizeof(char));
            EndParameterPointer1 = strchr(StartParameterPointer, 'n');
            EndParameterPointer2 = strchr(StartParameterPointer, 'd');
        }
        char *EndParameterPointer = NULL;

        if(EndParameterPointer1 == NULL)
        {
            EndParameterPointer = EndParameterPointer2;
        }
        else if(EndParameterPointer2 == NULL)
        {
            EndParameterPointer = EndParameterPointer1;
        }
        else if(EndParameterPointer1 != NULL && EndParameterPointer2 != NULL)
        {
            EndParameterPointer = (EndParameterPointer1 <= EndParameterPointer2) ? EndParameterPointer1 : EndParameterPointer2;
            if(DataType == String)
            {
                EndParameterPointer += sizeof(char);
            }
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

void GetParametersValueForecast(const char *InputJSON, const char *JSONParameterNameIn, char *JSONParamaterValueOut, uint8_t DataType, uint8_t NextDayNumber,
        const char *HourString)
{
    uint8_t NextDay = 0;
    uint8_t NextMonth = 0;
    uint16_t NextYear = 0;
    char DateSectionSearch[20];

    CalculateNextDayDate(NextDayNumber, &NextYear, &NextMonth, &NextDay);
    sprintf(DateSectionSearch, "%d-%02d-%02d %s", (int) NextYear, (int) NextMonth, (int) NextDay, HourString);

    char *ParameterPointer = NULL;
    char *StartParameterPointer;
    char *EndParameterPointer1;
    char *EndParameterPointer2;

    ParameterPointer = strstr(InputJSON, DateSectionSearch);
    ParameterPointer = strstr(ParameterPointer, JSONParameterNameIn);
    if(ParameterPointer != NULL)
    {
        if(DataType == Int)
        {
            StartParameterPointer = strchr(ParameterPointer, ':') + sizeof(char);
            EndParameterPointer1 = strchr(StartParameterPointer, '}');
            EndParameterPointer2 = strchr(StartParameterPointer, ',');
        }
        else if(DataType == String)
        {
            StartParameterPointer = strchr(ParameterPointer, ':') + (2 * sizeof(char));
            EndParameterPointer1 = strchr(StartParameterPointer, 'n');
            EndParameterPointer2 = strchr(StartParameterPointer, 'd');
        }
        char *EndParameterPointer = NULL;

        if(EndParameterPointer1 == NULL)
        {
            EndParameterPointer = EndParameterPointer2;
        }
        else if(EndParameterPointer2 == NULL)
        {
            EndParameterPointer = EndParameterPointer1;
        }
        else if(EndParameterPointer1 != NULL && EndParameterPointer2 != NULL)
        {
            EndParameterPointer = (EndParameterPointer1 <= EndParameterPointer2) ? EndParameterPointer1 : EndParameterPointer2;
            if(DataType == String)
            {
                EndParameterPointer += sizeof(char);
            }
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

