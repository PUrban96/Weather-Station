/*
 * SDCardConfig.c
 *
 *  Created on: 14 mar 2023
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "app_fatfs.h"
#include "SDCardConfig.h"
#include <string.h>
#include "Common.h"
#include "SystemState.h"
#include "sdcard.h"

static const char SDCardConfig_Network_FileName[] = "network.txt";
static const char SDCardConfig_Apikey_FileName[] = "apikey.txt";
static const char SDCardConfig_Localisation_FileName[] = "locdata.txt";

static char SDCardConfig_SSID[30] = { 0 };
static char SDCardConfig_PASS[30] = { 0 };
static char SDCardConfig_APIKEY[40] = { 0 };
static char SDCardConfig_CITY[30] = { 0 };
static char SDCardConfig_LATITUDE[30] = { 0 };
static char SDCardConfig_LONGITUDE[30] = { 0 };

static char SDCardCOnfig_ReadBuffer[SDCARDCONFIG_READ_BUFFER_SIZE] = { 0 };

static SDConfig_Result_e SDCardConfig_ReadNetwork(void);
static SDConfig_Result_e SDCardConfig_ReadApikey(void);
static SDConfig_Result_e SDCardConfig_ReadLocalisation(void);
static void SDCardConfig_GetParametersValue(const char *InputData, const char *ParameterNameIn, char *ParamaterValueOut);

/* Init */
/* ******************************************************************************* */
void SDCardConfig_Init(void)
{
    HAL_SPI_Init(&hspi2);

    SDCardConfig_ReadConfig();

    /* Increase speed after SD Card initialization */
    SDCARD_SPI_PORT.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    HAL_SPI_Init(&hspi2);
}

SDConfig_Result_e SDCardConfig_ReadConfig(void)
{
    SDConfig_Result_e status = SDCONFIG_OK;

    SDCardConfig_ReadNetwork();
    SDCardConfig_ReadApikey();
    SDCardConfig_ReadLocalisation();

    return status;
}
/* ******************************************************************************* */

/* Config read function */
/* ******************************************************************************* */
static SDConfig_Result_e SDCardConfig_ReadNetwork(void)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    FSIZE_t FileSize = 0;
    UINT ReceiveBytes = 0;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_open(&fil, SDCardConfig_Network_FileName, FA_READ);
        if(res == FR_OK)
        {
            FileSize = f_size(&fil);
            Common_ArrayClean(SDCardCOnfig_ReadBuffer, SDCARDCONFIG_READ_BUFFER_SIZE);
            res = f_read(&fil, SDCardCOnfig_ReadBuffer, FileSize, &ReceiveBytes);
            if(res == FR_OK)
            {
                status = SDCONFIG_OK;
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "SSID", SDCardConfig_SSID);
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "PASS", SDCardConfig_PASS);
            }
        }
        f_close(&fil);
    }
    return status;
}

static SDConfig_Result_e SDCardConfig_ReadApikey(void)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    FSIZE_t FileSize = 0;
    UINT ReceiveBytes = 0;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_open(&fil, SDCardConfig_Apikey_FileName, FA_READ);
        if(res == FR_OK)
        {
            FileSize = f_size(&fil);
            Common_ArrayClean(SDCardCOnfig_ReadBuffer, SDCARDCONFIG_READ_BUFFER_SIZE);
            res = f_read(&fil, SDCardCOnfig_ReadBuffer, FileSize, &ReceiveBytes);
            if(res == FR_OK)
            {
                status = SDCONFIG_OK;
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "APIKEY", SDCardConfig_APIKEY);
            }
        }
        f_close(&fil);
    }
    return status;
}

static SDConfig_Result_e SDCardConfig_ReadLocalisation(void)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    FSIZE_t FileSize = 0;
    UINT ReceiveBytes = 0;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_open(&fil, SDCardConfig_Localisation_FileName, FA_READ);
        if(res == FR_OK)
        {
            FileSize = f_size(&fil);
            Common_ArrayClean(SDCardCOnfig_ReadBuffer, SDCARDCONFIG_READ_BUFFER_SIZE);
            res = f_read(&fil, SDCardCOnfig_ReadBuffer, FileSize, &ReceiveBytes);
            if(res == FR_OK)
            {
                status = SDCONFIG_OK;
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "CITY", SDCardConfig_CITY);
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "LATITUDE", SDCardConfig_LATITUDE);
                SDCardConfig_GetParametersValue(SDCardCOnfig_ReadBuffer, "LONGITUDE", SDCardConfig_LONGITUDE);
            }
        }
        f_close(&fil);
    }
    return status;
}
/* ******************************************************************************* */

/* Config write function */
/* ******************************************************************************* */
SDConfig_Result_e SDCardConfig_WriteNetwork(const char *NewSSID, const char *NewPASS)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_unlink(SDCardConfig_Network_FileName);
        res = f_open(&fil, SDCardConfig_Network_FileName, FA_CREATE_ALWAYS | FA_WRITE);
        if(res == FR_OK)
        {
            res = f_printf(&fil, "SSID=%s$PASS=%s$", NewSSID, NewPASS);
            if(res > 0)
            {
                status = SDCONFIG_OK;
                Common_ArrayClean(SDCardConfig_SSID, 30);
                Common_ArrayClean(SDCardConfig_PASS, 30);
                strcpy(SDCardConfig_SSID, NewSSID);
                strcpy(SDCardConfig_PASS, NewPASS);
            }
        }
        f_close(&fil);
    }
    return status;
}

SDConfig_Result_e SDCardConfig_WriteApikey(const char *NewAPIKEY)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_unlink(SDCardConfig_Apikey_FileName);
        res = f_open(&fil, SDCardConfig_Apikey_FileName, FA_CREATE_ALWAYS | FA_WRITE);
        if(res == FR_OK)
        {
            res = f_printf(&fil, "APIKEY=%s$", NewAPIKEY);
            if(res > 0)
            {
                status = SDCONFIG_OK;
                Common_ArrayClean(SDCardConfig_APIKEY, 40);
                strcpy(SDCardConfig_APIKEY, NewAPIKEY);
            }
        }
        f_close(&fil);
    }
    return status;
}

SDConfig_Result_e SDCardConfig_WriteLocalisation(const char *NewCITY, const char *NewLATITUDE, const char *NewLONGITUDE)
{
    FATFS fs;
    FIL fil;
    FRESULT res;
    SDConfig_Result_e status = SDCONFIG_ERR;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_unlink(SDCardConfig_Localisation_FileName);
        res = f_open(&fil, SDCardConfig_Localisation_FileName, FA_CREATE_ALWAYS | FA_WRITE);
        if(res == FR_OK)
        {
            res = f_printf(&fil, "CITY=%s$LATITUDE=%s$LONGITUDE=%s$", NewCITY, NewLATITUDE, NewLONGITUDE);
            if(res > 0)
            {
                status = SDCONFIG_OK;
                Common_ArrayClean(SDCardConfig_CITY, 30);
                Common_ArrayClean(SDCardConfig_LATITUDE, 30);
                Common_ArrayClean(SDCardConfig_LONGITUDE, 30);
                strcpy(SDCardConfig_CITY, NewCITY);
                strcpy(SDCardConfig_LATITUDE, NewLATITUDE);
                strcpy(SDCardConfig_LONGITUDE, NewLONGITUDE);
            }
        }
        f_close(&fil);
    }
    return status;
}
/* ******************************************************************************* */

/* Getter function */
/* ******************************************************************************* */

char* SDCardConfig_GetNetworkSSID()
{
    return SDCardConfig_SSID;
}

char* SDCardConfig_GetNetworkPASS()
{
    return SDCardConfig_PASS;
}

char* SDCardConfig_GetAPIKEY()
{
    return SDCardConfig_APIKEY;
}

char* SDCardConfig_GetLocalisationCITY()
{
    return SDCardConfig_CITY;
}

char* SDCardConfig_GetLocalisationLATITUDE()
{
    return SDCardConfig_LATITUDE;
}

char* SDCardConfig_GetLocalisationLONGITUDE()
{
    return SDCardConfig_LONGITUDE;
}
/* ******************************************************************************* */

static void SDCardConfig_GetParametersValue(const char *InputData, const char *ParameterNameIn, char *ParamaterValueOut)
{
    char *ParameterPointer = NULL;
    char *StartParameterPointer;
    char *EndParameterPointer1;
    char *EndParameterPointer2;

    ParameterPointer = strstr(InputData, ParameterNameIn);
    if(ParameterPointer != NULL)
    {

        StartParameterPointer = strchr(ParameterPointer, '=') + sizeof(char);
        EndParameterPointer1 = strchr(StartParameterPointer, '$');
        EndParameterPointer2 = strchr(StartParameterPointer, '$');

        char *EndParameterPointer = NULL;

        if(EndParameterPointer1 == NULL)
        {
            EndParameterPointer = (EndParameterPointer2);
        }
        else if(EndParameterPointer2 == NULL)
        {
            EndParameterPointer = EndParameterPointer1;
        }
        else if(EndParameterPointer1 != NULL && EndParameterPointer2 != NULL)
        {
            EndParameterPointer = (EndParameterPointer1 <= EndParameterPointer2) ? EndParameterPointer1 : EndParameterPointer2;
        }
        else
        {
            ParamaterValueOut = NULL;
            return;
        }

        uint8_t ParameterLen = EndParameterPointer - StartParameterPointer;
        memcpy(ParamaterValueOut, StartParameterPointer, ParameterLen);
    }
}
