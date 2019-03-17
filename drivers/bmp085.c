#include "bmp085.h"
#include "os.h"
#include "i2c.h"

static long X1,X2,B5,B6,X1,X2,X3,B3,B7,p;
static unsigned long B4;
static uint32_t T;

void bmp085_task()
{
    uint16_t UT;
    uint32_t UP;

    for(;;)
    {
        i2c_write_bmp085(0xee,0xf4,0x2e);
        vTaskDelay(5);

        i2c_read_dma(0xee,0xf6,2,(uint8_t*)&UT);
        UT=(uint16_t)(UT>>8) + (uint16_t)(UT<<8);

        X1 = (UT-AC6)*AC5>>15;
        X2= (MC<<11)/(X1+MD);
        B5=X1+X2;
        T=(B5+8)>>4;

        i2c_write_bmp085(0xee,0xf4,0x34);
        vTaskDelay(5);  //delay 5 ticks, about 5 ms.

        i2c_read_dma(0xee,0xf6,3,(uint8_t*)&UP);

        UP=((((uint8_t*)&UP)[0]<<16)+(((uint8_t*)&UP)[1]<<8)+((uint8_t*)&UP)[2])>>8;

        B6=B5-4000;
        X1=(B2*(B6*B6>>12))>>11;
        X2=AC2*B6>>11;
        X3=X1+X2;
        B3=(AC1*4+X3+2)/4;
        X1=AC3*B6>>13;
        X2=(B1*(B6*B6>>12))>>16;
        X3=((X1+X2)+2)>>2;
        B4=AC4*(unsigned long)(X3+32768)>>15;
        B7=((unsigned long)UP-B3)*50000;
        if(B7<0x80000000)
        {
            p=(B7*2)/B4;
        }
        else
        {
            p=(B7/B4)*2;
        }

        X1=(p>>8)*(p>>8);
        X1=(X1*3038)>>16;
        X2=(-7375*p)>>16;
        p=p+((X1+X2+3791)>>4);
    }
}

uint32_t bmp085_getTemperature()
{
    return T;
}

long bmp085_getPressure()
{
    return p;
}