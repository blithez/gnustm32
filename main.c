#include <stdint.h>
#include "stm32f10x.h"
#include <stdio.h>
#include "os.h"
#include "led.h"
#include "i2c.h"

/*
#define AC1 (*(short*)&aa[4])
#define AC2 (*(short*)&aa[2])
#define AC3 (*(short*)&aa[0])
#define AC4 (*(uint16_t*)&bb[14])
#define AC5 (*(uint16_t*)&bb[12])
#define AC6 (*(uint16_t*)&bb[10])
#define B1 (*(short*)&bb[8])
#define B2 (*(short*)&bb[6])
#define MB (*(short*)&bb[4])
#define MC (*(short*)&bb[2])
#define MD (*(short*)&bb[0])
*/

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


#define _reverse_data(data,size) for(int i =0;i<size/2;i++)\
{data[i] = data[i]^data[size-i-1];\
data[size-i-1] = data[i] ^data[size-i-1];\
data[i] = data[i]^data[size-i-1];\
}

volatile long X1,X2,B5,T,B6,X1,X2,X3,B3,B7,p;
unsigned long B4;

void task(void *arg)
{
    uint16_t UT;
    uint32_t UP;

    led_init();

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
        vTaskDelay(5);
        
        
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
        
        
        vTaskDelay(70);
        GPIOC->ODR ^= 1<<13; 
        
    }
    
}

void main (void)
{
    SysTick_Config(72000);
    
    i2c_init();
    I2C_SoftwareResetCmd(I2C1,ENABLE);
    i2c_reset();
    i2c_init();
    
    
    xTaskCreate( task, "LCD", 500, NULL, 2, NULL );
    vTaskStartScheduler();
    
    for(;;);
}



