#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
/////////////////////////////////////
// ADCת��ʵ��
// �ɼ���ѹ��ת��
// 2014.05.14
/////////////////////////////////////
// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue;
// �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
//float ADC_ConvertedValueLocal;
 int main(void)
 {	 
  	u16 adcx;
	float temp;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	 //LCD�˿ڳ�ʼ��

	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"STM32ZET6");	
	LCD_ShowString(60,70,200,16,16,"ADC TEST");	
	LCD_ShowString(60,90,200,16,16,"Ansleliu");
	LCD_ShowString(60,110,200,16,16,"2014/05/12");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,200,16,16,"ADC_CH0_VOL:0.000V");

	Adc_Init();		  		//ADC��ʼ��

	while (1)
	{
		adcx=ADC_ConvertedValue;
//		ADC_ConvertedValueLocal =(float) adcx/4096*3.3; // ��ȡת����ADֵ
		/////////////////////////////////////////////////
		//��ʾ
		//���ڴ���
		printf("%i,",adcx);
		//��ʾADC��ֵ
		LCD_ShowxNum(156,130,adcx,4,16,0);
		//��ʾ��ѹֵ
		temp=(float)adcx*(3.3/4096);
		adcx=temp;
		LCD_ShowxNum(156,150,adcx,1,16,0);
		temp-=adcx;
		temp*=1000;
		LCD_ShowxNum(172,150,temp,3,16,0X80);
		//////////////////////////////
		//ָʾ��
		LED0=!LED0;
		delay_ms(2);
	}
 }

