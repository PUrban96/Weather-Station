/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_RED_Pin GPIO_PIN_11
#define LED_RED_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOC
#define T_IRQ_Pin GPIO_PIN_0
#define T_IRQ_GPIO_Port GPIOC
#define T_IRQ_EXTI_IRQn EXTI0_1_IRQn
#define SPI2_SD_CS_Pin GPIO_PIN_1
#define SPI2_SD_CS_GPIO_Port GPIOC
#define SPI2_LORA_CS_Pin GPIO_PIN_1
#define SPI2_LORA_CS_GPIO_Port GPIOA
#define SPI1_LCD_CS_Pin GPIO_PIN_4
#define SPI1_LCD_CS_GPIO_Port GPIOA
#define SPI2_T_CS_Pin GPIO_PIN_4
#define SPI2_T_CS_GPIO_Port GPIOC
#define BMP_CSB_Pin GPIO_PIN_5
#define BMP_CSB_GPIO_Port GPIOC
#define BUTTON_2_Pin GPIO_PIN_14
#define BUTTON_2_GPIO_Port GPIOB
#define BUTTON_2_EXTI_IRQn EXTI4_15_IRQn
#define BUTTON_1_Pin GPIO_PIN_15
#define BUTTON_1_GPIO_Port GPIOB
#define BUTTON_1_EXTI_IRQn EXTI4_15_IRQn
#define BMP_SDO_Pin GPIO_PIN_6
#define BMP_SDO_GPIO_Port GPIOC
#define PMS_RESET_Pin GPIO_PIN_7
#define PMS_RESET_GPIO_Port GPIOC
#define ESP_SW_Pin GPIO_PIN_8
#define ESP_SW_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_11
#define LCD_RESET_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_12
#define LCD_DC_GPIO_Port GPIOA
#define PMS_SLEEP_Pin GPIO_PIN_8
#define PMS_SLEEP_GPIO_Port GPIOC
#define LORA_RST_Pin GPIO_PIN_4
#define LORA_RST_GPIO_Port GPIOD
#define LORA_SW_Pin GPIO_PIN_5
#define LORA_SW_GPIO_Port GPIOD
#define ESP_RST_Pin GPIO_PIN_6
#define ESP_RST_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
