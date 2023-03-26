/*
 * SDCardIcon.h
 *
 *  Created on: Dec 3, 2022
 *      Author: Pietia2
 */

#ifndef INC_SDCARDICON_H_
#define INC_SDCARDICON_H_

#include "main.h"
#include "stm32g0xx_hal.h"

#define SD_CARD_ICON_SIZE        10400
#define ROW_BUFFER_SIZE           3600
#define LINE_READ_AMOUNT            10

uint8_t SDCardReadIcon(char *FileName, uint8_t Orientation, uint16_t X1, uint16_t Y1, uint16_t widht, uint16_t height);

#endif /* INC_SDCARDICON_H_ */
