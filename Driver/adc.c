#include "adc.h"
#include "main.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#define DR_ADDRESS      ((uint32_t)0x4001244C)
/* ���ADCΪ12λģ��ת��������ı�����ֻ��ADCConvertedValue�ĵ�12λ��Ч */
__IO uint16_t ADCConvertedValue;     // 


u16 AD_DATA[2];//ADת����������
float AD_ANALOG[2];//ADת����ģ����





void delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

void ADCinit()                    //adc��ʼ��
{
	/*ADC_GPIO�ܽų�ʼ��*/
	ADC_GPIO_Configuration();

	/*ADC��ʼ������*/
	SZ_STM32_ADC_Configuration(DMA1_Channel1,(u32)&ADC1->DR,(u32)&AD_DATA,2);
}

void AdcRun(void)
{
	u8 i;
    for(i=0;i<3;i++)
    AD_ANALOG[i] =(float) AD_DATA[i]*(3.3/4095);//������ת����ģ����
}

float showLighdata()              //��ʾ����
{
	  return AD_ANALOG[0];
}

float showSoildata()              //��ʾ����
{
	 return AD_ANALOG[1];
}




void ADC_GPIO_Configuration(void)//adc�ܽų�ʼ��
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��GPIOCʱ��      */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /*PC0 ��Ϊģ��ͨ��11�������� */                       
   // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;       //�ܽ�0
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   //����ģʽ
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
}


u8 DMA1_MEM_LEN;
void SZ_STM32_ADC_Configuration(DMA_Channel_TypeDef* DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	 DMA_InitTypeDef DMA_InitStructure;
	 ADC_InitTypeDef ADC_InitStructure; 

 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMAʱ��
	
    DMA_DeInit(DMA_CHx);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

	DMA1_MEM_LEN=cndtr;//ͨ�����ݳ���
	DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;  //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = cmar;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴����跢�͵��ڴ�  DMA_CCRXλ4
	DMA_InitStructure.DMA_BufferSize = 2;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //�������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //�ڴ����ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA_CHx, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	 
	 DMA_Cmd(DMA1_Channel1, ENABLE);//����DMAͨ�� 	

    /* ��ADC��������*/
   //ADC_InitTypeDef ADC_InitStructure; 
  //GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1,ENABLE );//ʹ�ܶ˿�1��ʱ�Ӻ�ADC1��ʱ�ӣ���ΪADC1��ͨ��1��PA1��

  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12M,ADC���ʱ�䲻�ܳ���14M��Ҳ����ADC��ʱ��Ƶ��Ϊ12MHz
  
  //PAx ��Ϊģ��ͨ����������                         
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_3;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ʵ��ʹ�õ���ADC1����ADC1�����ڶ���ģʽADC_CR1��λ19:16,���⼸λΪ0000
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//ADC_ScanConvMode ���������Ƿ���ɨ��ģʽ����ʵ�鿪��ɨ��ģʽ.ADC_CR1��λ8
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ADC_ContinuousConvMode ���������Ƿ�������ת��ģʽ ģ��ת������������ת��ģʽ��ADC_CR2��λ1
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ�������� ADC_CR2��λ19:17
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���ADC_CR2��λ11
  ADC_InitStructure.ADC_NbrOfChannel = 2;	//˳����й���ת����ADCͨ������ĿADC_SQR1λ23:20
  ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );//ADC1��ADC1ͨ��0����1ת��������ʱ��Ϊ239.5����
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5 );//ADC1��ADC1ͨ��1����2ת��������ʱ��Ϊ239.5����
 // ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5 );//ADC1��ADC1ͨ��3����3ת��������ʱ��Ϊ239.5����

  ADC_DMACmd(ADC1, ENABLE); //ʹ��ADC1��DMA���䣬ADC_CR2λ8
    
  ADC_Cmd(ADC1, ENABLE);	//ʹ�ܵ�ADC1,ADC_CR2λ0
	
  ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼��ADC_CR2λ3  
  while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
  ADC_StartCalibration(ADC1);	 //����ADУ׼��ADC_CR2λ2
  while(ADC_GetCalibrationStatus(ADC1))
	{};		//�ȴ�У׼����

   ADC_SoftwareStartConvCmd(ADC1, ENABLE);//�������ADת��

}
uint16_t GetADCConvertedValue(void)
{
    return ADCConvertedValue;
}




