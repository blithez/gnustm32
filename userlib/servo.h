#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f10x.h"

static void RCC_Configuration(void);
static void GPIO_Configuration(void);

void servo_init();
void servo_write(uint32_t value);

#endif