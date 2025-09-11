/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "lcdMenu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  HAL_RCC_NMI_IRQHandler();
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RCC global interrupt.
  */
void RCC_IRQHandler(void)
{
  /* USER CODE BEGIN RCC_IRQn 0 */

  /* USER CODE END RCC_IRQn 0 */
  /* USER CODE BEGIN RCC_IRQn 1 */

  /* USER CODE END RCC_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(B6_Pin);
  /* USER CODE BEGIN EXTI2_IRQn 1 */
  {
    uint32_t idr = GPIOB->IDR;
    uint8_t s = (uint8_t)(
        ((!!(idr & B1_Pin)) << BUT_LEFT_POS)  |
        ((!!(idr & B2_Pin)) << BUT_ON_POS)    |
        ((!!(idr & B3_Pin)) << BUT_UP_POS)    |
        ((!!(idr & B4_Pin)) << BUT_DOWN_POS)  |
        ((!!(idr & B5_Pin)) << BUT_RIGHT_POS) |
        ((!!(idr & B6_Pin)) << BUT_OFF_POS)
    );
    buttonState = s;
    HAL_TIM_Base_Start_IT(&htim7); // IT = interrupt
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 1);
  }

  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
	uint8_t adcChannelCounter = 0;
	uint16_t dcOffset = 1985;

	adcBuffer[adcChannelCounter] = (q15_t)(((int32_t)(adc1Buffer[adcChannelCounter] - dcOffset)
			* adcGain[adcChannelCounter]) >> 15);
	adcChannelCounter++;

	while(adcChannelCounter < ENUM_ADC_CHANNEL_COUNT)
	{
		adcBuffer[adcChannelCounter] = (q15_t)(((int32_t)(adc1Buffer[adcChannelCounter]) * adcGain[adcChannelCounter]) >> 15);
		adcChannelCounter++;
	}
	adcChannelCounter = 1;
	while(adcChannelCounter < ENUM_ADC_CHANNEL_COUNT)
	{
		adcMeanSum[adcChannelCounter - 1] = adcMeanSum[adcChannelCounter - 1] - adcMeanBuffer[adcChannelCounter - 1][adcMeanBufferPo];
		adcMeanBuffer[adcChannelCounter - 1][adcMeanBufferPo] = adcBuffer[adcChannelCounter];
		adcMeanSum[adcChannelCounter - 1] = adcMeanSum[adcChannelCounter - 1] + adcMeanBuffer[adcChannelCounter - 1][adcMeanBufferPo];
		adcChannelCounter++;
	}

	adcMeanBufferPo++;
	if (adcMeanBufferPo >= ADC_MEAN_BUFFER_SIZE)
	{
		adcMeanBufferPo = 0;
	}

	adcRmsSum = adcRmsSum - adcRmsBuffer[adcRmsBufferPo];
    adcRmsBuffer[adcRmsBufferPo] = (int32_t)adcBuffer[listVAC] * (int32_t)adcBuffer[listVAC];
	adcRmsSum = adcRmsSum + adcRmsBuffer[adcRmsBufferPo];
	adcRmsBufferPo++;
	if (adcRmsBufferPo >= ADC_RMS_BUFFER_SIZE)
	{
		adcRmsBufferPo = 0;
	}

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
	DMA1->IFCR |= DMA_IFCR_CGIF1;
  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles ADC1 global interrupt.
  */
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */

  /* USER CODE END ADC1_IRQn 0 */
  /* USER CODE BEGIN ADC1_IRQn 1 */

  /* USER CODE END ADC1_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(B5_Pin);
  HAL_GPIO_EXTI_IRQHandler(B4_Pin);
  HAL_GPIO_EXTI_IRQHandler(B1_Pin);
  HAL_GPIO_EXTI_IRQHandler(B2_Pin);
  HAL_GPIO_EXTI_IRQHandler(B3_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */
    uint32_t idr = GPIOB->IDR;
    uint8_t s = (uint8_t)(
        ((!!(idr & B1_Pin)) << BUT_LEFT_POS)  |
        ((!!(idr & B2_Pin)) << BUT_ON_POS)    |
        ((!!(idr & B3_Pin)) << BUT_UP_POS)    |
        ((!!(idr & B4_Pin)) << BUT_DOWN_POS)  |
        ((!!(idr & B5_Pin)) << BUT_RIGHT_POS) |
        ((!!(idr & B6_Pin)) << BUT_OFF_POS)
    );
    buttonState = s;
    HAL_TIM_Base_Start_IT(&htim7); // IT = interrupt
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 1);
  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
