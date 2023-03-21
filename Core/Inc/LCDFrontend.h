/*
 * LCDFrontend.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_LCDFRONTEND_H_
#define INC_LCDFRONTEND_H_

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SoftwareTimers.h"


/* Inside sensor config */
/* ******************************************************************************** */
#define LCD_FRONTEND_REFRESH_PERIOD 10000 /* in milisecond */
/* ******************************************************************************** */

void FrontendDrawInterface(SoftwareTimer *SWTimer);
void LCDFrontend_DrawConfigInterface(SoftwareTimer *SWTimer);
void LCDFrontend_ConfigSetRefleshFlag(void);
void FrontendInit(void);

void FrontEndDrawDebugInfo(uint32_t InfoToDraw, uint16_t X, uint16_t Y);

#endif /* INC_LCDFRONTEND_H_ */
