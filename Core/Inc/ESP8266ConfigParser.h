/*
 * ESP8266ConfigParser.h
 *
 *  Created on: 12 mar 2023
 *      Author: Pietia2
 */

#ifndef INC_ESP8266CONFIGPARSER_H_
#define INC_ESP8266CONFIGPARSER_H_

#include "main.h"
#include "stm32g0xx_hal.h"

#define ESP8266CONFIGPARSER_SSID_NAME           "SSID"
#define ESP8266CONFIGPARSER_PASSWORD_NAME       "PASSWORD"
#define ESP8266CONFIGPARSER_APIKEY_NAME         "APIKEY"
#define ESP8266CONFIGPARSER_CITY_NAME           "CITY"
#define ESP8266CONFIGPARSER_LATITUDE_NAME       "LATITUDE"
#define ESP8266CONFIGPARSER_LONGITUDE_NAME      "LONGITUDE"
#define ESP8266CONFIGPARSER_DATE_NAME           "DATE"

typedef enum _ESP8266ConfigParser_DataType_e
{
    CONFIG_NODATA,
    CONFIG_NETWORK_PARAM,
    CONFIG_APIKEY,
    CONFIG_LOCALISATION,
    CONFIG_DATE
}ESP8266ConfigParser_DataType_e;

void ESP8266ConfigParser_ParseData(const char *InputData);


#endif /* INC_ESP8266CONFIGPARSER_H_ */
