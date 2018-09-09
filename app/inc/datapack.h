#ifndef __DATA_PACK_H
#define __DATA_PACK_H

#include <stdint.h>

typedef struct{
    uint8_t header;
    uint8_t size;
    uint8_t chksum;
	uint8_t type;
}frame_header_t;

typedef enum{
ack=0
}pack_type_t;

typedef struct{
	uint8_t srcaddr:4;	//source address
	uint8_t dstaddr:4;	//destination address
}route_t;

typedef struct{
	route_t route;
	uint8_t val;
}pack_t;

#define PACK_DAT	pack_t

typedef struct{
frame_header_t header;
PACK_DAT	pack;
}frame_t;

void pack_init(pack_t *pack);
void chksum(pack_t *pack);
int32_t pack_send(int32_t (*write)(uint8_t*,int32_t),pack_t *packptr);
int32_t pack_read(int32_t (*read)(uint8_t*,int32_t));

#endif
