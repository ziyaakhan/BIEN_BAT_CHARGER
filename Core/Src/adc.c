/*
 * adc.c
 *
 *  Created on: Sep 9, 2025
 *      Author: Ziya
 */

#include "adc.h"

// ADC BUFFER VAR
q15_t adc1Buffer[ADC1_CHANNEL_COUNT];
int16_t adcBuffer[ADC1_CHANNEL_COUNT];

// ADC MEAN VAR

q15_t adcMeanBuffer[ADC1_CHANNEL_COUNT][ADC_MEAN_BUFFER_SIZE];
int32_t adcMeanSum[ADC_MEAN_CHANNEL_COUNT];
uint8_t adcMeanBufferPo = 0;

// ADC RMS VAR

int32_t adcRmsBuffer[ADC_RMS_BUFFER_SIZE];
int64_t adcRmsSum;
uint8_t adcRmsBufferPo;

uint16_t adcVAC = 0;
uint16_t adcTEMP = 0;
uint16_t adcIDC = 0;
uint16_t adcVBAT1 = 0;
uint16_t adcVDC1 = 0;
uint16_t adcVDC2 = 0;
uint16_t adcIDC2NoGain = 0;
uint16_t adcIDC2 = 0;
uint8_t temp;

int16_t adcGain[ADC1_CHANNEL_COUNT + 3];
uint16_t dcOffset = 1985;

void adc_init(void)
{
	adcGain[listVAC]   = 9100;
	adcGain[listTEMP]  = Q15(1);
	adcGain[listIDC]   = 2500;
	adcGain[listVBAT1] = 2500;
	adcGain[listVDC1]  = Q15(1);
	adcGain[listVDC2]  = Q15(1);
	adcGain[listIDC2]  = 15000;
	adcGain[listIDC2 + 1]  = 4000;
	adcGain[listIDC2 + 2]  = 3300;
	adcGain[listIDC2 + 3]  = 2575;

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1,
    LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA));
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)adc1Buffer);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADC1_CHANNEL_COUNT);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1); // opsiyonel: NVIC enable da yap
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    LL_ADC_Enable(ADC1);
    for (volatile int i = 0; i < 1000; i++) { __asm volatile("nop"); } // kısa gecikme

    LL_ADC_REG_StartConversionSWStart(ADC1);
}

void calculationTemp(uint16_t adcValue)
{
	if (adcValue < 3124)
	{
		temp = ((adcValue * M1) >>15) + C1;
	}
	else if ( adcValue < 3289)
	{
		temp = ((adcValue * M2) >>15) + C2;
	}
	else if (adcValue < 3545)
	{
		temp = ((adcValue * M3) >>15) + C3;
	}
	else if (adcValue < 3786)
	{
		temp = ((adcValue * M4) >>15) + C4;
	}
	else
	{
		temp = ((adcValue * M5) >>15) + C5;
	}
}
