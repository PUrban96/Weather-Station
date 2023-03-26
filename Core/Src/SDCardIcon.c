/*
 * SDCardIcon.c
 *
 *  Created on: Dec 3, 2022
 *      Author: Pietia2
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "SDCardIcon.h"
#include "app_fatfs.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

static BYTE SDCardIconRowBuffer[ROW_BUFFER_SIZE];

static inline uint8_t AcsiiToIntConverterCalc(char AsciiSign);
static inline uint8_t SDCardAsciiToInt(char AsciiSignHigh, char AsciiSignLow);
static inline void SDCardConvertLine(uint16_t widht, uint16_t LineAmount);

void SDCardIconInit(void)
{
    FATFS fs;
    FRESULT res;

    res = f_mount(&fs, "", 0);
    if(res != FR_OK)
    {
        ;
    }
}

uint8_t SDCardReadIcon(char *FileName, uint8_t Orientation, uint16_t X1, uint16_t Y1, uint16_t widht, uint16_t height)
{

    //ILI9341_Draw_Bitmap((const char*) WeatherIcon_01d, SCREEN_HORIZONTAL_1, X1, Y1, widht, height);

    FATFS fs;
    FIL fil;
    FRESULT res;
    UINT ReceiveBytes = 0;

    res = f_mount(&fs, "", 0);
    if(res == FR_OK)
    {
        res = f_open(&fil, FileName, FA_READ);
        for(int i = 0; i < height/LINE_READ_AMOUNT; i++)
        {
            if(i > 0)
                res = f_lseek(&fil, i * (widht * 4) *LINE_READ_AMOUNT * sizeof(char));
            res = f_read(&fil, SDCardIconRowBuffer, (widht * 4) * LINE_READ_AMOUNT, &ReceiveBytes);
            if(res == FR_OK)
            {
                SDCardConvertLine(widht,LINE_READ_AMOUNT);
                ILI9341_Draw_Bitmap((const char*) SDCardIconRowBuffer, SCREEN_HORIZONTAL_1, X1, Y1 + (i * LINE_READ_AMOUNT), widht, LINE_READ_AMOUNT);
            }
        }
        f_close(&fil);
    }
    return res;

}

static inline void SDCardConvertLine(uint16_t widht, uint16_t LineAmount)
{
    uint16_t LoopNumber = 0;
    for(int i = 0; i < (widht * 4) * LineAmount; i += 2)
    {
        SDCardIconRowBuffer[LoopNumber] = SDCardAsciiToInt(SDCardIconRowBuffer[i], SDCardIconRowBuffer[i + 1]);
        LoopNumber++;
    }

}

static inline uint8_t SDCardAsciiToInt(char AsciiSignHigh, char AsciiSignLow)
{
    uint8_t IntValue = 0;

    uint8_t ConverterHigh = AcsiiToIntConverterCalc(AsciiSignHigh);
    uint8_t ConverterLow = AcsiiToIntConverterCalc(AsciiSignLow);

    IntValue = ((AsciiSignHigh - ConverterHigh) * 16) + (AsciiSignLow - ConverterLow);
    return IntValue;
}

static inline uint8_t AcsiiToIntConverterCalc(char AsciiSign)
{
    uint8_t Converter = 0;

    if(AsciiSign > 64)
        Converter = 55;
    else
        Converter = 48;

    return Converter;
}

