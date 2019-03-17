#include "serial.h"

void serial_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /*********************enable peripheral clock***********************/
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    /*********************configure serial port pin*********************/
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    /**********************configure serial*****************************/
    USART_InitStructure.USART_BaudRate = BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init (USART1, &USART_InitStructure);

    USART_Cmd (USART1, ENABLE);
    USART_ClearFlag (USART1, USART_FLAG_TC);

    /**********************configure serial idle interrupt************************/
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //smaller ,higher
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    /**********************configure DMA*******************************/
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = 0x0;
    DMA_InitStructure.DMA_MemoryBaseAddr = 0x0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);//tx

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)serial_rx_buffer;
    DMA_InitStructure.DMA_BufferSize = RX_BUFFER_SIZE;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);//rx

    DMA_Cmd(DMA1_Channel5, ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Rx, ENABLE);

    /**********************configure dma interrupt************************/
    /*NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //smaller ,higher
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);*/

    /********************task for copy data into queue buffer**************/
    rx_buffer = xStreamBufferCreate( RX_MAX_STREAM_SIZE,1 );
    xTaskCreate((TaskFunction_t) cx_copy_task, "rx_mover", RX_TASK_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, &cx_copy_task_id);
}

int32_t serial_write(uint8_t *src,int32_t len)
{
    if( USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET && DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET)
    {
        taskYIELD();
        return -1; //serial transmission is busy
    }

    if(len ==0 )
    {
        return 0;
    }else if(len == 1)
    {
        USART_SendData(USART1,*src);
    }else
    {//use DMA for transmissionS
        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA1_Channel4->CNDTR = len;  //dma buffersize
        DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;   //dma peripheral base address
        DMA1_Channel4->CMAR = (uint32_t)src;       //dma memory base addr
        DMA_Cmd(DMA1_Channel4, ENABLE);

        USART_DMACmd(USART1,USART_DMAReq_Tx, ENABLE);
    }

    return 1;
}

int32_t serial_read(uint8_t *src,int32_t len)
{
    if(src == NULL || len > RX_BUFFER_SIZE)
    {
        return -1;
    }

    //blocked call
    return xStreamBufferReceive( rx_buffer,src,len,portMAX_DELAY);
}

/*****************************interrupt handler****************************/

extern TaskHandle_t consumerid;

void DMA1_Channel5_IRQHandler()
{
    if(DMA_GetFlagStatus(DMA1_FLAG_TC5) == SET)
    {

    }
    DMA_ClearITPendingBit(DMA1_IT_TC5);
}

void USART1_IRQHandler()
{
    uint32_t temp;

    if(USART_GetITStatus(USART1,USART_IT_IDLE)== SET)
    {
        temp = USART1->SR;
        temp = USART1->DR;
        temp = temp;

        DMA_Cmd(DMA1_Channel5,DISABLE);
        available_data_size = RX_BUFFER_SIZE -  DMA_GetCurrDataCounter(DMA1_Channel5);
        DMA1_Channel5->CNDTR = RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel5,ENABLE);
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);

        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        vTaskNotifyGiveFromISR(cx_copy_task_id,&pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR( pxHigherPriorityTaskWoken );
    }
}

void cx_copy_task()
{
    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        xStreamBufferSend( rx_buffer, serial_rx_buffer, available_data_size,0 );
    }
}