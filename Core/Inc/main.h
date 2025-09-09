/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SHUTDOWN1_Pin GPIO_PIN_13
#define SHUTDOWN1_GPIO_Port GPIOC
#define SHUTDOWN2_Pin GPIO_PIN_14
#define SHUTDOWN2_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOC
#define V_AC_Pin GPIO_PIN_0
#define V_AC_GPIO_Port GPIOA
#define TEMP_Pin GPIO_PIN_1
#define TEMP_GPIO_Port GPIOA
#define I_DC1_Pin GPIO_PIN_2
#define I_DC1_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOA
#define V_BAT1_Pin GPIO_PIN_6
#define V_BAT1_GPIO_Port GPIOA
#define V_DC1_Pin GPIO_PIN_7
#define V_DC1_GPIO_Port GPIOA
#define V_DC2_Pin GPIO_PIN_0
#define V_DC2_GPIO_Port GPIOB
#define I_DC2_Pin GPIO_PIN_1
#define I_DC2_GPIO_Port GPIOB
#define B6_Pin GPIO_PIN_2
#define B6_GPIO_Port GPIOB
#define B6_EXTI_IRQn EXTI2_IRQn
#define B5_Pin GPIO_PIN_10
#define B5_GPIO_Port GPIOB
#define B5_EXTI_IRQn EXTI15_10_IRQn
#define B4_Pin GPIO_PIN_11
#define B4_GPIO_Port GPIOB
#define B4_EXTI_IRQn EXTI15_10_IRQn
#define B1_Pin GPIO_PIN_12
#define B1_GPIO_Port GPIOB
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define B2_Pin GPIO_PIN_13
#define B2_GPIO_Port GPIOB
#define B2_EXTI_IRQn EXTI15_10_IRQn
#define B3_Pin GPIO_PIN_14
#define B3_GPIO_Port GPIOB
#define B3_EXTI_IRQn EXTI15_10_IRQn
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_BL_GPIO_Port GPIOB
#define LCD_D2_Pin GPIO_PIN_9
#define LCD_D2_GPIO_Port GPIOA
#define LCD_D3_Pin GPIO_PIN_10
#define LCD_D3_GPIO_Port GPIOA
#define LCD_D1_Pin GPIO_PIN_11
#define LCD_D1_GPIO_Port GPIOA
#define LCD_D0_Pin GPIO_PIN_12
#define LCD_D0_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_3
#define LCD_RS_GPIO_Port GPIOB
#define LCD_E_Pin GPIO_PIN_4
#define LCD_E_GPIO_Port GPIOB
#define RTS_Pin GPIO_PIN_5
#define RTS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
