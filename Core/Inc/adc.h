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
#define ADC_MEAN_CHANNEL_COUNT 6
#define ADC_RMS_CHANNEL_COUNT 1

#define ADC_MEAN_BUFFER_SIZE 64
#define ADC_MEAN_SAMPLE_SIZE 64

#define ADC_RMS_BUFFER_SIZE 20

#define SAMPLE_2N 6

#define N_VALUE 18

#define M1	Q15(0.04004)
#define C1	-94

#define M2	Q15(0.021)
#define C2	-31

#define M3	Q15(0.05)
#define C3	Q15(-121)

#define M4	Q15(0.04)
#define C4	-75

#define M5	Q15(0.03)
#define C5	-25

#include "main.h"

extern void adc_init();
extern void calculationTemp(uint16_t adcValue);

extern q15_t adc1Buffer[ADC1_CHANNEL_COUNT];
extern int16_t adcBuffer[ADC1_CHANNEL_COUNT];
extern int16_t adcGain[ADC1_CHANNEL_COUNT + 3];

extern q15_t adcMeanBuffer[ADC1_CHANNEL_COUNT][ADC_MEAN_BUFFER_SIZE];
extern int32_t adcMeanSum[ADC_MEAN_CHANNEL_COUNT];
extern uint8_t adcMeanBufferPo;

extern int32_t adcRmsBuffer[ADC_RMS_BUFFER_SIZE];
extern int64_t adcRmsSum;
extern uint8_t adcRmsBufferPo;

extern uint16_t adcVAC;
extern uint16_t adcTEMP;
extern uint16_t adcIDC;
extern uint16_t adcVBAT1;
extern uint16_t adcVDC1;
extern uint16_t adcVDC2;
extern uint16_t adcIDC2NoGain;
extern uint16_t adcIDC2;
extern uint8_t temp;

#endif /* INC_ADC_H_ */
