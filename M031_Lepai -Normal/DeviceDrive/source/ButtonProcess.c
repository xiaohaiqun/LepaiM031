#include <stdio.h>
#include "NuMicro.h"
#include <ButtonProcess.h>
#include <pwm_light.h>

extern void PowerOn();
extern void PowerOff();
extern void reset();

extern uint8_t PowerState;
extern uint8_t timecounter;
uint8_t NowBtn=0xff;
uint8_t Btn9timerStart=0;
extern uint8_t LEDOnWork;
//this should be added to device_GPIO_Init();
void Button_GPIO_Init(void)
{
		GPIO_SetMode(PB, (BIT13), GPIO_MODE_OUTPUT);//模拟按键控制引脚
		GPIO_SetMode(PB, (BIT12), GPIO_MODE_INPUT); //ip5328 I2C状态检测引脚
	
		GPIO_SetMode(PB, (BIT1|BIT0), GPIO_MODE_QUASI);
	  GPIO_SetMode(PF, (BIT3|BIT2|BIT15), GPIO_MODE_QUASI);
		GPIO_SetMode(PA, (BIT12|BIT13|BIT14|BIT15), GPIO_MODE_QUASI); 
	  //////////////////////////////////////////////////////////////////
	  /////for interupt raspberry//////////////////////////////////////
	  GPIO_SetMode(PB, BIT5, GPIO_MODE_QUASI);
	  PB5=0;
	  
	  //////////////////////////////////////////////////////////
	  //////for display raspberry's state////////////////////
		GPIO_SetMode(PB, BIT4, GPIO_MODE_QUASI);
	  GPIO_EnableInt(PB, (4), GPIO_INT_FALLING); //raspberry ONOFF
    
	  //多余引脚配置成输入模式
		GPIO_SetMode(PF, BIT0|BIT1, GPIO_MODE_INPUT);
		GPIO_SetMode(PB, BIT14|BIT15, GPIO_MODE_QUASI);
	  //////////////////////////////////////////////////////
    GPIO_EnableInt(PB, (1), GPIO_INT_BOTH_EDGE); //BTN1
		GPIO_EnableInt(PB, (0), GPIO_INT_BOTH_EDGE);//BTN2
		GPIO_EnableInt(PF, (3), GPIO_INT_BOTH_EDGE);//BTN3
		GPIO_EnableInt(PF, (2), GPIO_INT_BOTH_EDGE);//BTN4
		GPIO_EnableInt(PF, (15),GPIO_INT_BOTH_EDGE);//BTN5
		GPIO_EnableInt(PA, (12),GPIO_INT_BOTH_EDGE);//BTN6
		GPIO_EnableInt(PA, (13),GPIO_INT_BOTH_EDGE);//BTN7
		GPIO_EnableInt(PA, (14),GPIO_INT_BOTH_EDGE);//BTN8
		GPIO_EnableInt(PA, (15),GPIO_INT_BOTH_EDGE);//BTN9
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
		NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);
    
		GPIO_ENABLE_DEBOUNCE(PB,(BIT0|BIT1|BIT4));
    GPIO_ENABLE_DEBOUNCE(PF, (BIT3|BIT2|BIT15));
		GPIO_ENABLE_DEBOUNCE(PA, (BIT12|BIT13|BIT14|BIT15));
		Btn9timerStart=0;
		LEDOnWork=0;
		PB13=0;
}
//this shold be added to GPXXXXXXX_IRQHandle()
static struct{
  bool btn1;
	bool btn2;
	bool btn3;
	bool btn4;
	bool btn5;
	bool btn6;
	bool btn7;
	bool btn8;
	bool btn9;
}ISButtonPressed={0,0,0,0,0,0,0,0,0};
//Button9 按下相关操作
void Btn9pressHandler()
{
		/* Start Timer 0 */					
	if(Btn9timerStart==0)     //计时归零
	{
		TIMER_Start(TIMER0);
		timecounter=0;
		Btn9timerStart=1;		//开始计时	
		NowBtn=0x89;
		LEDOnWork=1;        //占用PWM LED
		LEDChange(blue);
	}
}
//Button 弹起相关操作
void Btn9releaseHandler()
{
		Btn9timerStart=0;
	  timecounter=0;
		NowBtn=0x09;
		LEDChange(dark);
		LEDOnWork=0;
		//PWM_ForceStop(PWM0, 0x3F);//关机按键松开后关灯
}



void Button_IRQHandler(void)
{ 
		if(GPIO_GET_INT_FLAG(PB, BIT0))
    {
        GPIO_CLR_INT_FLAG(PB, BIT0);
				if(!ISButtonPressed.btn1)
				{
					NowBtn=0x81;
					//printf("BUTTON 1 is pressed.\n");
				}
				else
				{
					NowBtn=0x01;
					//printf("BUTTON 1 is release.\n");
				}
				ISButtonPressed.btn1=!ISButtonPressed.btn1;
			
    }		
		else if(GPIO_GET_INT_FLAG(PB, BIT1))
    {
        GPIO_CLR_INT_FLAG(PB, BIT1);
					if(!ISButtonPressed.btn2)
				{
					NowBtn=0x82;
					//printf("BUTTON 2 is pressed.\n");
				}
				else
				{
					NowBtn=0x02;
					//printf("BUTTON 2 is release.\n");
				}
				ISButtonPressed.btn2=!ISButtonPressed.btn2;
			
    }
		
		else if(GPIO_GET_INT_FLAG(PA, BIT12))
    {
        GPIO_CLR_INT_FLAG(PA, BIT12);
				if(!ISButtonPressed.btn6)
				{
					NowBtn=0x86;
					//printf("BUTTON 6 is pressed.\n");
				}
				else
				{
					NowBtn=0x06;
				  //printf("BUTTON 6 is release.\n");
				}
				ISButtonPressed.btn6=!ISButtonPressed.btn6;
    }
		
		else if(GPIO_GET_INT_FLAG(PA, BIT13))
    {
        GPIO_CLR_INT_FLAG(PA, BIT13);
					if(!ISButtonPressed.btn7)
				{
					NowBtn=0x87;
					//printf("BUTTON 7 is pressed.\n");
				}
				else
				{
					NowBtn=0x07;
					//printf("BUTTON 7 is release.\n");
				}
				ISButtonPressed.btn7=!ISButtonPressed.btn7;
    }
		
	 else if(GPIO_GET_INT_FLAG(PA, BIT15))
    {
        GPIO_CLR_INT_FLAG(PA, BIT15);
        
			if(!ISButtonPressed.btn9){//开关机键按下					
					Btn9pressHandler();
				}
				else{		//开关机键松开	
					Btn9releaseHandler();
				}
				ISButtonPressed.btn9=!ISButtonPressed.btn9;
    }
				
		else if(GPIO_GET_INT_FLAG(PA, BIT14))
    {
      GPIO_CLR_INT_FLAG(PA, BIT14);
			if(!ISButtonPressed.btn8)
				{
					NowBtn=0x88;
					//printf("BUTTON 8 is pressed.\n");
				}
				else
				{
					NowBtn=0x08;
					//printf("BUTTON 8 is release.\n");
				}
			ISButtonPressed.btn8=!ISButtonPressed.btn8;
    }		
		 else if(GPIO_GET_INT_FLAG(PF, BIT3))
    {
        GPIO_CLR_INT_FLAG(PF, BIT3);
				if(!ISButtonPressed.btn3)
				{
					NowBtn=0x83;
					//printf("BUTTON 3 is pressed.\n");
				}
				else
				{
					NowBtn=0x03;
					//printf("BUTTON 3 is release.\n");
				}
				ISButtonPressed.btn3=!ISButtonPressed.btn3;
    }
		
		else if(GPIO_GET_INT_FLAG(PF, BIT2))
    {
        GPIO_CLR_INT_FLAG(PF, BIT2);
					if(!ISButtonPressed.btn4)
				{
					NowBtn=0x84;
					//printf("BUTTON 4 is pressed.\n");
				}
				else
				{
					NowBtn=0x04;
					//printf("BUTTON 4 is release.\n");
				}
				ISButtonPressed.btn4=!ISButtonPressed.btn4;
    }
		
		else if(GPIO_GET_INT_FLAG(PF, BIT15))
    {
        GPIO_CLR_INT_FLAG(PF, BIT15);
				if(!ISButtonPressed.btn5)
				{
					NowBtn=0x85;
					//printf("BUTTON 5 is pressed.\n");
				}
				else
				{
					NowBtn=0x05;
					//printf("BUTTON 5 is release.\n");
				}
				ISButtonPressed.btn5=!ISButtonPressed.btn5;
    } 	
		///////////////generate IRQ to raspberry//////////////////
		if(GPIO_GET_INT_FLAG(PB, BIT4)){
			//printf("raspberry shutdown\n");
			GPIO_CLR_INT_FLAG(PB, BIT4);
			PowerOff();  //Shut down the power supply of raspbery and m51.
			PowerState=0;
		}
		else
			PB5=!PB5;
}

