#include "adc.h"
#include "main.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#define DR_ADDRESS      ((uint32_t)0x4001244C)
/* 存放ADC为12位模数转换器结果的变量，只有ADCConvertedValue的低12位有效 */
__IO uint16_t ADCConvertedValue;     // 


u16 AD_DATA[2];//AD转换的数字量
float AD_ANALOG[2];//AD转换的模拟量





void delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

void ADCinit()                    //adc初始化
{
	/*ADC_GPIO管脚初始化*/
	ADC_GPIO_Configuration();

	/*ADC初始化配置*/
	SZ_STM32_ADC_Configuration(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_DATA,2);
}

void AdcRun(void)
{
	u8 i;
    for(i=0;i<3;i++)
    AD_ANALOG[i] =(float) AD_DATA[i]*(3.3/4095);//数字量转化成模拟量
}

float showLighdata()              //显示光照
{
	  return AD_ANALOG[0];
}

float showSoildata()              //显示土壤
{
	 return AD_ANALOG[1];
}




void ADC_GPIO_Configuration(void)//adc管脚初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOC时钟      */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /*PC0 作为模拟通道11输入引脚 */                       
   // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;       //管脚0
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   //输入模式
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
}


u8 DMA1_MEM_LEN;
void SZ_STM32_ADC_Configuration(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	 DMA_InitTypeDef DMA_InitStructure;
	 ADC_InitTypeDef ADC_InitStructure; 

 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA时钟
	
    DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值

	DMA1_MEM_LEN=cndtr;//通道数据长度
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设发送到内存  DMA_CCRX位4
	DMA_InitStructure.DMA_BufferSize = 2;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //内存数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	 
	 DMA_Cmd(DMA1_Channel1, ENABLE);//启动DMA通道 	

    /* 对ADC进行配置*/
   //ADC_InitTypeDef ADC_InitStructure; 
  //GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1,ENABLE );//使能端口1的时钟和ADC1的时钟，因为ADC1的通道1在PA1上

  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12M,ADC最大时间不能超过14M，也就是ADC的时钟频率为12MHz
  
  //PAx 作为模拟通道输入引脚                         
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//本次实验使用的是ADC1，并ADC1工作在独立模式ADC_CR1的位19:16,即这几位为0000
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//ADC_ScanConvMode 用来设置是否开启扫描模式，本实验开启扫面模式.ADC_CR1的位8
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ADC_ContinuousConvMode 用来设置是否开启连续转换模式 模数转换工作在连续转换模式，ADC_CR2的位1
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动 ADC_CR2的位19:17
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐ADC_CR2的位11
  ADC_InitStructure.ADC_NbrOfChannel = 2;	//顺序进行规则转换的ADC通道的数目ADC_SQR1位23:20
  ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );//ADC1；ADC1通道0；第1转换；采样时间为239.5周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5 );//ADC1；ADC1通道1；第2转换；采样时间为239.5周期
 // ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5 );//ADC1；ADC1通道3；第3转换；采样时间为239.5周期

  ADC_DMACmd(ADC1, ENABLE); //使能ADC1的DMA传输，ADC_CR2位8
    
  ADC_Cmd(ADC1, ENABLE);	//使能的ADC1,ADC_CR2位0
	
  ADC_ResetCalibration(ADC1);	//使能复位校准，ADC_CR2位3  
  while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
  ADC_StartCalibration(ADC1);	 //开启AD校准，ADC_CR2位2
  while(ADC_GetCalibrationStatus(ADC1))
	{};		//等待校准结束

   ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件启动AD转换

}
uint16_t GetADCConvertedValue(void)
{
    return ADCConvertedValue;
}




