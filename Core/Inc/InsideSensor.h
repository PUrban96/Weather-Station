/*
 * InsideSensor.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_INSIDESENSOR_H_
#define INC_INSIDESENSOR_H_

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"

/* Inside sensor config */
/* ******************************************************************************** */
#define I2C_HANDLER hi2c1
extern I2C_HandleTypeDef I2C_HANDLER;

#define INSIDE_SENSOR_PERIOD 10000 /* in milisecond */
/* ******************************************************************************** */

void InsideSensor_Init(void);
void InsideSensor_IndoorMeasurement(SoftwareTimer *SWTimer);

char *GetInsideTemperatureString(void);
char *GetInsidePressureString(void);
char *GetInsideHumidityString(void);

#endif /* INC_INSIDESENSOR_H_ */
