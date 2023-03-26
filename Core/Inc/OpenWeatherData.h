/*
 * OpenWeatherData.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_OPENWEATHERDATA_H_
#define INC_OPENWEATHERDATA_H_

#include "main.h"
#include "stm32g0xx_hal.h"

void AirPollutionDataParse(const char *AirPollutionJSON);
char *GetAirQualityIndex(void);

void CurrentWeatherDataParse(const char *CurrentWeatherJSON);
char *GetCurrentWeatherPressure();
char *GetCurrentWeatherHumidity();
char *GetCurrentWeatherTemperature();
char *GetCurrentWeatherRain1h();
char *GetCurrentWeatherSunrise();
char *GetCurrentWeatherSunset();
char *GetCurrentWeatherWindSpeed();
char *GetCurrentWeatherWindDegree();
char* GetCurrentWeatherWindCloudness();
char* GetCurrentWeatherWindVisibility();
char* GetCurrentWeatherIcon();
char* GetCurrentWeatherActualDate();

void ForecastWeatherDataParse(const char *ForecastJSON);
char* GetForecast_Day1_TemperatureDay();
char* GetForecast_Day1_TemperatureNight();
char* GetForecast_Day1_Icon();
char* GetForecast_Day2_TemperatureDay();
char* GetForecast_Day2_TemperatureNight();
char* GetForecast_Day2_Icon();
char* GetForecast_Day3_TemperatureDay();
char* GetForecast_Day3_TemperatureNight();
char* GetForecast_Day3_Icon();


#endif /* INC_OPENWEATHERDATA_H_ */
