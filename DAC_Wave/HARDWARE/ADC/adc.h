#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//void Adc_Init(void);
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
//void NVIC_Configuration(void);
void DMA_USERConfiguration(void);
void ADC_Configuration(void);
void TIM_Configuration(void);
void SysClock_Init(void); //ϵͳʱ������72M
void RCC_Configuration(void);//����ʱ��ʹ��
void NVIC_Configuration(void);//�ж�����
void GPIO_Configuration(void); //GPIO ����
void EXTI_Configuration(void);//�ⲿ�ж� 
#endif 
