#include "dac.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//DAC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//DACͨ��1�����ʼ��
#define DAC_DHR12RD_Address      ((u32)0x40007420)

/* �������� ---------------------------------------------------------*/
uc16 Sine12bit[32] = {2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

u32 DualSine12bit[32];

/**
  * @brief  ʹ��DAC��ʱ�ӣ���ʼ��GPIO
  * @param  ��
  * @retval ��
  */
static void DAC_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //ʹ��DACͨ��ʱ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;;	// �˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //ģ������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
					
	DAC_InitType.DAC_Trigger = DAC_Trigger_T4_TRGO;	//ʹ��TIM4��Ϊ����Դ
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_Triangle; //
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_2047; 
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1�������ر� BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);	 //��ʼ��DACͨ��1
	/* ����DAC ͨ��2 */
//    DAC_Init(DAC_Channel_2, &DAC_InitType);
	DAC_SetChannel1Data(DAC_Align_12b_R, 1023);  //12λ�Ҷ������ݸ�ʽ����DACֵ
//	DAC_SetChannel2Data(DAC_Align_12b_R, 1023);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	DAC_WaveGenerationCmd(DAC_Channel_1,DAC_Wave_Triangle,ENABLE);
    /* ʹ��ͨ��1 ��PA4��� */
    DAC_Cmd(DAC_Channel_1, ENABLE);
    /* ʹ��ͨ��2 ��PA5��� */
 //   DAC_Cmd(DAC_Channel_2, ENABLE);
    /* ʹ��DAC��DMA���� */
//    DAC_DMACmd(DAC_Channel_2, ENABLE);
}

//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	float temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

static void DAC_TIM_Config(void)
{
	
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	/* ʹ��TIM4ʱ�ӣ�TIM4CLK Ϊ72M */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
  /* TIM4������ʱ������ */
 // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 0x85;//��ʱ���� 20  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x69; //Ԥ��Ƶ������Ƶ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //ʱ�ӷ�Ƶϵ��
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  /* ����TIM4����Դ */
  TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);

	/* ʹ��TIM4 */
  TIM_Cmd(TIM4, ENABLE);

}

/**
  * @brief  ����DMA
  * @param  ��
  * @retval ��
  */
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* ʹ��DMA2ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	// DMA channel1 configuration
	DMA_DeInit(DMA1_Channel4);
	/* ����DMA2 */
  	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;	//�������ݵ�ַ
  	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualSine12bit ;//�ڴ����ݵ�ַ DualSine12bit
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//���ݴ��䷽���ڴ�������
  	DMA_InitStructure.DMA_BufferSize = 32;//�����СΪ32�ֽ�	
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�������ݵ�ַ�̶�	
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//�ڴ����ݵ�ַ����
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//������������Ϊ��λ
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//�ڴ���������Ϊ��λ	
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//ѭ��ģʽ
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	//��DMAͨ�����ȼ�
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//���ڴ����ڴ�ģʽ	

  	DMA_Init(DMA2_Channel4, &DMA_InitStructure);
	
    /* ʹ��DMA2-14ͨ�� */
//    DMA_Cmd(DMA2_Channel4, ENABLE);
}


/**
  * @brief  DAC��ʼ������
  * @param  ��
  * @retval ��
  */
void DAC_Mode_Init(void)
{
	u8 Idx = 0;  
	DAC_TIM_Config();
	DAC_Config();
//	DAC_DMA_Config();
	
	/* ������Ҳ������ݣ�˫ͨ���Ҷ���*/
//  for (Idx = 0; Idx < 32; Idx++)
//  {
//    DualSine12bit[Idx] = (Sine12bit[Idx] << 16) + (Sine12bit[Idx]);
//  }
}