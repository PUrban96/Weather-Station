/*
 * LCDBrightness.h
 *
 *  Created on: Dec 2, 2022
 *      Author: Pietia2
 */

#ifndef INC_LCDBRIGHTNESS_H_
#define INC_LCDBRIGHTNESS_H_

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"

/* LCD Brightness control config */
/* ******************************************************************************** */
#define LCD_PWM_TIMER               htim1
extern TIM_HandleTypeDef            LCD_PWM_TIMER;
#define LCD_PWM_CHANNEL             TIM_CHANNEL_1

#define LCD_BRIGHTNESS_PERIOD       200 /* in milisecond */

#define LCD_PWM_MIN_VALUE           5
#define LCD_PWM_MAX_VALUE           100
/* ******************************************************************************** */

/* Fotosensor  config */
/* ******************************************************************************** */
#define FOTOSENSOR_ADC_HANDLER      hadc1
extern ADC_HandleTypeDef            FOTOSENSOR_ADC_HANDLER;

#define FOTOSENSOR_PERIOD           10 /* in milisecond */
/* ******************************************************************************** */

void LCDBrightness_BrightnessControl(SoftwareTimer *SWTimer);
void LCDBrightness_Init(void);
void LCDFotoSensorMeas(SoftwareTimer *SWTimer);

#endif /* INC_LCDBRIGHTNESS_H_ */
