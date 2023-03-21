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

static uint32_t FotoSensorADCRaw = 0;
static uint16_t FotoSensorMeasNumber = 0;
static uint64_t FotoSensorAverage = 0;


void LCDBrightnessInit(void)
{
    HAL_TIM_PWM_Start(&LCD_PWM_TIMER, LCD_PWM_CHANNEL);
    __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, 20);

    HAL_ADCEx_Calibration_Start(&FOTOSENSOR_ADC_HANDLER);

    HAL_ADC_Start(&FOTOSENSOR_ADC_HANDLER);
    HAL_ADC_PollForConversion(&FOTOSENSOR_ADC_HANDLER, HAL_MAX_DELAY);
}

void LCDBrightnessControl(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= LCD_BRIGHTNESS_PERIOD)
    {
        FotoSensorADCRaw = HAL_ADC_GetValue(&FOTOSENSOR_ADC_HANDLER);
        if(FotoSensorADCRaw <= 150)
        {
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, LCD_PWM_MIN_VALUE);
        }

        if(FotoSensorADCRaw > 150 && FotoSensorADCRaw < 2000)
        {
            uint32_t PWMCalculate = (486 * FotoSensorADCRaw + 27027);
            PWMCalculate /= 10000;
            uint8_t PWMToSet = (uint8_t) PWMCalculate;
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, PWMToSet);
        }

        if(FotoSensorADCRaw >= 2000)
        {
            __HAL_TIM_SET_COMPARE(&LCD_PWM_TIMER, LCD_PWM_CHANNEL, LCD_PWM_MAX_VALUE);
        }
        HAL_ADC_Start(&FOTOSENSOR_ADC_HANDLER);
        ResetTimer(SWTimer);
    }
}

void LCDFotoSensorMeas(SoftwareTimer *SWTimer)
{
    if(SWTimer->TimerValue >= FOTOSENSOR_PERIOD)
    {
        FotoSensorMeasNumber++;
        FotoSensorAverage += HAL_ADC_GetValue(&FOTOSENSOR_ADC_HANDLER);
        ResetTimer(SWTimer);
        HAL_ADC_Start(&FOTOSENSOR_ADC_HANDLER);
    }
}
