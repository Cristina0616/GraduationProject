#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/////////////////////////////////////////////////////////////////
#define KEY0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)//��ȡ����0
#define KEY1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)//��ȡ����2 
#define KEY3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//��ȡ����3


#define KEY_UP 		1
#define KEY_DOWN	2
#define KEY_ADD	    3
#define KEY_MINUS	4
void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��
					    
#endif
