#include "i2c.h"

void i2c_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

    I2C_DeInit(I2C1);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x03A2;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress =
        I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn | I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    I2C_ITConfig(I2C1, I2C_IT_ERR |I2C_IT_EVT, ENABLE);//I2C_IT_BUF |

    /************************config dma*******************************/
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&I2C1->DR;
    //DMA_InitStructure.DMA_MemoryBaseAddr = NULL;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 16;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel7, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

    rxsemaphore = xSemaphoreCreateCounting(1,0);
}



uint8_t i2c_read()
{
    u8 ReceivedData;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, 0xEE, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_Cmd(I2C1,ENABLE);

    I2C_SendData(I2C1, 0xaa);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, 0xEE, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    I2C_Cmd(I2C1,ENABLE);

    ReceivedData = I2C_ReceiveData(I2C1);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));

     ReceivedData = I2C_ReceiveData(I2C1);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));

    I2C_GenerateSTOP(I2C1, ENABLE);

    return ReceivedData;
}



int i2c_read_dma(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CNDTR = (uint32_t)len;
    DMA1_Channel7->CMAR = (uint32_t)data;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    /*****************************i2c process**********************************/

    taskENTER_CRITICAL();
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, chipaddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_Cmd(I2C1,ENABLE);

    I2C_SendData(I2C1, memaddr);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, chipaddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    DMA_Cmd(DMA1_Channel7, ENABLE);
    I2C_DMALastTransferCmd(I2C1, ENABLE);
    I2C_DMACmd(I2C1, ENABLE);

    taskEXIT_CRITICAL();
    /**************************************************************************/
    //block caller
    xSemaphoreTake(rxsemaphore,portMAX_DELAY);
    return 0;
}

void i2c_write(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data)
{
     while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, chipaddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_Cmd(I2C1,ENABLE);

    I2C_SendData(I2C1, memaddr);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

int i2c_write_dma(uint32_t chipaddr,uint32_t memaddr,uint32_t len,uint8_t *data)
{

return 0;

}

void i2c_write_bmp085(uint32_t chipaddr,uint32_t memaddr,uint8_t data)
{
     while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, chipaddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_Cmd(I2C1,ENABLE);

    I2C_SendData(I2C1, memaddr);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void i2c_reset()
{
    I2C_Cmd(I2C1, DISABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB,GPIO_Pin_7);
    GPIO_ResetBits(GPIOB,GPIO_Pin_6);       //scl clock
    GPIO_SetBits(GPIOB,GPIO_Pin_7);         //sda data
    GPIO_SetBits(GPIOB,GPIO_Pin_6);

    /******************gpio i2c mode****************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

void I2C1_EV_IRQHandler()
{
    i2c_reset();
    i2c_init();
    I2C_ClearITPendingBit(I2C1,I2C_IT_SB|I2C_IT_ADDR|I2C_IT_ADD10|I2C_IT_STOPF|I2C_IT_BTF);
}

void I2C1_ER_IRQHandler()
{
    i2c_reset();
    i2c_init();
    I2C_ClearITPendingBit(I2C1,I2C_IT_BERR | I2C_IT_ARLO |I2C_IT_AF|I2C_IT_OVR|I2C_IT_PECERR|I2C_IT_TIMEOUT|I2C_IT_SMBALERT);
}

void DMA1_Channel7_IRQHandler()
{
    BaseType_t takstowake = pdTRUE;

    if(DMA_GetFlagStatus(DMA1_FLAG_TC7))
    {
        DMA_ClearFlag(DMA1_FLAG_TC7);
        I2C_DMACmd(I2C1, DISABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC7);

        xSemaphoreGiveFromISR(rxsemaphore,&takstowake);
        portYIELD_FROM_ISR(takstowake);
    }
}
