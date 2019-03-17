#ifndef __SERIAL_H_
#define __SERIAL_H_

#include "stm32f10x.h"
#include <stdint.h>
#include "os.h"

/**************************************
max receive buffer is 20 bytes,which means
if peer side sends more than 20 bytes,only
20 bytes can be read.
****************************************/

typedef enum{
    SERIAL_1=0,SERIAL_2,SERIAL_3
}serial_no_t;

typedef struct
{
    serial_no_t sno;
    uint8_t world_len;
    uint8_t stopbits;
    uint8_t parity;
    uint8_t hardwareflowcontrol;
}serial_config_t;

#define BAUDRATE 9600
#define RX_BUFFER_SIZE 20
#define RX_MAX_STREAM_SIZE 100
#define RX_TASK_STACK_SIZE 120

static uint8_t serial_rx_buffer[RX_BUFFER_SIZE];
static uint32_t available_data_size;

/**************************global call function********************************/
void serial_init(void); //default serial initialization
int32_t serial_write(uint8_t *src,int32_t len);
int32_t serial_read(uint8_t *src,int32_t len);

/***************************private call function******************************/
static StreamBufferHandle_t rx_buffer;
static TaskHandle_t cx_copy_task_id;
static void cx_copy_task();

#endif