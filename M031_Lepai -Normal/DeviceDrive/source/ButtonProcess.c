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
		GPIO_EnableInt(PB, (4), GPIO_INT_RISING); //raspberry ONOFF,上升沿触发（raspberry软关机时关机引脚变为高电平）
    
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

static struct {
	uint8_t btn1;
	uint8_t btn2;
	uint8_t btn3;
	uint8_t btn4;
	uint8_t btn5;
	uint8_t btn6;
	uint8_t btn7;
	uint8_t btn8;
	uint8_t btn9;
}BtnTimer={0,0,0,0,0,0,0,0,0};
	
//Button9 按下相关操作
void Btn9pressHandler()
{
		/* Start Timer 0 */					
	if(Btn9timerStart==0)     //计时归零
	{
		/*TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);
    TIMER_EnableInt(TIMER0); NVIC_EnableIRQ(TMR0_IRQn);
		TIMER_Start(TIMER0);*/
		timecounter=0;
		Btn9timerStart=1;		//开始计时	
		//NowBtn=0x89;
		LEDOnWork=1;        //占用PWM LED
		//LEDChange(blue);
	}
}
//Button 弹起相关操作
extern uint8_t shutdonwn_flag;
void Btn9releaseHandler()
{
		Btn9timerStart=0;
	  timecounter=0;
		//NowBtn=0x09;
		LEDChange(dark);
		LEDOnWork=0;
		//PWM_ForceStop(PWM0, 0x3F);//关机按键松开后关灯
}
#define LongPressTime 60 //600ms
uint8_t BtnIntFlag=0;
extern void blinkred();
void BtnPressTimeCounter()
{
	if(BtnIntFlag)
	{
		if(ISButtonPressed.btn1)
			(BtnTimer.btn1)++;
		else if(ISButtonPressed.btn2)
			(BtnTimer.btn2)++;
		else if(ISButtonPressed.btn3)
			(BtnTimer.btn3)++;
		else if(ISButtonPressed.btn4)
			(BtnTimer.btn4)++;
		else if(ISButtonPressed.btn5)
			(BtnTimer.btn5)++;
		else if(ISButtonPressed.btn6)
			(BtnTimer.btn6)++;
		else if(ISButtonPressed.btn7)
			(BtnTimer.btn7)++;
		else if(ISButtonPressed.btn8)
			(BtnTimer.btn8)++;
		else if(ISButtonPressed.btn9)
			(BtnTimer.btn9)++;
	}
}
void BtnLongPressHandler()
{
	if(BtnIntFlag)
	{
		if(BtnTimer.btn1>LongPressTime)
		{
			NowBtn=0x91;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn2>LongPressTime)
		{
			NowBtn=0x92;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn3>LongPressTime)
		{
			NowBtn=0x93;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn4>LongPressTime)
		{
			NowBtn=0x94;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn5>LongPressTime)
		{
			NowBtn=0x95;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn6>LongPressTime)
		{
			NowBtn=0x96;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn7>LongPressTime)
		{
			NowBtn=0x97;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn8>LongPressTime)
		{
			NowBtn=0x98;
			BtnIntFlag=0;
			PB5=!PB5;
		}
		else if(BtnTimer.btn9>LongPressTime)
		{
			NowBtn=0x99;
			BtnIntFlag=0;
			PB5=!PB5;
		}
	}
}
uint8_t ShutDownFlag=0;

void Button_IRQHandler(void)
{ 	
	if(GPIO_GET_INT_FLAG(PB, BIT0))//btn1
  {
		GPIO_CLR_INT_FLAG(PB, BIT0);
		if(!ISButtonPressed.btn1)
		{
			BtnTimer.btn1=0;
			BtnIntFlag=1;
			NowBtn=0x81; 
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn1<LongPressTime)
			{						
				NowBtn=0x11;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;
			}
			else 
				NowBtn=0x01;
			PB5=!PB5;			
			BtnTimer.btn1=0;
		}
		ISButtonPressed.btn1=!ISButtonPressed.btn1;		
  }			
	else if(GPIO_GET_INT_FLAG(PB, BIT1))//btn2
  {
		GPIO_CLR_INT_FLAG(PB, BIT1);
		if(!ISButtonPressed.btn2)
		{
			BtnTimer.btn2=0;
			BtnIntFlag=1;
			NowBtn=0x82;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn2<LongPressTime)
			{						
				NowBtn=0x12;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;
			}
			else
				NowBtn=0x02;
			PB5=!PB5;
			BtnTimer.btn2=0;
		}
	  ISButtonPressed.btn2=!ISButtonPressed.btn2;
  }
	
	else if(GPIO_GET_INT_FLAG(PF, BIT3))//btn3
	{
		GPIO_CLR_INT_FLAG(PF, BIT3);
		if(!ISButtonPressed.btn3)
		{
			BtnTimer.btn3=0;
			BtnIntFlag=1;
			NowBtn=0x83;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn3<LongPressTime)
			{						
				NowBtn=0x13;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;
				
			}
			else
				NowBtn=0x03;
			PB5=!PB5;
			BtnTimer.btn3=0;
		}
		ISButtonPressed.btn3=!ISButtonPressed.btn3;
	}
	
	else if(GPIO_GET_INT_FLAG(PF, BIT2))//btn4
	{
		GPIO_CLR_INT_FLAG(PF, BIT2);
		if(!ISButtonPressed.btn4)
		{
			BtnTimer.btn4=0;
			BtnIntFlag=1;
			NowBtn=0x84;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn4<LongPressTime)
			{						
				NowBtn=0x14;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;
			}
			else
				NowBtn=0x04;
			PB5=!PB5;
			BtnTimer.btn4=0;
		}
		ISButtonPressed.btn4=!ISButtonPressed.btn4;
	}
		
	else if(GPIO_GET_INT_FLAG(PF, BIT15))///btn5
	{
		GPIO_CLR_INT_FLAG(PF, BIT15);
		if(!ISButtonPressed.btn5)
		{
			BtnTimer.btn5=0;
			BtnIntFlag=1;
			NowBtn=0x85;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn5<LongPressTime)
			{						
				NowBtn=0x15;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;
			}
			else
				NowBtn=0x05;
			PB5=!PB5;
			BtnTimer.btn5=0;
		}
		ISButtonPressed.btn5=!ISButtonPressed.btn5;
	} 	
	
	else if(GPIO_GET_INT_FLAG(PA, BIT12))//btn6
	{
		GPIO_CLR_INT_FLAG(PA, BIT12);
		if(!ISButtonPressed.btn6)
		{
			BtnTimer.btn6=0;
			BtnIntFlag=1;
			NowBtn=0x86;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn6<LongPressTime)
			{						
				NowBtn=0x16;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;		
			}
			else 
				NowBtn=0x06;
			PB5=!PB5;
			BtnTimer.btn6=0;
		}
		ISButtonPressed.btn6=!ISButtonPressed.btn6;
	}
		
	else if(GPIO_GET_INT_FLAG(PA, BIT13))//btn7
	{
		GPIO_CLR_INT_FLAG(PA, BIT13);
		if(!ISButtonPressed.btn7)
		{
			BtnTimer.btn7=0;
			BtnIntFlag=1;
			NowBtn=0x87;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn7<LongPressTime)
			{						
				NowBtn=0x17;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;				
			}
			else
				NowBtn=0x07;
			PB5=!PB5;
			BtnTimer.btn7=0;
		}
		ISButtonPressed.btn7=!ISButtonPressed.btn7;
	}
	
	else if(GPIO_GET_INT_FLAG(PA, BIT14))//btn8
	{
		GPIO_CLR_INT_FLAG(PA, BIT14);
		if(!ISButtonPressed.btn8)
		{
			BtnTimer.btn8=0;
			BtnIntFlag=1;
			NowBtn=0x88;
			PB5=!PB5;
		}
		else
		{
			if(BtnTimer.btn8<LongPressTime)
			{						
				NowBtn=0x18;//短按，如果是长按则会在长按操作函数中处理;
				BtnIntFlag=0;			
			}
			else
				NowBtn=0x08;
			PB5=!PB5;
			BtnTimer.btn8=0;
		}
		ISButtonPressed.btn8=!ISButtonPressed.btn8;
	}	
	
  else if(GPIO_GET_INT_FLAG(PA, BIT15))
	{
		GPIO_CLR_INT_FLAG(PA, BIT15); 
		if(!ISButtonPressed.btn9)
			{//开关机键按下	
				BtnTimer.btn9=0;
				BtnIntFlag=1;	
				NowBtn=0x89;
			  PB5=!PB5;
				Btn9pressHandler();
			}
			else
			{	
				if(BtnTimer.btn9<LongPressTime)
				{						
					NowBtn=0x19;//短按，如果是长按则会在长按操作函数中处理;
					BtnIntFlag=0;		
				}
				else
					NowBtn=0x09;
				PB5=!PB5;
				BtnTimer.btn9=0;
				Btn9releaseHandler();
			}
			ISButtonPressed.btn9=!ISButtonPressed.btn9;
	}
	///////////////generate IRQ to raspberry//////////////////
	if(GPIO_GET_INT_FLAG(PB, BIT4)){
		//printf("raspberry shutdown\n");
		GPIO_CLR_INT_FLAG(PB, BIT4);
		ShutDownFlag=1;
	}
}