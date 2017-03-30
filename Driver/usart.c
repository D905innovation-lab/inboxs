#include "usart.h"
#include "main.h"


extern WebData WebSocketData;
void Usart1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

void Usart2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;               
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
}

void Usart3_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    /*??GPIO?--USART3*/
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
	
		
    USART_InitStructure.USART_BaudRate=38400;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Parity=USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE,DISABLE); 
    USART_ClearFlag(USART3,USART_FLAG_TC);
}

void NVIC_Configuration( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void USART1_IRQHandler(void)
{
	uint8_t ch_str;
	OSIntEnter();
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearFlag(USART1,USART_FLAG_RXNE); 
		ch_str  = USART_ReceiveData( USART1 );
		WebSocketData.WebSocketData_Buf[WebSocketData.WebSocketData_Length++] = ch_str;
	}

	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )                                         
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE);  
		USART_ReceiveData(USART1); 
		WebSocketData.WebSocketData_FinishFlag = 1;
		PC_Usart("%s\n",WebSocketData.WebSocketData_Buf);
  }
	OSIntExit();
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);
	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		

	return (ch);
}


char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

}

void USART2_printf( USART_TypeDef* USARTx, char *Data, ... )
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
														Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}

void UART3Write(u8 *data,u32 len)	 //发送n个字符
{	
    u16 i;
    if( len == 0)	
    {	
        for(i = 0;data[i] != '\0';i++)
        {
            uart3_putchar(data[i]);
        }
    }

    else
    {		
        for(i = 0;i < len;i++)
        {
            uart3_putchar(data[i]);
        }
    }

}

void uart3_putchar(u8 ch) //发送一个字符
{  
    USART_ClearFlag(USART3,USART_FLAG_TC);
    USART_SendData(USART3,ch);
    while( USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET );

}

u16 UART3_Receiver_buf(u8 *data,u32 len,u32 time)  
{
    u16 i=0;
    u32 time0=0;
    i = USART_ReceiveData(USART3);
    while((USART_GetFlagStatus(USART3,USART_FLAG_RXNE)) == RESET)
    {   
       // delay_us(1);
        time0++;
        if( time0 > time)
        {
            return 0;
        } 
    }
    data[0] = USART_ReceiveData(USART3);
    i = 1;
    time0 = 0;
    while(1)
    { 
        if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) != RESET)
        {
            time0 = 0;
            data[i] = USART_ReceiveData(USART3);
            i++;
            if( i == len && (len != 0) )
            { 
                break; 
            }
        }
        else 
        {
            if( time0 > 10000 )
                break;
            else 
                time0++;
        }
    }
    return i;
}


