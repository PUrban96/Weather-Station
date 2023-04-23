/*
 * DateAndTime.c
 *
 *  Created on: Dec 2, 2022
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "DateAndTime.h"
#include "SoftwareTimers.h"
#include <stdio.h>
#include <string.h>
#include "Common.h"
#include "ESP8266.h"
#include "OpenWeatherData.h"

static RTC_TimeTypeDef RtcTime;
static RTC_DateTypeDef RtcDate;

static char TimeString[10];
static char DateString[15];
static char DayString[12];

static char Next_1_DayString[12];
static char Next_2_DayString[12];
static char Next_3_DayString[12];

static void NextDayString(char *NexDayString, uint8_t NextDayNumber);
static uint8_t CalculateLeapYear(void);
static uint8_t CalculateDayInMonth(void);

void ReadDateAndTime(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= RTC_READ_PERIOD)
    {
        HAL_RTC_GetTime(&RTC_HANDLER, &RtcTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTC_HANDLER, &RtcDate, RTC_FORMAT_BIN);
        //BackUpDate();
        ResetTimer(SWTimer);
    }
}

void CheckActualDateAfterRestart(void)
{
    if(ESP8266_GetFirstDataSuccessFlag() == ESP_DATEFLAG_READY)
    {
        uint8_t Day = 0;
        uint8_t DayWeek = 0;
        uint8_t Month = 0;
        uint8_t Year = 0;

        UnixTimeToDate(GetCurrentWeatherActualDate(), &Day, &DayWeek, &Month, &Year);
        Month += 1;
        Year = (1900 + Year) - 2000;
        DateTime_SetNewDate(Day, DayWeek, Month, Year);

        ESP8266_SetFirstDataSuccessFlag(ESP_DATEFLAG_SET);
    }
}

void DateTime_SetNewDate(uint8_t Day, uint8_t DayWeek, uint8_t Month, uint8_t Year)
{
    RTC_DateTypeDef NewRtcDate = { 0 };
    NewRtcDate.WeekDay = DayWeek;
    NewRtcDate.Month = Month;
    NewRtcDate.Date = Day;
    NewRtcDate.Year = Year;
    HAL_RTC_SetDate(&RTC_HANDLER, &NewRtcDate, RTC_FORMAT_BIN);
}

void DateTime_SetNewTime(uint8_t Hour, uint8_t Minutes, uint8_t Seconds)
{
    RTC_TimeTypeDef sTime = { 0 };
    sTime.Hours = Hour;
    sTime.Minutes = Minutes;
    sTime.Seconds = Seconds;
    sTime.SubSeconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&RTC_HANDLER, &sTime, RTC_FORMAT_BIN);
}

char* GetTimeString(void)
{
    sprintf(TimeString, "%02d:%02d", RtcTime.Hours, RtcTime.Minutes);
    return TimeString;
}

char* GetDateString(void)
{
    sprintf(DateString, "%02d.%02d.20%d", RtcDate.Date, RtcDate.Month, RtcDate.Year);
    return DateString;
}

uint8_t GetRTCDateDay(void)
{
    return RtcDate.Date;
}

uint8_t GetRTCDateMonth(void)
{
    return RtcDate.Month;
}

uint8_t GetRTCDateYear(void)
{
    return RtcDate.Year;
}

char* GetDayString(void)
{
    Common_ArrayClean(DayString, 12);

    switch(RtcDate.WeekDay)
    {
    case 1:
        memcpy(DayString, "Monday", strlen("Monday"));
        break;
    case 2:
        memcpy(DayString, "Tuesday", strlen("Tuesday"));
        break;
    case 3:
        memcpy(DayString, "Wednesday", strlen("Wednesday"));
        break;
    case 4:
        memcpy(DayString, "Thursday", strlen("Thursday"));
        break;
    case 5:
        memcpy(DayString, "Friday", strlen("Friday"));
        break;
    case 6:
        memcpy(DayString, "Saturday", strlen("Saturday"));
        break;
    case 7:
        memcpy(DayString, "Sunday", strlen("Sunday"));
        break;

    default:
        memcpy(DayString, "Error", strlen("Error"));
        break;
    }
    return DayString;
}

char* GetNext_1_DayString(void)
{
    NextDayString(Next_1_DayString, 1);
    return Next_1_DayString;
}

char* GetNext_2_DayString(void)
{
    NextDayString(Next_2_DayString, 2);
    return Next_2_DayString;
}

char* GetNext_3_DayString(void)
{
    NextDayString(Next_3_DayString, 3);
    return Next_3_DayString;
}

static void NextDayString(char *NexDayString, uint8_t NextDayNumber)
{
    uint8_t CalculateDay = 0;

    if(RtcDate.WeekDay + NextDayNumber <= 6)
    {
        CalculateDay = RtcDate.WeekDay + NextDayNumber;
    }
    else if(RtcDate.WeekDay + NextDayNumber > 6)
    {
        CalculateDay = ((RtcDate.WeekDay + NextDayNumber) % 6) - 1;
    }

    switch(CalculateDay)
    {
    case 0:
        memcpy(NexDayString, "SUN", strlen("SUN"));
        break;
    case 1:
        memcpy(NexDayString, "MON", strlen("MON"));
        break;
    case 2:
        memcpy(NexDayString, "TUE", strlen("TUE"));
        break;
    case 3:
        memcpy(NexDayString, "WED", strlen("WED"));
        break;
    case 4:
        memcpy(NexDayString, "THU", strlen("THU"));
        break;
    case 5:
        memcpy(NexDayString, "FRI", strlen("FRI"));
        break;
    case 6:
        memcpy(NexDayString, "SAT", strlen("SAT"));
        break;

    default:
        memcpy(NexDayString, "ERR", strlen("ERR"));
        break;
    }
    //return DayString;
}

uint8_t GetActualMinutes(void)
{
    return RtcTime.Minutes;
}

void CalculateNextDayDate(uint8_t NextDayAmount, uint16_t *Year, uint8_t *Month, uint8_t *Day)
{
    if(RtcDate.Date + NextDayAmount <= CalculateDayInMonth())
    {
        *Year = 2000 + RtcDate.Year;
        *Month = RtcDate.Month;
        *Day = RtcDate.Date + NextDayAmount;
    }
    else if(RtcDate.Date + NextDayAmount > CalculateDayInMonth() && RtcDate.Month < 12)
    {
        *Year = 2000 + RtcDate.Year;
        *Month = RtcDate.Month + 1;
        *Day = (RtcDate.Date + NextDayAmount) % CalculateDayInMonth();
    }
    else if(RtcDate.Date + NextDayAmount > CalculateDayInMonth() && RtcDate.Month >= 12)
    {
        *Year = 2000 + RtcDate.Year + 1;
        *Month = 1;
        *Day = (RtcDate.Date + NextDayAmount) % CalculateDayInMonth();
    }
}

static uint8_t CalculateDayInMonth(void)
{
    uint8_t DayInMonth = 0;
    switch(RtcDate.Month)
    {
    case 1:
        DayInMonth = 31;
        break;

    case 2:
        if(CalculateLeapYear() == 1)
            DayInMonth = 29;
        else
            DayInMonth = 28;
        break;

    case 3:
        DayInMonth = 31;
        break;

    case 4:
        DayInMonth = 30;
        break;

    case 5:
        DayInMonth = 31;
        break;

    case 6:
        DayInMonth = 30;
        break;

    case 7:
        DayInMonth = 31;
        break;

    case 8:
        DayInMonth = 31;
        break;

    case 9:
        DayInMonth = 30;
        break;

    case 10:
        DayInMonth = 31;
        break;

    case 11:
        DayInMonth = 30;
        break;

    case 12:
        DayInMonth = 31;
        break;
    }

    return DayInMonth;
}

static uint8_t CalculateLeapYear(void)
{
    uint8_t Year = 2000 + RtcDate.Year;
    uint8_t LeapYearFlag = 0;
    if(Year % 4 == 0)
    {
        if(Year % 100 == 0)
        {
            if(Year % 400 == 0)
            {
                LeapYearFlag = 1;
            }
            else
            {
                LeapYearFlag = 0;
            }
        }
        else
        {
            LeapYearFlag = 1;
        }
    }
    else
    {
        LeapYearFlag = 0;
    }

    return LeapYearFlag;
}

uint8_t DateAndTime_CalculateDayInWeek(int Date, int Month, int Year)
{
    //Zeller's formula
    uint32_t YearWithCorrect = (Month < 3) ? (Year - 1) : Year;
    uint32_t Correction = (Month < 3) ? 0 : 2;
    int A = (23.0 * Month / 9.0);
    int B = (YearWithCorrect / 4.0);
    int C = (YearWithCorrect / 100.0);
    int D = (YearWithCorrect / 400.0);
    uint32_t DayNumberZeller = (A + Date + 4.0 + Year + B + C + D - Correction);
    DayNumberZeller = DayNumberZeller % 7;

    uint8_t SystemDayNumber = 0;

    switch(DayNumberZeller)
    {
    case 0:
        SystemDayNumber = 2;
        break;

    case 1:
        SystemDayNumber = 3;
        break;

    case 2:
        SystemDayNumber = 4;
        break;

    case 3:
        SystemDayNumber = 5;
        break;

    case 4:
        SystemDayNumber = 6;
        break;

    case 5:
        SystemDayNumber = 0;
        break;

    case 6:
        SystemDayNumber = 1;
        break;
    }

    return SystemDayNumber;
}
