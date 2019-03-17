#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "os.h"

void i2c_reset();

void i2c_init();
void i2c_write(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data);
uint8_t i2c_read();

static SemaphoreHandle_t rxsemaphore = NULL;
int i2c_read_dma(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data);
int i2c_write_dma(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data);
void i2c_write_bmp085(uint32_t chipaddr,uint32_t memaddr,uint8_t data);
#endif

