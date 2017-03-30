#ifndef __WEBSOCKET_H_
#define __WEBSOCKET_H_

#include "main.h"

OS_ERR WebPostTask_Create(void);
void WebPost_Task(void *p_arg);


typedef struct WebSocket
{
	uint8_t WebSocketData_Buf[1024];
	uint8_t WebSocketData_FinishFlag;
	uint16_t WebSocketData_Length;
}WebData;


//请求内容标志位
#define DEVICE_DATA_POST_FLAG       (1<<0)
#define PHOTO_POST_FLAG 		    (1<<1)
#define DEVICE_SWITCH_POST_FLAG	    (1<<2)
#define DEVICE_SET_GET_FLAG	   		(1<<3)



#endif
















