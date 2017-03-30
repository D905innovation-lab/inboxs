#ifndef __USART_H_
#define __USART_H_
#include <stdio.h>
#include "main.h"
void Usart1_Init(void);
void Usart2_Init(void);
void Usart3_Init(void);
void NVIC_Configuration( void );
int fputc(int ch, FILE *f);
char *itoa( int value, char *string, int radix );
void USART2_printf( USART_TypeDef* USARTx, char *Data, ... );
void UART3Write(u8 *data,u32 len);
void uart3_putchar(u8 ch);
u16 UART3_Receiver_buf(u8 *data,u32 len,u32 time);




#define     PC_Usart( fmt, ... )           USART2_printf ( USART2, fmt, ##__VA_ARGS__ ) 

#define  second	 1000000

#define  L_TIME  (1*second)
#define  Z_TIME  (10*second)
#define  HH_TIME (90*second)
#define  H_TIME  (30*second) 
#endif

