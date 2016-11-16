#include "led.h"
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 	 
#include "dac.h"
/////////////////////////////////////////////////////////
//
/*
#define N 5
extern __IO uint16_t ADC_ConvertedValue;
extern vu16 ADC_RegularConvertedValueTab[32], ADC_InjectedConvertedValueTab[32]; 
u16 adcx;
int xpoint=-N;
int ypoint=0;
int oldy=0;
// Private variables ---------------------------------------------------------//
ErrorStatus HSEStartUpStatus;
///////////////////////////////////////////
//
void NVIC_Configuration(void);
void RCC_Configuration(void);
*/
#define N 5
vu16 ADC_RCVTab[400] ;	        //DMA����Ŀ�ĵ�ַ
     vu16 ADC_RCVTab1[400];	       //�ݴ�ռ�
   	 float a=10	;                 // ���ڲ���X���ܱ���
	 int p=1;		             // ���ڲ���Y���ܱ���
     uint32_t Capture = 0;	   	// ����ֵ
     uint32_t TIM2Freq = 0;    // Ƶ��
	 vu16 TIM2_RCVTab[2] ;	  //��ʱ��DMA ����Ŀ�ĵ�ַ���ڲ�Ƶ��
	 uint16_t s0,s1;		 //�ݴ����

 int main(void)
 {	 
 	//��ʼ����---------------------------------------------------------------------------/ 
    int x=-N,y,i=0,t=0,SUM=0,AVERAGE=0,H,L; //X,YΪ���꣬iΪ��־����tΪ�м�����SUM��AVERAGE,H,LΪ��ƽ��ֵ�����ֵ����Сֵ
    float AVERAGE1=0,H1=0,L1=0;		      //�м�����ʵ��ֵ
	char string[50] = ""; 				 //�м��ַ���

	//ϵͳʱ�ӳ�ʼ��������ʱ�ӿ�����GPIO���ã��ж�����-----------------------------------/ 
    SysClock_Init();
	RCC_Configuration();              //����ʱ��ʹ��
	GPIO_Configuration();
	NVIC_Configuration();
	EXTI_Configuration() ;
	//�������ã�DMA,ADC,TIM,SPI,TOUCH----------------------------------------------------/
	DMA_USERConfiguration(); //DMAʹ������	 ���ڴ��䲨�����ݵĲ������ݣ��Լ���ʱ���Ĳ���ֵ
	ADC_Configuration();    //ADC����  ����400������
	TIM_Configuration()	;  //��ʱ�����ã���Ƶ��

	SPI_Configuration();  // MCUͨ��SPI1�봥����ͨ�ţ��ڴ˽��� SPI����
	////////////////////////////////////////////////////////////////////
	//
	delay_init();	    	 //��ʱ������ʼ��
//	RCC_Configuration();
//	NVIC_Configuration();	  
//	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	 //LCD��ʼ��
	LCD_Clear(BLACK);
	LCD_Display_Dir(1);

	POINT_COLOR=RED;
		
	//��ʼ��DAC����ʼDACת��//
	DAC_Mode_Init();
//	Adc_Init();	//ADC��ʼ��
	while(1)
	{
	//ѭ����ʼ ����ADC��DMA���󣬵ȴ�400�����ݴ�����ɣ��رմ��䣬�ٻ�����------------/
	    ADC_DMACmd(ADC1, ENABLE);      //����ADC��DMA ����
		t=ADC_RCVTab[399];
	  	while(t==ADC_RCVTab[399]);	 //�ȴ��������
		ADC_DMACmd(ADC1, DISABLE); 	//�رմ���


        //�����Σ�-----------------------------------------------------------------------/


		for(i=0;i<390;i=i+1)	  
		{
			 x+=N;
			if(x >= 320)
			{
				x = 0;
				LCD_Clear(BLACK);
			}
			//��ʾ
			printf("%i,",ADC_RCVTab[i]);
			y = 240 - (ADC_RCVTab[i]/17.1);
//			LCD_Fast_DrawPoint(x,y,BLUE);
			LCD_DrawLine(x,240 - (ADC_RCVTab1[i-1]/17.1),x+N,y);
			delay_us(100);
			ADC_RCVTab1[i]=y;//����Ҫ�����ֵ
			//////////////////////////////
			//ָʾ��
			LED0=!LED0;
		}
/*
		     x=i*p+81;	                                //ȷ��x
			 if(x<470)
			    {
			      y=160+(int)((a/10)*(-95+ADC_RCVTab[i]*160/2048));	 //ȷ��y  98��Ӧƫ�Ƶ�ѹΪ1V�����
				  ypoint = 240 - (adcx/17.1);
		     	  LCD_Fast_DrawPoint(xpoint,ypoint,BLUE);
		          LCD_DrawLine(xpoint,oldy,xpoint+N,ypoint);
//		   	      SetPixel(x,y);					//����
		   	      ADC_RCVTab1[i]=y;				   //����Ҫ�����ֵ
//				  Delay(30000);

			    }
			 else i=390;

		    }


        //�����ֵ����Сֵ��ƽ��ֵ---------------------------------------------------------/


		for(i=0,SUM=0,H=0,L=0xffff;i<400;i++)
		  {
		   SUM=SUM+ADC_RCVTab[i];            //���
		  
		   if(ADC_RCVTab[i]>H)			   //�����ֵ
		      H=ADC_RCVTab[i];
										 //����Сֵ
			if(ADC_RCVTab[i]<L)
			L=ADC_RCVTab[i];

		  }


		   
		   AVERAGE=SUM/400;
		   AVERAGE1=((AVERAGE*3.27/4096*1000+60)-1050)*3;              //��ƽ��ֵʵ��ֵ
		   H1=(((H)*3.27/4096*1000+100)-1030)*3;			          //�����ֵʵ��ֵ
		   L1=(((L)*3.27/4096*1000-20)-1000)*3;		                 //����Сֵʵ��ֵ

		   sprintf(string, "  %6.1f", AVERAGE1);                     //дƽ��ֵ
//		   PutsXY(10,105,string);			 

		   sprintf(string, "  %6.1f", H1);		                //д���ֵ		
//		   PutsXY(10,80,"MAX:    ");
//		   PutsXY(10,55,string);

		   sprintf(string, " %6.1f", L1);	                 //д��Сֵ
//		   PutsXY(10,30,"MIN:    ");
//		   PutsXY(10,5,string);

		   sprintf(string,"%6.1f", (H1-L1));	         //д���ֵ
//		   PutsXY(10,205,string);


		   
           //�ȴ�DMA��TIM��ȡ��ֵȫ�����£�����Ƶ��-------------------------------------------/


		   s0=TIM2_RCVTab[0];
		   s1=TIM2_RCVTab[1] ;
           while(s1==TIM2_RCVTab[1]) ;                                 //ȫ������
                
		   if(TIM2_RCVTab[0]<TIM2_RCVTab[1])				    
             Capture = (TIM2_RCVTab[1] - TIM2_RCVTab[0]); 	         //��һ����ʱ��������
				  
		   else
		     Capture = ((0xFFFF- TIM2_RCVTab[0]) + TIM2_RCVTab[1]);//����������ڱ߽� 
				   		   
		   TIM2Freq = (uint32_t)7200000 / Capture;                // ����Ƶ��
		   sprintf(string, "%6d", TIM2Freq);
//		   PutsXY(10,255,string);	


		   //��ʱ���ֲ��Σ��ٲ������Σ��ص�ѭ����ʼ�����»���----------------------------------/

		  // Delay(800000); //��ʱ���ֲ���
		   
		   	for(i=0;i<390;i=i+1)	 //��������
		  {
		     
		     x=i*p+81;
			 if(x<470)
			 {
			 y=ADC_RCVTab1[i];
//		  	 ErasePixel(x,y) ;
//			 Draw_horizontal_xuxian(80,470,160);
			  }
			else  i=390;
	    } 
*/		    
	}  
	////////////////////////////////////////////////////
	//
/*
  	while(1)
	{
		xpoint+=N;
		if(xpoint >= 320)
		{
			xpoint = 0;
			LCD_Clear(BLACK);
		}
		adcx=ADC_ConvertedValue;
		//��ʾ
		ypoint = 240 - (adcx/17.1);
//		LCD_Fast_DrawPoint(xpoint,ypoint,BLUE);
		LCD_DrawLine(xpoint,oldy,xpoint+N,ypoint);
		delay_us(1.17);
		//////////////////////////////
		//ָʾ��
		LED0=!LED0;
		oldy = ypoint;
	}
*/		   	      		
 }
/*
void ADC1_2_IRQHandler()
{
  int i	=0 ;
  ADC_ITConfig(ADC1,ADC_IT_JEOC,DISABLE);

  for(;i<32;i++)
  {
  		xpoint+=N;
		if(xpoint >= 320)
		{
			xpoint = 0;
			LCD_Clear(BLACK);
		}
		adcx=ADC_RegularConvertedValueTab[i];
		//��ʾ
		ypoint = 240 - (adcx/17.1);
//		LCD_Fast_DrawPoint(xpoint,ypoint,BLUE);
		LCD_DrawLine(xpoint,oldy,xpoint+N,ypoint);
		//////////////////////////////
		//ָʾ��
		LED0=!LED0;
		oldy = ypoint;
  }
  ADC_ITConfig(ADC1,ADC_IT_JEOC,ENABLE);
}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
void RCC_Configuration(void)
{   
  /* RCC system reset(for debug purpose) //
  RCC_DeInit();

  /* Enable HSE //
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready //
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer //
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state //
    FLASH_SetLatency(FLASH_Latency_2);
	
    /* HCLK = SYSCLK//
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK //
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 //
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* ADCCLK = PCLK2/4 //
    RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
  
    /* PLLCLK = 8MHz * 9 = 56 MHz//
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_7);

    /* Enable PLL // 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready//
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source //
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source //
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  // Set the Vector Table base location at 0x20000000// 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else // VECT_TAB_FLASH //
  // Set the Vector Table base location at 0x08000000 //
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  //Enable ADC1_2 IRQChannel //
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
 */
