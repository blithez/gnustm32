#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include <stdint.h>

void led_init(void);
void led_blinker(uint32_t ms);

#endif