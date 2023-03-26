/*
 * SDCardConfig.h
 *
 *  Created on: 14 mar 2023
 *      Author: Pietia2
 */

#ifndef INC_SDCARDCONFIG_H_
#define INC_SDCARDCONFIG_H_

#include "main.h"
#include "stm32g0xx_hal.h"

#define SDCARDCONFIG_READ_BUFFER_SIZE 200

typedef enum _SDConfig_Result_e
{
    SDCONFIG_ERR, SDCONFIG_OK
} SDConfig_Result_e;

void SDCardConfig_Init(void);
SDConfig_Result_e SDCardConfig_ReadConfig(void);
SDConfig_Result_e SDCardConfig_WriteNetwork(const char *NewSSID, const char *NewPASS);
SDConfig_Result_e SDCardConfig_WriteApikey(const char *NewAPIKEY);
SDConfig_Result_e SDCardConfig_WriteLocalisation(const char *NewCITY, const char *NewLATITUDE, const char *NewLONGITUDE);
char* SDCardConfig_GetNetworkSSID();
char* SDCardConfig_GetNetworkPASS();
char* SDCardConfig_GetAPIKEY();
char* SDCardConfig_GetLocalisationCITY();
char* SDCardConfig_GetLocalisationLATITUDE();
char* SDCardConfig_GetLocalisationLONGITUDE();

#endif /* INC_SDCARDCONFIG_H_ */
