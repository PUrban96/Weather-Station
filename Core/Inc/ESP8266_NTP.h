/*
 * ESP8266_NTP.h
 *
 *  Created on: 23 kwi 2023
 *      Author: Pietia2
 */

#ifndef INC_ESP8266_NTP_H_
#define INC_ESP8266_NTP_H_

#include "main.h"
#include "stm32g0xx_hal.h"
#include "Common.h"

#define ESP8266_NTP_PACKET_SIZE 48

typedef struct _ESP8266NTP_DateAndTime_s
{
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
    uint8_t Day;
    uint8_t Month;
    uint8_t Year;
    uint8_t DayWeek;
} ESP8266NTP_DateAndTime_s;

void ESP8266NTP_PreparePacket(uint8_t *packetBuffer);
bool ESP8266_SetTime(char *ResponseBuffer);
char *ESP8266NTP_GetServer(void);
char *ESP8266NTP_GetPort(void);
char *ESP8266NTP_GetProtocol(void);
Common_FlagState_e ESP8266NTP_GetTimeRequestFlag(void);
void ESP8266NTP_SetTimeRequestFlag(Common_FlagState_e NewState);

#endif /* INC_ESP8266_NTP_H_ */
