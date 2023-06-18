/*
 * ESP8266_NTP.c
 *
 *  Created on: 23 kwi 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "ESP8266_NTP.h"
#include "SoftwareTimers.h"
#include "ESP8266.h"
#include "Common.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "DateAndTime.h"

static char ESP8266NTP_Server[] = "info.cyf-kr.edu.pl";
static char ESP8266NTP_Port[] = "123";
static char ESP8266NTP_Protocol[] = "UDP";

static Common_FlagState_e ESP8266NTP_TimeRequestFlag = FLAG_RESET;
static ESP8266NTP_TimeType_e ESP8266NTP_TimeType = NTP_DAYLIGHT_TIME;


static bool ESP8266NTP_CalculateDateAndTime(char *ResponseBuffer, ESP8266NTP_DateAndTime_s *DateAndTime);
static void ESP8266NTP_UnixTimeToDate(uint32_t UinxTimestamp, ESP8266NTP_DateAndTime_s *Date);
static void ESP8266NTP_UnixTimeToHour(uint32_t UinxTimestamp, ESP8266NTP_DateAndTime_s *Time);

void ESP8266NTP_PreparePacket(uint8_t *packetBuffer)
{
    Common_ArrayClean((char*) packetBuffer, ESP8266_NTP_PACKET_SIZE);

    packetBuffer[0] = 0b11100011;  // LI, Version, Mode
    packetBuffer[1] = 0;           // Stratum, or type of clock
    packetBuffer[2] = 6;           // Polling Interval
    packetBuffer[3] = 0xEC;        // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 0x49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 0x49;
    packetBuffer[15] = 0x52;
}

bool ESP8266_SetTime(char *ResponseBuffer)
{
    ESP8266NTP_DateAndTime_s NTPTime = { 0 };
    bool status = ESP8266NTP_CalculateDateAndTime(ResponseBuffer, &NTPTime);
    if(status == true)
    {
        DateTime_SetNewDate(NTPTime.Day, NTPTime.DayWeek, NTPTime.Month+1, NTPTime.Year-100);
        DateTime_SetNewTime(NTPTime.Hour, NTPTime.Minute, NTPTime.Second);
        ESP8266NTP_SetTimeRequestFlag(FLAG_RESET);
        return true;
    }
    else
    {
        return false;
    }

}

static bool ESP8266NTP_CalculateDateAndTime(char *ResponseBuffer, ESP8266NTP_DateAndTime_s *DateAndTime)
{
    bool status = false;

    char *StartParameters = strchr(ResponseBuffer, ':');
    if(StartParameters != NULL)
    {
        uint16_t highWord = StartParameters[41] << 8 | StartParameters[42];
        uint16_t lowWord = StartParameters[43] << 8 | StartParameters[44];

        uint64_t NTPTime = highWord << 16 | lowWord; //NTP time (seconds since Jan 1 1900)
        uint32_t UnixTime = NTPTime - 2208988800UL; //UNIX Time (seconds since Jan 1 1970)

        ESP8266NTP_UnixTimeToDate(UnixTime, DateAndTime);
        ESP8266NTP_UnixTimeToHour(UnixTime, DateAndTime);

        status = true;
    }
    else
    {
        status = false;
    }
    return status;
}

static void ESP8266NTP_UnixTimeToDate(uint32_t UinxTimestamp, ESP8266NTP_DateAndTime_s *Date)
{
    //UinxTimestamp += (DateAndTime_GetTimeTypeOffset() * 3600);
    struct tm ts;
    time_t default_time = UinxTimestamp;

    (void) localtime_r(&default_time, &ts);
    Date->Day = ts.tm_mday;
    Date->DayWeek = ts.tm_wday;
    Date->Month = ts.tm_mon;
    Date->Year = ts.tm_year;
}

static void ESP8266NTP_UnixTimeToHour(uint32_t UinxTimestamp, ESP8266NTP_DateAndTime_s *Time)
{
    //UinxTimestamp += (DateAndTime_GetTimeTypeOffset() * 3600);
    struct tm ts;
    time_t default_time = UinxTimestamp;

    (void) localtime_r(&default_time, &ts);
    Time->Hour = ts.tm_hour + ESP8266NTP_TimeType;
    Time->Minute = ts.tm_min;
    Time->Second = ts.tm_sec;
}

char* ESP8266NTP_GetServer(void)
{
    return ESP8266NTP_Server;
}

char* ESP8266NTP_GetPort(void)
{
    return ESP8266NTP_Port;
}

char* ESP8266NTP_GetProtocol(void)
{
    return ESP8266NTP_Protocol;
}

Common_FlagState_e ESP8266NTP_GetTimeRequestFlag(void)
{
    return ESP8266NTP_TimeRequestFlag;
}

void ESP8266NTP_SetTimeRequestFlag(Common_FlagState_e NewState)
{
    ESP8266NTP_TimeRequestFlag = NewState;
}

