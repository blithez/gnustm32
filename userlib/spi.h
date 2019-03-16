#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#include <stdint.h>

//PORT A
#define SPI1_CS GPIO_Pin_4
#define SPI1_SCK GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7

//PORT B
#define SPI2_CS GPIO_Pin_12
#define SPI2_SCK GPIO_Pin_13
#define SPI2_MISO GPIO_Pin_14
#define SPI2_MOSI GPIO_Pin_15

#define DATA_SIZE 128   //byts

void spi1_init();
void spi2_init();


#endif