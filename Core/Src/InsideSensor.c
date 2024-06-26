/*
 * InsideSensor.c
 *
 *  Created on: 3 lis 2022
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "InsideSensor.h"
#include "SoftwareTimers.h"
#include "BMXX80.h"
#include <stdlib.h>
#include "Common.h"

/* Variable */
/* ******************************************************************************** */
static float InsideTemperature = 0;
static int32_t InsidePressure = 0;
static float InsideHumidity = 0;

static char InsideTemperatureString[6] = { 0 };
static char InsidePresureString[6] = { 0 };
static char InsideHumidityString[6] = { 0 };

static Common_FlagState_e InsideSensor_ExternalRequest = FLAG_RESET;
/* ******************************************************************************** */

void InsideSensor_IndoorMeasurement(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= INSIDE_SENSOR_PERIOD || InsideSensor_ExternalRequest == FLAG_SET)
    {
        BME280_ReadTemperatureAndPressureAndHuminidity(&InsideTemperature, &InsidePressure, &InsideHumidity);
        StartAndResetTimer(SWTimer);
        InsideSensor_ExternalRequest = FLAG_RESET;
    }
}

void InsideSensor_Init(void)
{
    //HAL_GPIO_WritePin(BME_POWER_GPIO_Port, BME_POWER_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    BME280_Init(&I2C_HANDLER, BME280_TEMPERATURE_16BIT, BME280_PRESSURE_ULTRALOWPOWER, BME280_HUMINIDITY_STANDARD, BME280_NORMALMODE);
    BME280_SetConfig(BME280_STANDBY_MS_10, BME280_FILTER_OFF);
}

char* GetInsideTemperatureString(void)
{
    utoa(InsideTemperature, InsideTemperatureString, 10);
    return InsideTemperatureString;
}

char* GetInsidePressureString(void)
{
    utoa(InsidePressure / 100, InsidePresureString, 10);
    return InsidePresureString;
}

char* GetInsideHumidityString(void)
{
    utoa(InsideHumidity, InsideHumidityString, 10);
    return InsideHumidityString;
}

void InsideSensor_ExternalMeasRequest(void)
{
    InsideSensor_ExternalRequest = FLAG_SET;
}
