#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
// LEDָʾ��
//								  
////////////////////////////////////////////////////////////////////////////////// 	   
//��ʼ��PE0��PE1Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PE.0 �˿�����	
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOE.0
	 GPIO_SetBits(GPIOE,GPIO_Pin_0);						 //PE.0 �����

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	    		 //LED1-->PE.1 �˿�����, �������
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);	
	 GPIO_SetBits(GPIOE,GPIO_Pin_1); 						 //PE.1 ����� 

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED2-->PE.2 �˿�����, �������
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);	
	 GPIO_SetBits(GPIOE,GPIO_Pin_2); 						 //PE.2 ����� 
}
