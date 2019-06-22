
#include "stm32f10x.h"
#include "os.h"
#include <stdint.h>
#include "serial.h"
#include "led.h"
#include "servo.h"
#include "datapack.h"
#include "i2c.h"
#include "bmp085.h"

void communicate_task ()
{
    uint8_t data[8] = {0xa5,0,0};
    uint32_t T;
    long     p;
    for(;;)
    {
        T = bmp085_getTemperature();
        p = bmp085_getPressure();

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
    //serial_init ();
    //servo_init ();
    //i2c_init();

    //xTaskCreate ((TaskFunction_t) bmp085_task, "communication", 200, NULL, 6, NULL);
    //xTaskCreate ((TaskFunction_t) communicate_task, "communication", 100, NULL, 6, NULL);

    vTaskStartScheduler ();
    for (;;);
}
