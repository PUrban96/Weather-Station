/*
 * OpenWeatherData.c
 *
 *  Created on: 6 lis 2022
 *      Author: Pietia2
 */

#include <string.h>
#include "main.h"
#include "stm32g0xx_hal.h"
#include "JSONParseSimple.h"
#include "OpenWeatherData.h"
#include "stdio.h"
#include "DateAndTime.h"
#include <stdlib.h>
#include "Common.h"

enum
{
    NextDay1 = 1,
    NextDay2 = 2,
    NextDay3 = 3
};

/* Variable */
/* ******************************************************************************** */
static char AirQualityIndex[2] = "5";
static char CurrentWeatherPressure[5] = "0";
static char CurrentWeatherHumidity[4] = "0";
static char CurrentWeatherTemperature[6] = "0";
static char CurrentWeatherRain1h[6] = "0";
static char CurrentWeatherSunrise[12] = "00:00";
static char CurrentWeatherSunset[12] = "00:00";
static char CurrentWeatherWindSpeed[6] = "0";
static char CurrentWeatherWindDegree[6] = "0";
static char CurrentWeatherWindCloudness[4] = "0";
static char CurrentWeatherWindVisibility[6] = "0";
static char CurrentWeatherIcon[4] = "01d";
static char CurrentWeatherActualDate[12] = "00:00";

static char Forecast_Day1_TemperatureDay[6] = "0";
static char Forecast_Day1_TemperatureNight[6] = "0";
static char Forecast_Day1_Icon[4] = "01d";

static char Forecast_Day2_TemperatureDay[6] = "0";
static char Forecast_Day2_TemperatureNight[6] = "0";
static char Forecast_Day2_Icon[4] = "01d";

static char Forecast_Day3_TemperatureDay[6] = "0";
static char Forecast_Day3_TemperatureNight[6] = "0";
static char Forecast_Day3_Icon[4] = "01d";
/* ******************************************************************************** */

/* Function declaration */
/* ******************************************************************************** */
static void GetWeatherDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType);
static void GetForecastDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType,
        uint8_t NextDayNumber, const char *ForecastHour);
/* ******************************************************************************** */

void AirPollutionDataParse(const char *AirPollutionJSON)
{
    //Common_ArrayClean(AirQualityIndex, 2);
    GetWeatherDataFormJSON(AirPollutionJSON, "aqi", AirQualityIndex, Int);
}

void CurrentWeatherDataParse(const char *CurrentWeatherJSON)
{
    Common_ArrayClean(CurrentWeatherPressure, 5);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "pressure", CurrentWeatherPressure, Int);

    Common_ArrayClean(CurrentWeatherHumidity, 4);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "humidity", CurrentWeatherHumidity, Int);

    Common_ArrayClean(CurrentWeatherTemperature, 6);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "temp", CurrentWeatherTemperature, Int);

    //Common_ArrayClean(CurrentWeatherRain1h, 6);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "1h", CurrentWeatherRain1h, Int);

    Common_ArrayClean(CurrentWeatherSunset, 12);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "sunset", CurrentWeatherSunset, Int);

    Common_ArrayClean(CurrentWeatherSunrise, 12);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "sunrise", CurrentWeatherSunrise, Int);

    Common_ArrayClean(CurrentWeatherWindSpeed, 6);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "speed", CurrentWeatherWindSpeed, Int);

    Common_ArrayClean(CurrentWeatherWindDegree, 6);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "deg", CurrentWeatherWindDegree, Int);

    Common_ArrayClean(CurrentWeatherWindCloudness, 4);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "all", CurrentWeatherWindCloudness, Int);

    Common_ArrayClean(CurrentWeatherWindVisibility, 6);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "visibility", CurrentWeatherWindVisibility, Int);

    Common_ArrayClean(CurrentWeatherIcon, 4);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "icon", CurrentWeatherIcon, String);

    Common_ArrayClean(CurrentWeatherActualDate, 12);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "dt", CurrentWeatherActualDate, Int);
}

static void GetWeatherDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType)
{
    GetParametersValue(JSONWeatherData, ParameterName, JSONParamaterValueOut, DataType);
    if(JSONParamaterValueOut == NULL)
        memcpy(JSONParamaterValueOut, "0", sizeof(char));
}

void ForecastWeatherDataParse(const char *ForecastJSON)
{
    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day1_TemperatureDay, Int, NextDay1, "12:00:00");
    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day1_TemperatureNight, Int, NextDay1, "00:00:00");
    GetForecastDataFormJSON(ForecastJSON, "icon", Forecast_Day1_Icon, String, NextDay1, "12:00:00");

    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day2_TemperatureDay, Int, NextDay2, "12:00:00");
    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day2_TemperatureNight, Int, NextDay2, "00:00:00");
    GetForecastDataFormJSON(ForecastJSON, "icon", Forecast_Day2_Icon, String, NextDay2, "12:00:00");

    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day3_TemperatureDay, Int, NextDay3, "12:00:00");
    GetForecastDataFormJSON(ForecastJSON, "temp", Forecast_Day3_TemperatureNight, Int, NextDay3, "00:00:00");
    GetForecastDataFormJSON(ForecastJSON, "icon", Forecast_Day3_Icon, String, NextDay3, "12:00:00");
    //free(ForecastJSON);
}

static void GetForecastDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType,
        uint8_t NextDayNumber, const char *ForecastHour)
{
    GetParametersValueForecast(JSONWeatherData, ParameterName, JSONParamaterValueOut, DataType, NextDayNumber, ForecastHour);
    if(JSONParamaterValueOut == NULL)
        memcpy(JSONParamaterValueOut, "0", sizeof(char));
}

char* GetAirQualityIndex(void)
{
    return AirQualityIndex;
}

char* GetCurrentWeatherPressure()
{
    return CurrentWeatherPressure;
}

char* GetCurrentWeatherHumidity()
{
    return CurrentWeatherHumidity;
}

char* GetCurrentWeatherTemperature()
{
    return CurrentWeatherTemperature;
}

char* GetCurrentWeatherRain1h()
{
    return CurrentWeatherRain1h;
}

char* GetCurrentWeatherSunrise()
{
    return CurrentWeatherSunrise;
}

char* GetCurrentWeatherSunset()
{
    return CurrentWeatherSunset;
}

char* GetCurrentWeatherWindSpeed()
{
    return CurrentWeatherWindSpeed;
}

char* GetCurrentWeatherWindDegree()
{
    return CurrentWeatherWindDegree;
}

char* GetCurrentWeatherWindCloudness()
{
    return CurrentWeatherWindCloudness;
}

char* GetCurrentWeatherWindVisibility()
{
    return CurrentWeatherWindVisibility;
}

char* GetCurrentWeatherIcon()
{
    return CurrentWeatherIcon;
}

char* GetCurrentWeatherActualDate()
{
    return CurrentWeatherActualDate;
}

/*Forecast */
char* GetForecast_Day1_TemperatureDay()
{
    return Forecast_Day1_TemperatureDay;
}

char* GetForecast_Day1_TemperatureNight()
{
    return Forecast_Day1_TemperatureNight;
}

char* GetForecast_Day1_Icon()
{
    return Forecast_Day1_Icon;
}

char* GetForecast_Day2_TemperatureDay()
{
    return Forecast_Day2_TemperatureDay;
}

char* GetForecast_Day2_TemperatureNight()
{
    return Forecast_Day2_TemperatureNight;
}

char* GetForecast_Day2_Icon()
{
    return Forecast_Day2_Icon;
}

char* GetForecast_Day3_TemperatureDay()
{
    return Forecast_Day3_TemperatureDay;
}

char* GetForecast_Day3_TemperatureNight()
{
    return Forecast_Day3_TemperatureNight;
}

char* GetForecast_Day3_Icon()
{
    return Forecast_Day3_Icon;
}

