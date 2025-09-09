/*
 * adc.c
 *
 *  Created on: Sep 9, 2025
 *      Author: Ziya
 */

#include "adc.h"

q15_t adc1Buffer[ADC1_CHANNEL_COUNT];
int16_t adcBuffer[ADC1_CHANNEL_COUNT];

int16_t adcGain[ADC1_CHANNEL_COUNT];

/*
void adc_init()
{
	LL_ADC_Enable(ADC1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	LL_ADC_REG_StartConversionSWStart(ADC1);
	LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1,LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA));
	LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t) adc1Buffer);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADC1_CHANNEL_COUNT);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	LL_ADC_Enable(ADC1);

    while(!LL_ADC_IsEnabled(ADC1))
    {
        asm("NOP");
    }
}
*/

void adc_init(void)
{
	adcGain[listVAC]   = Q15(1);
	adcGain[listTEMP]  = Q15(1);
	adcGain[listIDC]   = Q15(1);
	adcGain[listVBAT1] = 2490;
	adcGain[listVDC1]  = Q15(1);
	adcGain[listVDC2]  = Q15(1);
	adcGain[listIDC2]  = Q15(1);

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

    // Başlat
    LL_ADC_REG_StartConversionSWStart(ADC1);
}
