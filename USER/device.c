#include "device.h"
#include "wifi.h"
#include "main.h"

SYS_PARAM DeviceList;
SYS_SET DeviceSet;

extern WebData WebSocketData;
extern OS_Q POSTDATA_Msg;
extern uint8_t temperature,humidity;
extern uint8_t PostSensorBuf[135],PostSwitchBuf[45];
extern uint8_t content_length;

uint8_t SwitchMsg = DEVICE_SWITCH_POST_FLAG;
uint8_t SensorMsg = DEVICE_DATA_POST_FLAG;
uint8_t PhotoMsg  = PHOTO_POST_FLAG;
uint8_t GetMsg    = DEVICE_SET_GET_FLAG;



//采集传感器数据任务

#define COLLECT_TASK_PRIO		5	
#define COLLECT_STK_SIZE 		128
OS_TCB CollectTaskTCB;
CPU_STK COLLECT_TASK_STK[COLLECT_STK_SIZE];

//检查设备状态任务

#define CHECK_DATA_TASK_PRIO	8	
#define CHECK_DATA_STK_SIZE 	128
OS_TCB CheckDataTaskTCB;
CPU_STK CHECK_DATA_TASK_STK[CHECK_DATA_STK_SIZE];

//拍照任务

#define CAMERA_TASK_PRIO	4	
#define CAMERA_STK_SIZE 	128
OS_TCB CameraTaskTCB;
CPU_STK CAMERA_TASK_STK[CAMERA_STK_SIZE];

//获取设定值任务

#define GET_SETDATA_TASK_PRIO	6	
#define GET_SETDATA_STK_SIZE 	128
OS_TCB  GetSetDataTaskTCB;
CPU_STK GET_SETDATA_TASK_STK[GET_SETDATA_STK_SIZE];




uint8_t PumbTime;


/**
		 * 设备初始化
		 * 
		 * @author hys
		 * @param Device_Init
		 * @parameter (void)
		 * @return 	
		 
	 */
void Device_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//RBG_LED	output
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//Fan Pump 	 output
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//WT0~WT2	input
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//SOIL		input
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//LUMI 		input
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//DUMI		output
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//default  set   high;	
	GPIOA->BSRR =GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	//default  set   down;
	GPIOC->BRR =GPIO_Pin_4|GPIO_Pin_5;
	//default  set   down
	GPIOB->BRR  = GPIO_Pin_14;
	
	DeviceList.PumbWebFlag = 1;
	DeviceSet.humidity_set = 20;
	DeviceSet.light_set = 10;
	DeviceSet.soil_set = 20;
}



/**
		 * 读取水位状态
		 * 
		 * @author hys
		 * @param Read_waterstate
		 * @parameter (void)
		 * @return 	uint8_t
		 
	 */
uint8_t Read_waterstate(void)
{	
	uint8_t Waterstate = 0;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)!= 1)
		Waterstate = 1;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)!= 1)
		Waterstate = 2;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)!= 1)
		Waterstate = 3;
	return Waterstate;		
}



/**
		 * 创建传感器数据采集任务
		 * 
		 * @author hys
		 * @param CollectTask_Create
		 * @parameter (void)
		 * @return 	OS_ERR
		 
	 */
OS_ERR CollectTask_Create(void)
{
	OS_ERR err;
	OSTaskCreate(
		(OS_TCB *)&CollectTaskTCB, 
		(CPU_CHAR *)"collect_task", 
		(OS_TASK_PTR)Collect_Task, 
		(void *)0, 
		(OS_PRIO)COLLECT_TASK_PRIO, 
		(CPU_STK*)COLLECT_TASK_STK, 
		(CPU_STK_SIZE)COLLECT_STK_SIZE/10, 
		(CPU_STK_SIZE)COLLECT_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)err);
	return err;
}


/**
		 * 创建设备状态检查任务
		 * 
		 * @author hys
		 * @param CheckDeviceTaskCreate
		 * @parameter (void)
		 * @return 	OS_ERR
	 * 
	 */
OS_ERR CheckDeviceTaskCreate(void)
{
	OS_ERR err;
	OSTaskCreate(
		(OS_TCB *)&CheckDataTaskTCB, 
		(CPU_CHAR *)"checkdata_task", 
		(OS_TASK_PTR)CheckDecvice_Task, 
		(void *)0, 
		(OS_PRIO)CHECK_DATA_TASK_PRIO, 
		(CPU_STK*)CHECK_DATA_TASK_STK, 
		(CPU_STK_SIZE)CHECK_DATA_STK_SIZE/10, 
		(CPU_STK_SIZE)CHECK_DATA_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)err);
	return err;
}

/**
		 * 创建拍照任务
		 * 
		 * @author hys
		 * @param CheckDeviceTaskCreate
		 * @parameter (void)
		 * @return 	OS_ERR
	 * 
	 */
OS_ERR CameraTask_Create(void)
{
	OS_ERR err;
	OSTaskCreate(
		(OS_TCB *)&CameraTaskTCB, 
		(CPU_CHAR *)"Camera_task", 
		(OS_TASK_PTR)Camera_Task, 
		(void *)0, 
		(OS_PRIO)CAMERA_TASK_PRIO, 
		(CPU_STK*)CAMERA_TASK_STK, 
		(CPU_STK_SIZE)CAMERA_STK_SIZE/10, 
		(CPU_STK_SIZE)CAMERA_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)err);
	return err;
}


/**
		 * 创建获取设定值任务
		 * 
		 * @author hys
		 * @param CheckDeviceTaskCreate
		 * @parameter (void)
		 * @return 	OS_ERR
	 * 
	 */
OS_ERR GetSetDataTask_Create(void)
{
	OS_ERR err;
	OSTaskCreate(
		(OS_TCB *)&GetSetDataTaskTCB, 
		(CPU_CHAR *)"GetSetData_task", 
		(OS_TASK_PTR)GetSetData_Task, 
		(void *)0, 
		(OS_PRIO)GET_SETDATA_TASK_PRIO, 
		(CPU_STK*)GET_SETDATA_TASK_STK, 
		(CPU_STK_SIZE)GET_SETDATA_STK_SIZE/10, 
		(CPU_STK_SIZE)GET_SETDATA_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)err);
	return err;
}



/**
		 * 传感器采集任务
		 * 
		 * @author hys
		 * @param CheckDeviceTaskCreate
		 * @parameter (void *)p_arg
		 * @return 	void
	 * 
	 */
void Collect_Task(void *p_arg)
{
	OS_ERR err;
	uint8_t soil_data,light_data;
	uint8_t i;
	p_arg = p_arg;
	while(1)
	{	
		soil_data = showSoildata(); 
		light_data = showLighdata(); 
		AdcRun();
		DHT11_Read_Data(&temperature,&humidity);
		
		DeviceList.temperature = temperature;
		DeviceList.humidity = humidity;
		DeviceList.soil_state = soil_data/3.3*100;
		DeviceList.light_state = light_data/3.3*100;
		DeviceList.water_state = Read_waterstate();
		memset(PostSensorBuf,0,145);

		sprintf(PostSensorBuf,"{\"key\":\"inbox\",\"value\":{\"environment_temperature\":%d,\"environment_humidity\":%d,\"soil_humidity\":%d,\"water_level\":%d,\"light_intensity\":%d}}"
															,DeviceList.temperature,DeviceList.humidity,DeviceList.soil_state
															,DeviceList.light_state,DeviceList.water_state,DeviceList.light_state);
															
	
		for(i = 0;i < 165; i++)
		{
			if((PostSensorBuf[i] == 0x7D)&&(PostSensorBuf[i+1] == 0x7D))
			{
					content_length = i+2;
			}
		}
		OSQPost(&POSTDATA_Msg, (void *)&SensorMsg, 1, OS_OPT_POST_FIFO, &err);
		OSTimeDlyHMSM(0,3,0,0, OS_OPT_TIME_HMSM_NON_STRICT, &err);
	}	
}


/**
		 * 设备状态检查任务
		 * 
		 * @author hys
		 * @param CheckDecvice_Task
		 * @parameter (void *)p_arg
		 * @return 	
	 * 
	 */
void CheckDecvice_Task(void *p_arg)
{
	OS_ERR err;
	uint16_t i;
	uint8_t light[2],humd[2];
	p_arg = p_arg;
	while(1)
	{
		//add task...
		for(i = 0;i < 512;i++)
		{
			if((WebSocketData.WebSocketData_Buf[i] == 0x32)&&(WebSocketData.WebSocketData_Buf[i+1] == 0x63))
			{
				if(WebSocketData.WebSocketData_Buf[i+39] == 0x31)
				{
						DeviceList.LedSwitch = 1;
				}			
				else if(WebSocketData.WebSocketData_Buf[i+39] == 0x30)
				{
						DeviceList.LedSwitch = 0;
				}
				
				if(WebSocketData.WebSocketData_Buf[i+45] == 0x31)
				{
						DeviceList.WaterPumpSwitch = 1;
				}			
				else if(WebSocketData.WebSocketData_Buf[i+45] == 0x30)
				{
						DeviceList.WaterPumpSwitch = 0;
				}		
			}
			if((WebSocketData.WebSocketData_Buf[i] == 0x33)&&(WebSocketData.WebSocketData_Buf[i+1] == 0x35))
			{
				light[0] = WebSocketData.WebSocketData_Buf[i + 39] - '0';
				light[1] = WebSocketData.WebSocketData_Buf[i + 40] - '0';
				DeviceSet.light_set = light[0]*10 + light[1];
				humd[0] = WebSocketData.WebSocketData_Buf[i + 53] - '0';
				humd[1] = WebSocketData.WebSocketData_Buf[i + 54] - '0';
				DeviceSet.humidity_set= humd[0]*10 + humd[1];
			}
		}
		
		Device_Run();
		OSTimeDlyHMSM(0,0,0,200, OS_OPT_TIME_HMSM_STRICT, &err);
	}	
}


void Camera_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSQPost(&POSTDATA_Msg, (void *)&PhotoMsg, 1, OS_OPT_POST_FIFO, &err);
		OSTimeDlyHMSM(0,10,0,0, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}


void GetSetData_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSQPost(&POSTDATA_Msg, (void *)&GetMsg, 1, OS_OPT_POST_FIFO, &err);
		OSTimeDlyHMSM(0,5,0,0, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}



/**
		 * 设备运行
		 * 
		 * @author hys
		 * @param Device_Run
		 * @parameter (void)
		 * @return 	void
	 * 
	 */
void Device_Run(void)
{
	OS_ERR err;
	if(DeviceList.LedSwitch != 0)
	{
		DUMI_ON;
	}
	else if(DeviceList.LedSwitch == 0)
	{
		DUMI_OFF;
	}

	if(DeviceList.humidity > DeviceSet.humidity_set)
	{
		FAN_ON;
	}

	else if(DeviceList.humidity <= DeviceSet.humidity_set)
	{
		FAN_OFF;
	}
	
	if(PumbTime != 0)
	{
		PumbTime++;
		DeviceList.WaterPumpSwitch = 0;
	}
	
	if(((DeviceList.WaterPumpSwitch != 0)&&(DeviceList.PumbWebFlag != 0)) || (DeviceList.soil_state < DeviceSet.soil_set))
	{
		PUMP_ON;
		DeviceList.PumbWebFlag = 0;
		PumbTime = 1;
		memset(PostSwitchBuf,0,45);
		sprintf(PostSwitchBuf,"{\"key\":\"inbox\",\"value\":{\"light\":%d,\"pumb\":0}}\r\n\r\n"
															,DeviceList.LedSwitch);	
	}
		
	if(PumbTime == 15)
	{
		DeviceList.WaterPumpSwitch = 0;
		PUMP_OFF;
	}

	if(PumbTime == 70)
	{
		OSQPost(&POSTDATA_Msg, (void *)&SwitchMsg, 1, OS_OPT_POST_FIFO, &err);
	}
	
	if(PumbTime >= 80)
	{
		PumbTime = 0;
	}
	
}




















