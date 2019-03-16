#ifndef __ADC_SCANNER_H
#define __ADC_SCANNER_H

#include "stm32f10x.h"
#include "board_defs.h"

void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);
static void adc_init();
static void DMA_Configuration(void);

void scanner_init();
inline void trigger_scan();

void timer1_init();


#endif