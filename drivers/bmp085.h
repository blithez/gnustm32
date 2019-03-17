#ifndef __BMP085_H_
#define __BMP085_H_

#include <stdint.h>

#define AC1 ((int16_t)0x1a47)
#define AC2 ((int16_t)0xfba0)
#define AC3 ((int16_t)0xc780)
#define AC4 ((uint16_t)0x8745)
#define AC5 ((uint16_t)0x6363)
#define AC6 ((uint16_t)0x4a68)
#define B1 ((int16_t)0x157a)
#define B2 ((int16_t)0x0036)
#define MB ((int16_t)0x8000)
#define MC ((int16_t)0xd4bd)
#define MD ((int16_t)0x0980)

void     bmp085_task();
uint32_t bmp085_getTemperature();
long     bmp085_getPressure();

#endif