#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "dac_TriangleWave.h"
#include "dac.h"
#include "key.h"
/////////////////////////////////////
// ADCת��ʵ��
void GPIO_Configuration(void);
void RCC_Configuration(void);
void DMA_Configuration(void);
void ADC_Configuration(u8);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void Time_Configuration(void);
void ADC_Restart(u8 cysj);
///////////////////////////////////////
ErrorStatus HSEStartUpStatus;
#define ADC1_DR_Address ((u32)0x4001244C)//ADC1�����ַ dma��
u32 ADC_ConvertedValue[4096];//ADC1�ɼ����ݻ���
u32 Lcd_Buff1[320];	//˫lcd��������ʵ��lcd�Ŀ��ٲ�д
u32 Lcd_Buff2[320];

DMA_InitTypeDef DMA_InitStructure;		  
u8 dma_flag=1;//��¼˫lcd����ʹ�����
u8 WK_flag=1;//��¼λ�����
u8 H_range=4,L_range=4;//���ڲ�����ʾ�Ŀ��
u32 min,max,average; //���ֵ��Сֵƽ��ֵ
u16 QSW[2];//��ʼλ				
u16 ZZW;//��ֹλ

////////////////////////////////////
 int main(void)
 {	 
	u8 key;
    RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();

	delay_init();//��ʱ������ʼ��		  
	uart_init(9600);//���ڳ�ʼ��Ϊ
 	LED_Init();	 //LED�˿ڳ�ʼ��
	KEY_Init();	//���̳�ʼ��
	//////////////////////////////////
	LCD_Init();	//LCD�˿ڳ�ʼ��
	LCD_Clear(BLACK);
	LCD_Display_Dir(1);
	POINT_COLOR = RED;

	Time_Configuration();
	DMA_Configuration();

	Dac_TriangleWave();
//	DAC_Mode_Init();
	ADC_Configuration(ADC_SampleTime_1Cycles5);

	while (1)
    {
		/////////////////////////
		//��չ����ʱ
	    key=KEY_Scan(0);
		if(key == 1)
		{
			if((4000-L_range*320)>=3*(QSW[1]-QSW[0])&&L_range!=13)//���ܳ����ٽ�ֵ
				L_range++;
		}
		else if(key == 2)
		{
			if(L_range > 1)
				L_range--;	
		}
		LED0 = !LED0;
	}
 }

/************************************************************
*����GPIOC0Ϊģ������
************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;  
	//PC3����Ϊģ��ͨ��3                   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

//////////////////////////////////////////////////////////////////////////////
//ʱ������
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  ///////////////////////////////////////////////////
	//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);//ʹ��ADC1,GPIOCʱ��
}

/************************************************************
*����DMA
************************************************************/
void DMA_Configuration(void)
{
	DMA_DeInit(DMA1_Channel1);//��λDMAͨ��1
 	DMA_InitStructure.DMA_PeripheralBaseAddr =ADC1_DR_Address;//���� DMAͨ���������ַ=ADC1_DR_Address
 	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //����DMAͨ���洢����ַ
 	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//ָ������ΪԴ��ַ
	DMA_InitStructure.DMA_BufferSize = 4096;//����DMA��������С1
 	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ǰ����Ĵ�����ַ����
 	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢����ַ����
 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�����������ݿ��32λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; //����洢�����ݿ��32λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMAͨ������ģʽλΪѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMAͨ�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ֹDMAͨ���洢�����洢������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMAͨ��1
	
	DMA_ITConfig( DMA1_Channel1,DMA_IT_TC, ENABLE);//ʱ��dma�ж�
	DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��1
}
/************************************************************
*����adc1
************************************************************/
void ADC_Configuration(u8 cysj)
{
	ADC_InitTypeDef ADC_InitStructure; 
    //������ADC1�ļĴ�������
  	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADģʽѡΪ����ģʽ
  	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//�Զ�ɨ��ģʽʹ��
  	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//����ת��ģʽʹ��
  	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//û���жϴ���ת��
  	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�������Ҷ���
  	ADC_InitStructure.ADC_NbrOfChannel = 1;//��ʼ��ADCͨ������1
  	ADC_Init(ADC1, &ADC_InitStructure);//����ADC1�豸
  	//PA0��ӦADC1ͨ����0
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, cysj);
	//ʹ��ADC1ģ��DMA
	ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA
	//ʹ��ADC1
  	ADC_Cmd(ADC1, ENABLE);
  	//��λADC1�ļĴ���  
  	ADC_ResetCalibration(ADC1);
    //�ȴ���λ���� 	
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	//��ʼADC1У׼
  	ADC_StartCalibration(ADC1);
  	//�ȴ�ADC1У׼���� 	
  	while(ADC_GetCalibrationStatus(ADC1));
    //ʹ��ADC1ת��
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
void ADC_Restart(u8 cysj)
{
	//PA0��ӦADC1ͨ����0
  	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, cysj);
	//ʹ��ADC1ģ��DMA
	ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA
	//ʹ��ADC1
  	ADC_Cmd(ADC1, ENABLE);
  	//��λADC1�ļĴ���  
  	ADC_ResetCalibration(ADC1);
    //�ȴ���λ���� 	
  	while(ADC_GetResetCalibrationStatus(ADC1));
  	//��ʼADC1У׼
  	ADC_StartCalibration(ADC1);
  	//�ȴ�ADC1У׼���� 	
  	while(ADC_GetCalibrationStatus(ADC1));
    //ʹ��ADC1ת��
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/************************************************************
*�����ж����ȼ�
************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	 	 
	//�������ȼ�ģʽ���������Ķ�ԭ�����е�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel=ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//��TIM3�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
/************************************************************
*���ö�ʱ�� 50ms 
************************************************************/
void Time_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/*Time base configuration  */
	TIM_TimeBaseStructure.TIM_Period = 100;//50ms
	TIM_TimeBaseStructure.TIM_Prescaler = 35999; //����Ԥ��Ƶ����Ƶϵ��36000
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//������ʱ�ӷָ� 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//ѡ�����ϼ���

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx 
	TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
	TIM_ARRPreloadConfig(TIM2, DISABLE);
  	/* TIM IT enable */
  	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);//ʹ�ܶ�ʱ�����ж�

  	/* TIM3 enable counter */
  	TIM_Cmd(TIM3, ENABLE);//ʹ�ܶ�ʱ��
}
/************************************************************
*DMA���жϺ���
*adc1���ݻ�����װ���� �ر�dma�жϹر�adc���ȴ�lcdˢ��
************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);

	DMA_ITConfig( DMA1_Channel1,DMA_IT_TC, DISABLE);

	DMA_ClearFlag(DMA1_FLAG_TC1);
	DMA_ClearITPendingBit(DMA1_IT_TC1);
}
/************************************************************
*��ʱ���жϺ���
*����adc�������е����� ˢ��lcd���Ϊ50����
************************************************************/
void TIM3_IRQHandler(void)
{
	u16 i;//����λ
	u16 qsw_n=0;
	u16 PL_flag=0;//Ƶ�ʱ�־
	u16 temp = 0;//�м����
	double MAX_flag=0.0,MIN_flag=0.0;//���ֵ��Сֵ
	QSW[0]=0;//��ʼλ
	QSW[1]=0;//��ʼλ
	ZZW=0; //ת��λ
    /////////////////////////////////////////
	//�����ֵ
	max=ADC_ConvertedValue[0];
	min=ADC_ConvertedValue[0];
	LCD_Clear(BLACK);
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  	{
		//ȡ�����ֵ��Сֵ		
		for(i=0;i<4096;i++)
		{
			if(ADC_ConvertedValue[i]>max)
			{
				max=ADC_ConvertedValue[i];		
			}
			if(ADC_ConvertedValue[i]<min)
			{
				min=ADC_ConvertedValue[i];			
			}		
		}
		//////////////////////////////////////////////////////
		//��ʾ���ֵ
		LCD_ShowString(150,10,200,16,16,"MAX_VAL:0.000v");
		MAX_flag = max*3.3/4096;
		temp = MAX_flag;
		LCD_ShowxNum(214,10,temp,1,16,0);
		MAX_flag-=temp;
		MAX_flag*=1000;
		LCD_ShowxNum(230,10,MAX_flag,3,16,0);
		
		///////////////////////////////////////////////////////
		//��ʾ��Сֵ
		LCD_ShowString(150,30,200,16,16,"MIN VAL:0.000v");
		MIN_flag = min*3.3/4096;
		temp = MIN_flag;
		LCD_ShowxNum(214,30,temp,1,16,0);
		MIN_flag-=temp;
		MIN_flag*=1000;
		LCD_ShowxNum(230,30,MIN_flag,3,16,0);

		average=((max+min)>>1)>>4;

		for(i=4;i<2048;i++)
		{
			//��׽������
			if((ADC_ConvertedValue[i-1]>>4)<average&&
				(ADC_ConvertedValue[i-4]>>4)<average&&
				(ADC_ConvertedValue[i+1]>>4)>average&&
				(ADC_ConvertedValue[i+4]>>4)>average)
			{
				//�ҵ�������			
				if(qsw_n==0)
				{
					//��һ��������	
					QSW[0]=i;
					qsw_n++;
				}
				else if(qsw_n==1&&(i-QSW[0]>5))
				{
					//�ڶ���������
					QSW[1]=i;
					//LCD����ʾƵ��
					if(WK_flag==1)//λ���־Ϊ1
					{
						//11kHz����
						LCD_ShowString(10,10,200,16,16,"WxVEFRE:000000kHz");
						PL_flag=2571428/(QSW[1]-QSW[0]);
												
						PL_flag=PL_flag/10;
						LCD_ShowxNum(114,10,PL_flag%10,1,16,0);

						PL_flag=PL_flag/10;
						LCD_ShowxNum(106,10,PL_flag%10,1,16,0);
						LCD_ShowChar(98,10,'.',16,0);

						PL_flag=PL_flag/10;
						LCD_ShowxNum(90,10,PL_flag%10,1,16,0);

						PL_flag=PL_flag/10;
						LCD_ShowxNum(82,10,PL_flag%10,1,16,0);

						PL_flag=PL_flag/10;
						LCD_ShowxNum(74,10,PL_flag%10,1,16,0);
					}
					if(WK_flag==2)
					{
						//3.4kHz~11kHz
						LCD_ShowString(10,10,200,16,16,"WAxEFRE:000000kHz");
						PL_flag=1384615/(QSW[1]-QSW[0]);

						LCD_ShowxNum(114,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(106,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(98,10,PL_flag%10,1,16,0);
						LCD_ShowChar(90,10,'.',16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(82,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(74,10,PL_flag%10,1,16,0);
					}
					if(WK_flag==3)
					{
						//1kH~3.4kHz
						LCD_ShowString(10,10,200,16,16,"WAVxFRE:000000kHz");
						PL_flag=429642/(QSW[1]-QSW[0]);

						LCD_ShowxNum(114,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(106,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(98,10,PL_flag%10,1,16,0);
						LCD_ShowChar(90,10,'.',16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(82,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(74,10,PL_flag%10,1,16,0);
					}
					if(WK_flag==4)
					{
						//С��1kHz
						LCD_ShowString(10,10,200,16,16,"WAVExRE:000000kHz");
						PL_flag=142857/(QSW[1]-QSW[0]);

						LCD_ShowxNum(114,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(106,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(98,10,PL_flag%10,1,16,0);
						LCD_ShowChar(90,10,'.',16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(82,10,PL_flag%10,1,16,0);
						PL_flag=PL_flag/10;
						LCD_ShowxNum(74,10,PL_flag%10,1,16,0);
					}
					break;
				}
				
			}
			//��׽�½���
			if((ADC_ConvertedValue[i-1]>>4)>average&&
				(ADC_ConvertedValue[i-4]>>4)>average&&
				(ADC_ConvertedValue[i+1]>>4)<average&&
				(ADC_ConvertedValue[i+4]>>4)<average)
			{
				if(QSW[0]!=0)
					ZZW=i;	
			}
			//��׽��ʱ
			if(i==2046)
			{
				QSW[0]=0;
				QSW[1]=0;
				ZZW=0;
				WK_flag=4;//��ⲻ��Ƶ�ʣ�����������
				break;	
			}
		
		}
		//װlcd����
		if(dma_flag==1)
		{
			for(i=0;i<319;i++)  		
				Lcd_Buff1[i]=240 - ADC_ConvertedValue[i*L_range+QSW[0]]/17.1;	//L_range����H_range����
		}
		else
		{
			for(i=0;i<319;i++)  		
				Lcd_Buff2[i]=240 - ADC_ConvertedValue[i*L_range+QSW[0]]/17.1;	
		}	
		
	    //ˢ��lcd
		if(dma_flag==1)
		{
//			for(i=0;i<319;i++) 
//			{
//				POINT_COLOR = DARKBLUE;
//				LCD_DrawLine(i,Lcd_Buff2[i],i+1,Lcd_Buff2[i+1]);	
//			} 		
			for(i=0;i<319;i++)  
			{
				POINT_COLOR = RED;
				LCD_DrawLine(i,Lcd_Buff1[i],i+1,Lcd_Buff1[i+1]);
				printf("%i,",Lcd_Buff1[i]);
			}		
			dma_flag=2;
		}
		else
		{
//			for(i=0;i<319;i++)
//			{
//				POINT_COLOR = DARKBLUE;
//				LCD_DrawLine(i,Lcd_Buff1[i],i+1,Lcd_Buff1[i+1]);
//			}  		
			for(i=0;i<319;i++)
			{
				POINT_COLOR = RED;
				LCD_DrawLine(i,Lcd_Buff2[i],i+1,Lcd_Buff2[i+1]);
			}  		
			dma_flag=1;
		}		
		
		//����Ӧ����Ƶ�� Ƶ�ʲ���Ϊ�˲�Ӱ���ٽ�ֵ��������ʾ	
		//Ƶ��С��7.3KHz
		if(WK_flag==1)
		{
			if(QSW[1]-QSW[0]>350)
			{
				WK_flag++;
				ADC_Configuration(ADC_SampleTime_13Cycles5);
			}
		}
		
		if(WK_flag==2)
		{	
			//Ƶ��С��2.7k ����ʱ���ӳ�
			if(QSW[1]-QSW[0]>512)
			{
				WK_flag++;
				ADC_Configuration(ADC_SampleTime_71Cycles5);
			}
			//Ƶ�ʴ���11khz�����ٲ���ʱ��
			if(QSW[1]-QSW[0]<126)
			{
				WK_flag--;
				ADC_Configuration(ADC_SampleTime_1Cycles5);
			}
		}
		
		if(WK_flag==3)
		{
			//Ƶ��С��860Hz����ʱ���ӳ�
			if(QSW[1]-QSW[0]>498)
			{
				WK_flag++;
				ADC_Configuration(ADC_SampleTime_239Cycles5);
			}
			//Ƶ�ʴ���3.4khz�����ٲ���ʱ��
			if(QSW[1]-QSW[0]<127)
			{
				WK_flag--;
				ADC_Configuration(ADC_SampleTime_13Cycles5);
			}			
		}
		//Ƶ�ʴ���1k�����ٲ���ʱ��
		if(WK_flag==4)
		{
			if(QSW[1]-QSW[0]<142)
			{
				WK_flag--;
				ADC_Configuration(ADC_SampleTime_71Cycles5);
			}
		}
		//�ָ�dma adc
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	  	DMA_Cmd(DMA1_Channel1, ENABLE);
		DMA_ITConfig( DMA1_Channel1,DMA_IT_TC, ENABLE);	
		ADC_DMACmd(ADC1, ENABLE); 
	}
	else
	{
		
	}
}

