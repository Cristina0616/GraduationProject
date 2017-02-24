#include "dac.h"
//////////////////////////////////////////////////////////////////////////////////	 								  
//DACͨ��1�����ʼ��
#define DAC_DHR12RD_Address      ((u32)0x40007420)

extern u16 Wave32bit[32];
u32 DualWave32bit[32];
u16 per=0;
//////////////////////////////////////////////////
//ʹ��DAC��ʱ�ӣ���ʼ��GPIO
static void DAC_Config(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	  //ʹ��PORTAͨ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );	  //ʹ��DACͨ��ʱ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;;	// �˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		 //ģ������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	DAC_DeInit();				
	DAC_InitType.DAC_Trigger = DAC_Trigger_T2_TRGO;	//ʹ��TIM2��Ϊ����Դ
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;//DAC1�������ر� BOFF1=1
    DAC_Init(DAC_Channel_1,&DAC_InitType);//��ʼ��DACͨ��1
    DAC_Init(DAC_Channel_2, &DAC_InitType);//��ʼ��DACͨ��2
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
	DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
 
    DAC_Cmd(DAC_Channel_1, ENABLE);//ʹ��ͨ��1 ��PA4���
    DAC_Cmd(DAC_Channel_2, ENABLE);//ʹ��ͨ��2 ��PA5���
    
    DAC_DMACmd(DAC_Channel_2, ENABLE);//ʹ��DAC��DMA����
	DAC_DMACmd(DAC_Channel_1, ENABLE);
}

static void DAC_TIM_Config(u16 peri)
{
	
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
  // ʹ��TIM2ʱ�ӣ�TIM2CLK Ϊ72M//
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_DeInit(TIM2);	
  // TIM2������ʱ������/
  // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = peri;//��ʱ���� 
  TIM_TimeBaseStructure.TIM_Prescaler = 0; //Ԥ��Ƶ������Ƶ 72M / (0+1) = 72M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //ʱ�ӷ�Ƶϵ��
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* ����TIM2����Դ */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	/* ʹ��TIM2 */
  TIM_Cmd(TIM2, ENABLE);

}

////////////////////////////////////////////////
//����DMA

static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* ʹ��DMA2ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	// DMA channel4 configuration
	DMA_DeInit(DMA1_Channel4);
	/* ����DMA2 */
  	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;	//�������ݵ�ַ
  	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DualWave32bit ;//�ڴ����ݵ�ַ DualSine12bit
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
    DMA_Cmd(DMA2_Channel4, ENABLE);
}


//////////////////////////////////////////////////
//DAC��ʼ������

void DAC_Mode_Init(u16 peri)
{
	u8 Idx = 0;  
	RCC_Configuration();
	DAC_TIM_Config(peri);
	DAC_Config();
	DAC_DMA_Config();
	
	/* ������Ҳ������ݣ�˫ͨ���Ҷ���*/
  for (Idx = 0; Idx < 32; Idx++)
  {
  	DualWave32bit[Idx] = (Wave32bit[Idx] << 16) + (Wave32bit[Idx]);
  }
}
void Dac_close(void)
{
	TIM_Cmd(TIM2, DISABLE);
	DAC_Cmd(DAC_Channel_1, DISABLE);//ʹ��ͨ��1 ��PA4���
    DAC_Cmd(DAC_Channel_2, DISABLE);//ʹ��ͨ��2 ��PA5���
  
    DAC_DMACmd(DAC_Channel_2, DISABLE);//ʹ��DAC��DMA����
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	DMA_Cmd(DMA2_Channel4, DISABLE);
}
