/*
 * LCDBrightness.c
 *
 *  Created on: Dec 2, 2022
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "LCDBrightness.h"
#include "SoftwareTimers.h"
#include "Common.h"

/* Variable */
/* ******************************************************************************** */
static uint16_t FotoSensorMeasNumber = 0;
static uint64_t FotoSensorAverage = 0;
/* ******************************************************************************** */

static uint64_t LCDBrigtness_GetLightSensorAVGValue(void);

void LCDBrightness_Init(void)
{
    HAL_TIM_PWM_Start(&LCD_PWM_TIMER, LCD_PWM_CHANNEL);
    __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, 20);

    HAL_ADCEx_Calibration_Start(&FOTOSENSOR_ADC_HANDLER);

    HAL_ADC_Start(&FOTOSENSOR_ADC_HANDLER);
    HAL_ADC_PollForConversion(&FOTOSENSOR_ADC_HANDLER, HAL_MAX_DELAY);
}


void LCDBrightness_BrightnessControl(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= LCD_BRIGHTNESS_PERIOD)
    {
        uint32_t FotoSensorADCRaw = LCDBrigtness_GetLightSensorAVGValue();

        if(FotoSensorADCRaw <= 200)
        {
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, LCD_PWM_MIN_VALUE);
        }

        if(FotoSensorADCRaw > 200 && FotoSensorADCRaw < 2000)
        {
            uint32_t PWMCalculate = (553 * FotoSensorADCRaw - 109220);
            PWMCalculate /= 1000;
            uint16_t PWMToSet = (uint16_t) PWMCalculate;
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, PWMToSet);
        }

        if(FotoSensorADCRaw >= 2000)
        {
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, LCD_PWM_MAX_VALUE);
        }

        ResetTimer(SWTimer);
    }
}


void LCDBrigtness_LightSensorMeas(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= FOTOSENSOR_PERIOD)
    {
        FotoSensorMeasNumber++;
        FotoSensorAverage += HAL_ADC_GetValue(&FOTOSENSOR_ADC_HANDLER);
        ResetTimer(SWTimer);
        HAL_ADC_Start(&FOTOSENSOR_ADC_HANDLER);
    }
}

static uint64_t LCDBrigtness_GetLightSensorAVGValue(void)
{
    uint64_t AVGValue = FotoSensorAverage / FotoSensorMeasNumber;
    FotoSensorMeasNumber = 0;
    FotoSensorAverage = 0;
    return AVGValue;
}
