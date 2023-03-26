/*
 * ILI9341_STM32_Driver.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Pietia2
 */

#ifndef INC_ILI9341_STM32_DRIVER_H_
#define INC_ILI9341_STM32_DRIVER_H_

#include "stm32g0xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define ILI9341_SCREEN_HEIGHT 240
#define ILI9341_SCREEN_WIDTH    320

//SPI INSTANCE
#define HSPI_INSTANCE                           &hspi1

//CHIP SELECT PIN AND PORT, STANDARD GPIO
#define LCD_CS_PORT                             SPI1_LCD_CS_GPIO_Port
#define LCD_CS_PIN                              SPI1_LCD_CS_Pin

//DATA COMMAND PIN AND PORT, STANDARD GPIO
#define LCD_DC_PORT                             LCD_DC_GPIO_Port
#define LCD_DC_PIN                              LCD_DC_Pin

//RESET PIN AND PORT, STANDARD GPIO
#define LCD_RST_PORT                            LCD_RESET_GPIO_Port
#define LCD_RST_PIN                             LCD_RESET_Pin


#define BURST_MAX_SIZE  500

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F
#define DARKORANGE  0xFA20

#define SCREEN_VERTICAL_1           0
#define SCREEN_HORIZONTAL_1     1
#define SCREEN_VERTICAL_2           2
#define SCREEN_HORIZONTAL_2     3

void ILI9341_SPI_Init(void);
void ILI9341_SPI_Send(unsigned char SPI_Data);
void ILI9341_Write_Command(uint8_t Command);
void ILI9341_Write_Data(uint8_t Data);
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);
void ILI9341_Reset(void);
void ILI9341_Set_Rotation(uint8_t Rotation);
void ILI9341_Enable(void);
void ILI9341_Init(void);
void ILI9341_Fill_Screen(uint16_t Colour);
void ILI9341_Draw_Colour(uint16_t Colour);
void ILI9341_Draw_Pixel(uint16_t X,uint16_t Y,uint16_t Colour);
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size);


void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour);
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour);
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour);

void ILI9341_Draw_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour);


#endif /* INC_ILI9341_STM32_DRIVER_H_ */
