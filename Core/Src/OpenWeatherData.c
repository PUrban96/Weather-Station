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

char AirQualityIndex[2] = "5";
char CurrentWeatherPressure[5] = "0";
char CurrentWeatherHumidity[4] = "0";
char CurrentWeatherTemperature[6] = "0";
char CurrentWeatherRain1h[6] = "0";
char CurrentWeatherSunrise[12] = "00:00";
char CurrentWeatherSunset[12] = "00:00";
char CurrentWeatherWindSpeed[6] = "0";
char CurrentWeatherWindDegree[6] = "0";
char CurrentWeatherWindCloudness[4] = "0";
char CurrentWeatherWindVisibility[6] = "0";
char CurrentWeatherIcon[4] = "01d";
char CurrentWeatherActualDate[12] = "00:00";

char Forecast_Day1_TemperatureDay[6] = "0";
char Forecast_Day1_TemperatureNight[6] = "0";
char Forecast_Day1_Icon[4] = "01d";

char Forecast_Day2_TemperatureDay[6] = "0";
char Forecast_Day2_TemperatureNight[6] = "0";
char Forecast_Day2_Icon[4] = "01d";

char Forecast_Day3_TemperatureDay[6] = "0";
char Forecast_Day3_TemperatureNight[6] = "0";
char Forecast_Day3_Icon[4] = "01d";

static void GetWeatherDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType);
static void GetForecastDataFormJSON(const char *JSONWeatherData, const char *ParameterName, char *JSONParamaterValueOut, uint8_t DataType,
        uint8_t NextDayNumber, const char *ForecastHour);

void AirPollutionDataParse(const char *AirPollutionJSON)
{
    GetWeatherDataFormJSON(AirPollutionJSON, "aqi", AirQualityIndex, Int);
}

void CurrentWeatherDataParse(const char *CurrentWeatherJSON)
{
    GetWeatherDataFormJSON(CurrentWeatherJSON, "pressure", CurrentWeatherPressure, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "humidity", CurrentWeatherHumidity, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "temp", CurrentWeatherTemperature, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "1h", CurrentWeatherRain1h, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "sunset", CurrentWeatherSunset, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "sunrise", CurrentWeatherSunrise, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "speed", CurrentWeatherWindSpeed, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "deg", CurrentWeatherWindDegree, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "all", CurrentWeatherWindCloudness, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "visibility", CurrentWeatherWindVisibility, Int);
    GetWeatherDataFormJSON(CurrentWeatherJSON, "icon", CurrentWeatherIcon, String);
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

