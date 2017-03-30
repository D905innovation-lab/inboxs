#ifndef 	__DELAY_H_
#define 	__DELAY_H_


#include "main.h"

void delay_osschedlock(void);
void delay_osschedunlock(void);
void delay_ostimedly(u32 ticks);
void SysTick_Handler(void);
void delay_init(void);
void delay_us(u32 nus);
void delay_ms(u16 nms);

#endif



