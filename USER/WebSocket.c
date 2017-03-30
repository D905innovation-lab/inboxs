#include "main.h"
#include "WebSocket.h"
#include "photodata.h"

const uint8_t GetSwitchData[] = "GET http://api.yeelink.net/v1.0/device/354593/sensor/400901/datapoints HTTP/1.1\r\nHost: api.yeelink.net\r\nU-ApiKey: 0ad358217706ef3af6cbe7833a1835ba\r\n\r\n";
const uint8_t GetSetData[] = "GET http://api.yeelink.net/v1.0/device/354593/sensor/402896/datapoints HTTP/1.1\r\nHost: api.yeelink.net\r\nU-ApiKey: 0ad358217706ef3af6cbe7833a1835ba\r\n\r\n";
uint8_t PostSwitchData[] = "POST http://api.yeelink.net/v1.0/device/354593/sensor/400901/datapoints HTTP/1.1\r\nHost: api.yeelink.net\r\nContent-Length: 44\r\nU-ApiKey: 0ad358217706ef3af6cbe7833a1835ba\r\n\r\n";
uint8_t PostSensorData[] = "POST http://api.yeelink.net/v1.0/device/354593/sensor/400693/datapoints HTTP/1.1\r\nHost: api.yeelink.net\r\nContent-Length: 133\r\nU-ApiKey: 0ad358217706ef3af6cbe7833a1835ba\r\n\r\n";
uint8_t PostPhotoData[175] = "POST http://api.yeelink.net/v1.0/device/354593/sensor/400698/photos HTTP/1.1\r\nHost: api.yeelink.net\r\nContent-Length: 50000\r\nU-ApiKey: 0ad358217706ef3af6cbe7833a1835ba\r\n\r\n";



uint8_t content_length;

extern OS_Q POSTDATA_Msg;
extern SYS_PARAM DeviceList; 
extern uint8_t photodata[31000];
extern uint16_t remb;

WebData WebSocketData = {0};
uint8_t PostSensorBuf[165] = {0};
uint8_t PostSwitchBuf[65] = {0};


#define WEBPOST_TASK_PRIO		7	
#define WEBPOST_STK_SIZE 		128
OS_TCB WebPostTaskTCB;
CPU_STK WEBPOST_TASK_STK[WEBPOST_STK_SIZE];


/**
		 * 创建Web请求任务
		 * 
		 * @author hys
		 * @param WebPostTaskCreate
		 * @parameter (void)
		 * @return 	OS_ERR
	 * 
	 */
OS_ERR WebPostTask_Create(void)
{
	OS_ERR err;
	OSTaskCreate(
		(OS_TCB *)&WebPostTaskTCB, 
		(CPU_CHAR *)"webpost_task", 
		(OS_TASK_PTR)WebPost_Task, 
		(void *)0, 
		(OS_PRIO)WEBPOST_TASK_PRIO, 
		(CPU_STK*)WEBPOST_TASK_STK, 
		(CPU_STK_SIZE)WEBPOST_STK_SIZE/10, 
		(CPU_STK_SIZE)WEBPOST_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)err);
	
	return err;
}





/**
		 * Web请求任务
		 * 
		 * @author hys
		 * @param WebPost_Task
		 * @parameter (void *)p_arg
		 * @return 	void
	 * 
	 */

void WebPost_Task(void *p_arg)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	uint8_t *Msg;
	CPU_SR_ALLOC();
	uint16_t i;
	uint8_t length[5] = {0};
	uint8_t	temp_length;
	p_arg = p_arg;
	while(1)
	{
		Msg = OSQPend(
				(OS_Q *)&POSTDATA_Msg, 
				(OS_TICK)0, 
				(OS_OPT)OS_OPT_PEND_NON_BLOCKING, 
				(OS_MSG_SIZE *)&size, 
				(CPU_TS *)0, 
				(OS_ERR *)&err);
		PC_Usart("%s",Msg);
		switch(*Msg)
		{
			case DEVICE_DATA_POST_FLAG:
				WebSocketData.WebSocketData_Length = 0;
				memset(WebSocketData.WebSocketData_Buf,0,1024);
				length[0] = (content_length / 100) + '0';
				length[1] = (content_length / 10 % 10) + '0';
				length[2] = (content_length % 10) + '0';
				PostSensorData[121] = length[0];
				PostSensorData[122] = length[1];
				PostSensorData[123] = length[2];
   				printf("%s",PostSensorData);
				printf("%s",PostSensorBuf);
				break;
			case DEVICE_SWITCH_POST_FLAG:
				WebSocketData.WebSocketData_Length = 0;
				memset(WebSocketData.WebSocketData_Buf,0,1024);
				DeviceList.PumbWebFlag = 1;
   				printf("%s",PostSwitchData);
				printf("%s",PostSwitchBuf);
				break;
			case PHOTO_POST_FLAG:
				OS_CRITICAL_ENTER();
				
				WebSocketData.WebSocketData_Length = 0;
				memset(WebSocketData.WebSocketData_Buf,0,1024);		
				for(i = 0;i < 170;i++)
				{				
					photodata[i] = PostPhotoData[i];					
				}
				beginphoto();	
				length[0] = ((remb - 170) / 10000) + '0';
				length[1] = ((remb - 170) / 1000 % 10) + '0';
				length[2] = ((remb - 170) / 100 % 10) + '0';
				length[3] = ((remb - 170) / 10 % 10) + '0';
				length[4] = ((remb - 170) % 10 ) + '0';
				photodata[117] = length[0];
				photodata[118] = length[1];
				photodata[119] = length[2];
				photodata[120] = length[3];
				photodata[121] = length[4]; 
				for(i = 0;i < remb;i++)
				{
					
					USART_SendData(USART1,photodata[i]);
						while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
					
				}
				OS_CRITICAL_EXIT();
				break;
			case DEVICE_SET_GET_FLAG :
			printf("%s",GetSetData);
				
				break;
			default:
				WebSocketData.WebSocketData_Length = 0;
				memset(WebSocketData.WebSocketData_Buf,0,1024);
				printf("%s",GetSwitchData);		
				
		}
		OSTimeDlyHMSM(0,0,2,0, OS_OPT_TIME_HMSM_STRICT, &err);
	}	
}







