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

RTC_TimeTypeDef RtcTime;
RTC_DateTypeDef RtcDate;

char TimeString[10];
char DateString[15];
char DayString[12];

char Next_1_DayString[12];
char Next_2_DayString[12];
char Next_3_DayString[12];

static void NextDayString(char *NexDayString, uint8_t NextDayNumber);
static uint8_t CalculateLeapYear(void);
static uint8_t CalculateDayInMonth(void);
static void BackUpDate();
static void BackupDateToBR(void);
static uint32_t CalculateDayNumber(uint8_t Date, uint8_t Month, uint8_t Year);
static void CalculateDateFromDayNumber(uint32_t DayNumber, uint8_t *Date, uint8_t *Month, uint8_t *Year);

void ReadDateAndTime(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= RTC_READ_PERIOD)
    {
        HAL_RTC_GetTime(&RTC_HANDLER, &RtcTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTC_HANDLER, &RtcDate, RTC_FORMAT_BIN);
        BackUpDate();
        ResetTimer(SWTimer);
    }
}

void CheckActualDateAfterRestart(void)
{
    if(ESP8266_GetFirstDataSuccessFlag() == FLAG_SET)
    {
        uint8_t Day = 0;
        uint8_t DayWeek = 0;
        uint8_t Month = 0;
        uint8_t Year = 0;

        UnixTimeToDate(GetCurrentWeatherActualDate(), &Day, &DayWeek, &Month, &Year);
        Month += 1;
        Year = (1900 + Year) - 2000;
        DateTime_SetNewDate(Day, DayWeek, Month, Year);

         ESP8266_SetFirstDataSuccessFlag(FLAG_RESET);
    }
}

void DateTime_SetNewDate(uint8_t Day, uint8_t DayWeek, uint8_t Month, uint8_t Year)
{
    RTC_DateTypeDef NewRtcDate = {0};
    NewRtcDate.WeekDay = DayWeek;
    NewRtcDate.Month = Month ;
    NewRtcDate.Date = Day;
    NewRtcDate.Year = Year;
    HAL_RTC_SetDate(&RTC_HANDLER, &NewRtcDate, RTC_FORMAT_BIN);
}

void DateTime_SetNewTime(uint8_t Hour, uint8_t Minutes, uint8_t Seconds)
{
    RTC_TimeTypeDef sTime = {0};
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
    switch(RtcDate.WeekDay)
    {
    case 0:
        memcpy(DayString, "Sunday", strlen("Sunday"));
        break;
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
        CalculateDay = ((RtcDate.WeekDay + NextDayNumber) % 6)-1;
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

/* Save Date to Backup Reister */
static void BackUpDate()
{
    static uint8_t CompareDate = 0;
    if(RtcDate.Date != CompareDate)
    {
        BackupDateToBR();
        CompareDate = RtcDate.Date;
    }
}

static void BackupDateToBR(void)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, ((RtcDate.Date << 8) | (RtcDate.Month)));
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, ((RtcDate.Year << 8) | (RtcDate.WeekDay)));
}

/* Restore Date to Backup Reister */
void RestoreDateFromBackupRegister(void)
{
    RTC_DateTypeDef BackupDate;

    RtcDate.Date = (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2) >> 8);
    RtcDate.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    RtcDate.Year = (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3) >> 8);
    RtcDate.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);

    HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN); // There is also an internal date update based on HW RTC time elapsed!!
    HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &BackupDate, RTC_FORMAT_BIN); // Days elapsed since MCU power down

    if(BackupDate.Year == 0)
    {
        uint32_t BackupDateDays = CalculateDayNumber(BackupDate.Date, BackupDate.Month, BackupDate.Year);
        uint32_t RtcDateDays = CalculateDayNumber(RtcDate.Date, RtcDate.Month, RtcDate.Year);

        RtcDateDays += (BackupDateDays - CalculateDayNumber(1, 1, 0));

        CalculateDateFromDayNumber(RtcDateDays, &RtcDate.Date, &RtcDate.Month, &RtcDate.Year);

        HAL_RTC_SetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);
    }
}

static uint32_t CalculateDayNumber(uint8_t Date, uint8_t Month, uint8_t Year)
{
    // Format:
    // DD.MM.YY
    uint32_t _Year = Year + 20;
    Month = (Month + 9) % 12;
    _Year = _Year - (Month / 10);
    return ((365 * _Year) + (_Year / 4) - (_Year / 100) + (_Year / 400) + (((Month * 306) + 5) / 10) + (Date - 1));
}

static void CalculateDateFromDayNumber(uint32_t DayNumber, uint8_t *Date, uint8_t *Month, uint8_t *Year)
{
    uint32_t _Date, _Month, _Year;
    _Year = ((10000 * DayNumber) + 14780) / 3652425;
    int32_t ddd = DayNumber - ((365 * _Year) + (_Year / 4) - (_Year / 100) + (_Year / 400));
    if(ddd < 0)
    {
        _Year -= 1;
        ddd = DayNumber - ((365 * _Year) + (_Year / 4) - (_Year / 100) + (_Year / 400));
    }
    int32_t mi = ((100 * ddd) + 52) / 3060;
    _Month = (mi + 2) % 12 + 1;
    _Year = _Year + (mi + 2) / 12;
    _Date = ddd - ((mi * 306) + 5) / 10 + 1;
    *Date = _Date;
    *Month = _Month;
    *Year = _Year - 20;
}
