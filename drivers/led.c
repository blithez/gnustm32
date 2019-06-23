#include "led.h"
#include "os.h"

void led_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void blink_task(void *arg)
{
    uint8_t tmp = 0;

    for(;;)
    {
        tmp = !tmp;

        vTaskDelay((uint32_t)arg);

        if (tmp == 0)
            GPIO_SetBits(GPIOC, GPIO_Pin_13);
        else
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        }
    }
}


void led_blinker(uint32_t ms)
{
    xTaskCreate((TaskFunction_t) blink_task, "blinker", 64, (void*)ms, 1,NULL);
}
