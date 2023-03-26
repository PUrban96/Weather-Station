/*
 * LCDFrontend.c
 *
 *  Created on: 20 paź 2022
 *      Author: Pietia2
 */

#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include <stdlib.h>
#include "WeatherIcon.h"
#include "BMXX80.h"
#include "InsideSensor.h"
#include "SoftwareTimers.h"
#include "LCDFrontend.h"
#include "OpenWeatherData.h"
#include "string.h"
#include <stdio.h>
#include "DateAndTime.h"
#include "SDCardIcon.h"
#include "Common.h"
#include "ESP8266Config.h"

/* Function declaration */
/* ******************************************************************************** */
static void FrontendDrawDayName(char *DayName);
static void FrontendDrawDate(char *Date);
static void FrontendDrawTime(char *Time);

static void FrontendDrawOutsideWeatherSectionName(char *SectionName);
static void FrontendDrawOutsideRain(char *RainLevel);
static void FrontendDrawOutsideTemperature(char *Temperature);
static void FrontendDrawOutsidePressure(char *Pressure);
static void FrontendDrawOutsideHumidity(char *Humidity);
static void FrontendDrawOutsideWeatherIcon(char *WeatherIcon);

static void DrontendDrawInsideSectionName(char *SectionName);
static void FrontendDrawInsideTemperature(char *Temperature);
static void FrontendDrawInsideHumidity(char *Humidity);
static void FrontendDrawInsidePresure(char *Presure);

static void FrontendDrwaFirstDayDayName(char *DayName);
static void FrontendDrawFirstDayWeatherIcon(char *WeatherIcon);
static void FrontendDrawFirstDayTemperatureDay(char *Temperature);
static void FrontendDrawFirstDayTemperatureNight(char *Temperature);

static void FrontendDrawSecondDayDayName(char *DayName);
static void FrontendDrawFSecondDayWeatherIcon(char *WeatherIcon);
static void FrontendDrawSecondDayTemperatureDay(char *Temperature);
static void FrontendDrawSecondDayTemperatureNight(char *Temperature);

static void FrontendDrawThirdDayWeatherIcon(char *WeatherIcon);
static void FrontendDrawThirdDayDayName(char *DayName);
static void FrontendDrawThirdDayTemperatureDay(char *Temperature);
static void FrontendDrawThirdDayTemperatureNight(char *Temperature);

static void FrontendDrawWindDirection(char *WindDirecion);
static void FrontendDrawWindSpeed(char *WindSpeed);

static void FrontendDrawSunInfoSectionName(char *SunSectionName);
static void FrontendDrawSunriseTime(char *SunriseUnixTime);
static void FrontendDrawSunsetTime(char *SunsetUnixTime);

static void FrontendDrawAdditionalInfoSectionName(char *AdditionalSectionName);
static void FrontendDrawAdditionalInfoCloudness(char *Cloudness);

static void FrontendDrawVisibilitySectionName(char *AdditionalSectionName);
static void FrontendDrawAdditionalInfoVisibility(char *Visibility);

static void FrontendDrawAirPollutionSectionName(char *SectionName);
static void FrontendDrawAirPollution(char *AirPolutionIndex);

static void FrontendDrawLine(void);

static void FrontendDrawWeatherIcon(char *WeatherIcon, uint16_t X1, uint16_t Y1);
static void TemperatureWithoutFractionPart(const char *TemmperatureIn, char *TemperatureOut);
/* ******************************************************************************** */

/* Variable */
/* ******************************************************************************** */
static Common_FlagState_e LCDFrontendConfig_ReflashFlag = FLAG_RESET;
static Common_FlagState_e LCDFrontend_ExtarnalInterfaceRefresh = FLAG_RESET;
/* ******************************************************************************** */

void LCDFrontend_Init(void)
{
    //HAL_GPIO_WritePin(LCD_BRIGHTNESS_GPIO_Port, LCD_BRIGHTNESS_Pin, GPIO_PIN_SET);
    ILI9341_Init();
    HAL_Delay(1000);
    ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
}

void LCDFrontend_DrawInterface(SoftwareTimer *SWTimer)
{
    static uint8_t CurrentMinutes = 0;
    //if(SWTimer->TimerValue >= LCD_FRONTEND_REFRESH_PERIOD)
    if(GetActualMinutes() != CurrentMinutes || LCDFrontend_ExtarnalInterfaceRefresh == FLAG_SET)
    {
        CurrentMinutes = GetActualMinutes();
        ILI9341_Fill_Screen(BLACK);

        FrontendDrawDayName(GetDayString());
        FrontendDrawDate(GetDateString());
        FrontendDrawTime(GetTimeString());

        FrontendDrawOutsideWeatherIcon(GetCurrentWeatherIcon());
        FrontendDrawOutsideWeatherSectionName("OUT");
        FrontendDrawOutsideTemperature(GetCurrentWeatherTemperature());
        FrontendDrawOutsidePressure(GetCurrentWeatherPressure());
        FrontendDrawOutsideHumidity(GetCurrentWeatherHumidity());
        FrontendDrawOutsideRain(GetCurrentWeatherRain1h());

        DrontendDrawInsideSectionName("IN");
        FrontendDrawInsideTemperature(GetInsideTemperatureString());
        FrontendDrawInsideHumidity(GetInsideHumidityString());
        FrontendDrawInsidePresure(GetInsidePressureString());

        FrontendDrawFirstDayWeatherIcon(GetForecast_Day1_Icon());
        FrontendDrwaFirstDayDayName(GetNext_1_DayString());
        FrontendDrawFirstDayTemperatureDay(GetForecast_Day1_TemperatureDay());
        FrontendDrawFirstDayTemperatureNight(GetForecast_Day1_TemperatureNight());

        FrontendDrawFSecondDayWeatherIcon(GetForecast_Day2_Icon());
        FrontendDrawSecondDayDayName(GetNext_2_DayString());
        FrontendDrawSecondDayTemperatureDay(GetForecast_Day2_TemperatureDay());
        FrontendDrawSecondDayTemperatureNight(GetForecast_Day2_TemperatureNight());

        FrontendDrawThirdDayWeatherIcon(GetForecast_Day3_Icon());
        FrontendDrawThirdDayDayName(GetNext_3_DayString());
        FrontendDrawThirdDayTemperatureDay(GetForecast_Day3_TemperatureDay());
        FrontendDrawThirdDayTemperatureNight(GetForecast_Day3_TemperatureNight());

        FrontendDrawWindDirection(GetCurrentWeatherWindDegree());
        FrontendDrawWindSpeed(GetCurrentWeatherWindSpeed());

        FrontendDrawSunInfoSectionName("SUN");
        FrontendDrawSunriseTime(GetCurrentWeatherSunrise());
        FrontendDrawSunsetTime(GetCurrentWeatherSunset());

        FrontendDrawAirPollutionSectionName("AQI");
        FrontendDrawAirPollution(GetAirQualityIndex());

        FrontendDrawAdditionalInfoSectionName("CLOUDS");
        FrontendDrawAdditionalInfoCloudness(GetCurrentWeatherWindCloudness());

        FrontendDrawVisibilitySectionName("VIS");
        FrontendDrawAdditionalInfoVisibility(GetCurrentWeatherWindVisibility());

        FrontendDrawLine();

        StartAndResetTimer(SWTimer);
        LCDFrontend_ExtarnalInterfaceRefresh = FLAG_RESET;
    }
}

void LCDFrontend_DrawConfigInterface(SoftwareTimer *SWTimer)
{
    if(LCDFrontendConfig_ReflashFlag == FLAG_SET)
    {
        ILI9341_Fill_Screen(BLACK);
        ILI9341_Draw_Text("Config Mode", 10, 0, GREEN, 4, BLACK);
        ILI9341_Draw_Text("Network parameters:", 0, 40, WHITE, 2, BLACK);

        ILI9341_Draw_Text("ID: ", 0, 60, WHITE, 2, BLACK);
        ILI9341_Draw_Text(ESP8266Config_GetIPAddress(), 30, 60, YELLOW, 2, BLACK);

        ILI9341_Draw_Text("SSID: ", 0, 80, WHITE, 2, BLACK);
        ILI9341_Draw_Text(ESP8266Config_GetSSID(), 50, 80, YELLOW, 2, BLACK);

        ILI9341_Draw_Text("PASS: ", 0, 100, WHITE, 2, BLACK);
        ILI9341_Draw_Text(ESP8266Config_GetPassword(), 50, 100, YELLOW, 2, BLACK);

        ILI9341_Draw_Text("1.Connect with AP", 0, 140, WHITE, 2, BLACK);
        ILI9341_Draw_Text("2.Enter IP in browser", 0, 160, WHITE, 2, BLACK);
        ILI9341_Draw_Text("3.Set your configuration", 0, 180, WHITE, 2, BLACK);
        LCDFrontendConfig_ReflashFlag = FLAG_RESET;
    }
}

void LCDFrontend_ConfigSetRefleshFlag(void)
{
    LCDFrontendConfig_ReflashFlag = FLAG_SET;
}

void LCDFrontend_ExternalRefresh(void)
{
    LCDFrontend_ExtarnalInterfaceRefresh = FLAG_SET;
}

/* Date and time */
/* ******************************************************************************* */
void FrontendDrawDateAndTime(void)
{
    ILI9341_Draw_Text("Tuesday", 0, 0, WHITE, 2, BLACK);
    ILI9341_Draw_Text("06.10.2022", 130, 0, WHITE, 2, BLACK);
    ILI9341_Draw_Text("12:45", 70, 20, WHITE, 4, BLACK);

    FrontendDrawDayName("Tuesday");
    FrontendDrawDate("06.10.2022");
    FrontendDrawTime("12:45");
}

static void FrontendDrawDayName(char *DayName)
{
    ILI9341_Draw_Text(DayName, 0, 0, WHITE, 2, BLACK);
}

static void FrontendDrawDate(char *Date)
{
    ILI9341_Draw_Text(Date, 130, 0, WHITE, 2, BLACK);
}

static void FrontendDrawTime(char *Time)
{
    ILI9341_Draw_Text(Time, 70, 20, WHITE, 4, BLACK);
}

/* ******************************************************************************* */

/* Curent outside weather */
/* ******************************************************************************* */
void FrontendDrawOutsideWeather(void)
{
    FrontendDrawOutsideWeatherIcon("10");
    FrontendDrawOutsideWeatherSectionName("OUT");
    FrontendDrawOutsideTemperature("8");
    FrontendDrawOutsidePressure("1007");
    FrontendDrawOutsideHumidity("75");
    FrontendDrawOutsideRain("2");
}

static void FrontendDrawOutsideWeatherIcon(char *WeatherIcon)
{
    FrontendDrawWeatherIcon(WeatherIcon, 60, 45);
}

static void FrontendDrawOutsideWeatherSectionName(char *SectionName)
{
    ILI9341_Draw_Text(SectionName, 145, 55, YELLOW, 2, BLACK);
}

static void FrontendDrawOutsideTemperature(char *Temperature)
{

    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    /*
     char *DotPointer = strchr(Temperature, '.');
     if(DotPointer != NULL)
     {
     memcpy(TemperatureToFrontend, Temperature, (DotPointer - Temperature));
     }
     */

    //TemperatureWithoutFractionPart(Temperature,TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 85, 115, WHITE, 2, BLACK);
}

static void FrontendDrawOutsidePressure(char *Pressure)
{
    ILI9341_Draw_Presure(Pressure, 145, 75, WHITE, 2, BLACK);
}
static void FrontendDrawOutsideHumidity(char *Humidity)
{
    ILI9341_Draw_Humidity(Humidity, 145, 95, WHITE, 2, BLACK);
}

static void FrontendDrawOutsideRain(char *RainLevel)
{
    ILI9341_Draw_Rain(RainLevel, 145, 115, WHITE, 2, BLACK);
}
/* ******************************************************************************* */

/* Curent inside weather */
/* ******************************************************************************* */
void FrontendDrawInsideWeather(void)
{
    DrontendDrawInsideSectionName("IN");
    FrontendDrawInsideTemperature("23");
    FrontendDrawInsideHumidity("51");
    FrontendDrawInsidePresure("995");
}

static void DrontendDrawInsideSectionName(char *SectionName)
{
    ILI9341_Draw_Text(SectionName, 15, 55, YELLOW, 2, BLACK);
}
static void FrontendDrawInsideTemperature(char *Temperature)
{
    ILI9341_Draw_Temperature(Temperature, 15, 75, WHITE, 2, BLACK);
}

static void FrontendDrawInsideHumidity(char *Humidity)
{
    ILI9341_Draw_Humidity(Humidity, 15, 95, WHITE, 2, BLACK);
}

static void FrontendDrawInsidePresure(char *Presure)
{
    ILI9341_Draw_Presure(Presure, 10, 115, WHITE, 2, BLACK);
}
/* ******************************************************************************* */

/* Next day weather */
/* ******************************************************************************* */

static void FrontendDrwaFirstDayDayName(char *DayName)
{
    ILI9341_Draw_Text(DayName, 10, 145, YELLOW, 2, BLACK);
}

static void FrontendDrawFirstDayWeatherIcon(char *WeatherIcon)
{
    FrontendDrawWeatherIcon(WeatherIcon, 0, 140);
}

static void FrontendDrawFirstDayTemperatureDay(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 25, 210, WHITE, 2, BLACK);
}

static void FrontendDrawFirstDayTemperatureNight(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 25, 225, DARKGREY, 2, BLACK);
}
/* ******************************************************************************* */

/* Second day weather */
/* ******************************************************************************* */

static void FrontendDrawFSecondDayWeatherIcon(char *WeatherIcon)
{
    FrontendDrawWeatherIcon(WeatherIcon, 75, 140);
}

static void FrontendDrawSecondDayDayName(char *DayName)
{
    ILI9341_Draw_Text(DayName, 95, 145, YELLOW, 2, BLACK);
}

static void FrontendDrawSecondDayTemperatureDay(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 110, 210, WHITE, 2, BLACK);
}

static void FrontendDrawSecondDayTemperatureNight(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 110, 225, DARKGREY, 2, BLACK);
}
/* ******************************************************************************* */

/* Third day weather */
/* ******************************************************************************* */

static void FrontendDrawThirdDayWeatherIcon(char *WeatherIcon)
{
    FrontendDrawWeatherIcon(WeatherIcon, 150, 140);
}

static void FrontendDrawThirdDayDayName(char *DayName)
{
    ILI9341_Draw_Text(DayName, 170, 145, YELLOW, 2, BLACK);
}

static void FrontendDrawThirdDayTemperatureDay(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 185, 210, WHITE, 2, BLACK);
}

static void FrontendDrawThirdDayTemperatureNight(char *Temperature)
{
    char TemperatureToFrontend[10] = "0";
    TemperatureWithoutFractionPart(Temperature, TemperatureToFrontend);
    ILI9341_Draw_Temperature(TemperatureToFrontend, 185, 225, DARKGREY, 2, BLACK);
}
/* ******************************************************************************* */

/* Wind data */
/* ******************************************************************************* */
static void FrontendDrawWindDirection(char *WindDirecion)
{
    int WindDirectionInteger = atoi(WindDirecion);
    ILI9341_Draw_WindDirection(WindDirectionInteger, 120, 270, 20, DARKGREEN, GREEN, RED);
}

static void FrontendDrawWindSpeed(char *WindSpeed)
{
    ILI9341_Draw_WindSpeed(WindSpeed, 95, 295, WHITE, 2, BLACK);
}
/* ******************************************************************************* */

/* Sun Time data */
/* ******************************************************************************* */
static void FrontendDrawSunInfoSectionName(char *SunSectionName)
{
    ILI9341_Draw_Text(SunSectionName, 180, 250, YELLOW, 2, BLACK);
}

static void FrontendDrawSunriseTime(char *SunriseUnixTime)
{
    char SunriseTime[10] = { 0, 0, 0, 0, 0, 0 };
    UnixTimeToHours(SunriseUnixTime, SunriseTime);
    ILI9341_Draw_Text(SunriseTime, 180, 270, WHITE, 2, BLACK);
}

static void FrontendDrawSunsetTime(char *SunsetUnixTime)
{
    char SunsetTime[10] = { 0, 0, 0, 0, 0, 0 };
    UnixTimeToHours(SunsetUnixTime, SunsetTime);
    ILI9341_Draw_Text(SunsetTime, 180, 290, DARKGREY, 2, BLACK);
}
/* ******************************************************************************* */

/* Additional weather data */
/* ******************************************************************************* */
static void FrontendDrawAdditionalInfoSectionName(char *AdditionalSectionName)
{
    ILI9341_Draw_Text(AdditionalSectionName, 5, 250, YELLOW, 2, BLACK);
}

static void FrontendDrawAdditionalInfoCloudness(char *Cloudness)
{
    ILI9341_Draw_Humidity(Cloudness, 5, 265, WHITE, 2, BLACK);
}

static void FrontendDrawVisibilitySectionName(char *AdditionalSectionName)
{
    ILI9341_Draw_Text(AdditionalSectionName, 5, 280, YELLOW, 2, BLACK);
}

static void FrontendDrawAdditionalInfoVisibility(char *Visibility)
{
    ILI9341_Draw_Meters(Visibility, 5, 295, WHITE, 2, BLACK);
}
/* ******************************************************************************* */

/* Air pollution index */
/* ******************************************************************************* */
static void FrontendDrawAirPollutionSectionName(char *SectionName)
{
    ILI9341_Draw_Text(SectionName, 210, 55, YELLOW, 2, BLACK);
}

static void FrontendDrawAirPollution(char *AirPolutionIndex)
{
    uint8_t AirPollution = atoi(AirPolutionIndex);

    switch(AirPollution)
    {
    case 1:
        ILI9341_Draw_Filled_Rectangle_Coord(230, 120, 241, 130, DARKGREEN);
        break;
    case 2:
        ILI9341_Draw_Filled_Rectangle_Coord(230, 120, 241, 130, DARKGREEN);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 108, 241, 118, GREENYELLOW);
        break;
    case 3:
        ILI9341_Draw_Filled_Rectangle_Coord(230, 120, 241, 130, DARKGREEN);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 108, 241, 118, GREENYELLOW);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 96, 241, 106, YELLOW);
        break;
    case 4:
        ILI9341_Draw_Filled_Rectangle_Coord(230, 120, 241, 130, DARKGREEN);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 108, 241, 118, GREENYELLOW);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 96, 241, 106, YELLOW);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 84, 241, 94, ORANGE);
        break;
    case 5:
        ILI9341_Draw_Filled_Rectangle_Coord(230, 120, 241, 130, DARKGREEN);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 108, 241, 118, GREENYELLOW);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 96, 241, 106, YELLOW);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 84, 241, 94, ORANGE);
        ILI9341_Draw_Filled_Rectangle_Coord(230, 72, 241, 82, RED);
        break;
    }

}

/* Frontend line */
/* ******************************************************************************* */
static void FrontendDrawLine(void)
{
    ILI9341_Draw_Hollow_Rectangle_Coord(0, 53, 240, 54, BLUE);
    ILI9341_Draw_Hollow_Rectangle_Coord(70, 53, 71, 141, BLUE);
    ILI9341_Draw_Hollow_Rectangle_Coord(70, 53, 71, 141, BLUE);
    ILI9341_Draw_Hollow_Rectangle_Coord(0, 140, 240, 141, BLUE);
    ILI9341_Draw_Hollow_Rectangle_Coord(0, 243, 240, 244, BLUE);
}

/* Helping function */
/* ******************************************************************************* */

static void FrontendDrawWeatherIcon(char *WeatherIcon, uint16_t X1, uint16_t Y1)
{
    int WeatherIconInteger = atoi(WeatherIcon);

    switch(WeatherIconInteger)
    {
    case (1):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("01d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("01n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (2):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("02d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("02n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (3):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("03d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("03n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (4):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("04d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("04n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (9):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("09d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("09n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (10):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("10d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("10n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (11):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("11d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("11n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (13):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("13d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("13n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;

    case (50):
        if(WeatherIcon[2] == 'd')
            SDCardReadIcon("50d.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        if(WeatherIcon[2] == 'n')
            SDCardReadIcon("50n.txt", SCREEN_HORIZONTAL_1, X1, Y1, 90, 90);
        break;
    }
}

static void TemperatureWithoutFractionPart(const char *TemmperatureIn, char *TemperatureOut)
{
    char *DotPointer = strchr(TemmperatureIn, '.');
    if(DotPointer != NULL)
    {
        memcpy(TemperatureOut, TemmperatureIn, (DotPointer - TemmperatureIn));
    }
    if(TemperatureOut[0] == '-' && TemperatureOut[1] == '0')
    {
        TemperatureOut[0] = 0;
        TemperatureOut[1] = 0;
        memcpy(TemperatureOut, "0", sizeof(char));
    }
}
/* ******************************************************************************* */

void FrontEndDrawDebugInfo(uint32_t InfoToDraw, uint16_t X, uint16_t Y)
{
    char Text[20] = { 0 };
    utoa(InfoToDraw, Text, 10);
    ILI9341_Draw_Text(Text, X, Y, RED, 2, BLACK);
}
