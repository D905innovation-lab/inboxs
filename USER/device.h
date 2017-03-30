#ifndef __DEVICE_H_
#define __DEVICE_H_

#include "main.h"


OS_ERR CollectTask_Create(void);
void Collect_Task(void *p_arg);
void Device_Run(void);
void CheckDecvice_Task(void *p_arg);
OS_ERR CheckDeviceTaskCreate(void);
OS_ERR CameraTask_Create(void);
void Camera_Task(void *p_arg);
OS_ERR GetSetDataTask_Create(void);
void GetSetData_Task(void *p_arg);
void Device_Init(void);
uint8_t Read_waterstate(void);


#define FAN_ON (GPIOC->BSRR  = GPIO_Pin_4)
#define FAN_OFF (GPIOC->BRR  = GPIO_Pin_4)

#define PUMP_ON (GPIOC->BSRR  = GPIO_Pin_5)
#define PUMP_OFF (GPIOC->BRR  = GPIO_Pin_5)

#define DUMI_ON		(GPIOB->BSRR  = GPIO_Pin_14)
#define DUMI_OFF	(GPIOB->BRR  = GPIO_Pin_14)



typedef struct _param_t
{
	uint8_t		soil_state;
	uint8_t 	light_state;
	uint8_t 	temperature;
	uint8_t		humidity;
	uint8_t		water_state;
	uint8_t 	WaterPumpSwitch;
	uint8_t 	LedSwitch;
	uint8_t     FanSwitch;
	uint8_t 	PhotoSwitch;
	uint8_t     PumbWebFlag;     
}SYS_PARAM;

typedef struct _set_t
{
	uint8_t		soil_set;
	uint8_t 	light_set;
	uint8_t 	humidity_set; 
}SYS_SET;


extern uint16_t TimeoutTmrCount;

#endif



























