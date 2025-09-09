/*
 * adc.h
 *
 *  Created on: Sep 9, 2025
 *      Author: Ziya
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#define Q15(x) ((int16_t)((x) < -1.0 ? -32768 : ((x) > 1.0 ? 32767 : (x) * 32768.0)))

enum {
	listVAC = 0,
	listTEMP,
	listIDC,
	listVBAT1,
	listVDC1,
	listVDC2,
	listIDC2,
	ENUM_ADC_CHANNEL_COUNT
};

#define ADC1_CHANNEL_COUNT ENUM_ADC_CHANNEL_COUNT

#include "main.h"

extern void adc_init();

extern q15_t adc1Buffer[ADC1_CHANNEL_COUNT];
extern int16_t adcBuffer[ADC1_CHANNEL_COUNT];
extern int16_t adcGain[ADC1_CHANNEL_COUNT];

#endif /* INC_ADC_H_ */
