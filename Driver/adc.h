#ifndef _ADC_H_
#define _ADC_H_


#include "stm32f10x.h"
#include <stdio.h>

void ADC_GPIO_Configuration(void);
void SZ_STM32_ADC_Configuration(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);
uint16_t GetADCConvertedValue(void);
float showLighdata(void);
float showSoildata(void);
void ADCinit(void);
void AdcRun(void);

#endif
