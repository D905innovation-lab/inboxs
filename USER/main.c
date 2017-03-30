#include "main.h"
//Æô¶¯ÈÎÎñ
void Start_Task(void *p_arg);
#define START_TASK_PRIO		3	
#define START_STK_SIZE 		128
OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];

#define POST_Q_NUM 6
OS_Q POSTDATA_Msg;

extern WebData WebSocketData;


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	Usart1_Init();
	Usart2_Init();
	Usart3_Init();
	Device_Init();
	delay_init();
//	Wifi_Init();
	NVIC_Configuration();
	ADCinit();
	DHT11_Init();
	OSInit(&err);
	OS_CRITICAL_ENTER();
	OSQCreate(
		(OS_Q *)&POSTDATA_Msg, 
		(CPU_CHAR *)"PostData Msg", 
		(OS_MSG_QTY)POST_Q_NUM, 
		(OS_ERR *)&err);
	OSTaskCreate(
		(OS_TCB *)&StartTaskTCB, 
		(CPU_CHAR *)"start_task", 
		(OS_TASK_PTR)Start_Task, 
		(void *)0, 
		(OS_PRIO)START_TASK_PRIO, 
		(CPU_STK*)START_TASK_STK, 
		(CPU_STK_SIZE)START_STK_SIZE/10, 
		(CPU_STK_SIZE)START_STK_SIZE, 
		(OS_MSG_QTY)0, 
		(OS_TICK)0, 
		(void *)0, 
		(OS_OPT)OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
		(OS_ERR *)&err);
	OS_CRITICAL_EXIT();
	OSStart(&err);
	return 0;
}



void Start_Task(void * p_arg)
{
	
	OS_ERR err;  
	CPU_SR_ALLOC();
	p_arg = p_arg;
	CPU_Init();
	OS_CRITICAL_ENTER();
	GetSetDataTask_Create();
	CameraTask_Create();
	CheckDeviceTaskCreate();
	CollectTask_Create();
	WebPostTask_Create();
	OS_CRITICAL_EXIT();
	OSTaskDel(
		(OS_TCB *)0, 
		(OS_ERR *)&err);
}


