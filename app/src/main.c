
#include "stm32f10x.h"
#include "os.h"
#include <stdint.h>
#include "serial.h"
#include "led.h"
#include "servo.h"
#include "datapack.h"
#include "i2c.h"

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

long X1,X2,B5,B6,X1,X2,X3,B3,B7,p;
unsigned long B4;

uint32_t T;

void bmp085_task()
{
    uint16_t UT;
    uint32_t UP;
    for (; ; )
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
        }else
        {
            p=(B7/B4)*2;
        }

        X1=(p>>8)*(p>>8);
        X1=(X1*3038)>>16;
        X2=(-7375*p)>>16;
        p=p+((X1+X2+3791)>>4);
    }
}

void communicate_task ()
{
    uint8_t data[8] = {0xa5,0,0};
    for(;;)
    {
        data[2] = (uint8_t)T;
        data[3] = (uint8_t)(T>>8);
        data[4] = ((uint8_t*)&p)[0];
        data[5] = ((uint8_t*)&p)[1];
        data[6] = ((uint8_t*)&p)[2];
        data[7] = ((uint8_t*)&p)[3];
        data[1] = 0xa5+data[2]+data[3] +data[4] +data[5]+data[6]+data[7];
        serial_write(data,8);

        vTaskDelay(100);
    }
}

void main (void)
{
    SysTick_Config (72000);

    led_init ();
    led_blinker (100);
    serial_init ();
    servo_init ();
    i2c_init();

    xTaskCreate ((TaskFunction_t) bmp085_task, "communication", 200, NULL, 6, NULL);
    xTaskCreate ((TaskFunction_t) communicate_task, "communication", 100, NULL, 6, NULL);

    vTaskStartScheduler ();
    for (;;);
}
