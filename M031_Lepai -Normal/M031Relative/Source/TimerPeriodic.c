#include <sys.h>
#include <stdio.h>
#include <pwm_light.h>

extern uint8_t NowBtn;
extern uint8_t PowerOn();
extern uint8_t PowerOff();
extern void reset();
extern uint8_t PowerState;

void TIMER_GPIO_Init(){
	 /* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_EnableModuleClock(TMR2_MODULE);
    CLK_EnableModuleClock(TMR3_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_PCLK0, 0);
    CLK_SetModuleClock(TMR2_MODULE, CLK_CLKSEL1_TMR2SEL_HIRC, 0);
    CLK_SetModuleClock(TMR3_MODULE, CLK_CLKSEL1_TMR3SEL_HIRC, 0);
}
uint32_t timecounter =0;
extern uint8_t  Btn9timerStart;

uint8_t InPowerStarting=0;
uint8_t timer0flag=0;

extern uint8_t ShutDownFlag;
uint8_t InPowerOffFlag=0;
uint8_t ShutDownTime=0;
void Btn9LongPressHandler()
{

	if(!InPowerStarting)
	{
		if(Btn9timerStart)
		{
			timecounter++;
			if(timecounter>=5)
			{//长按关机
					Btn9timerStart=0;
					LEDChange(yellow);
					InPowerOffFlag=1;
					PowerOff();
					InPowerOffFlag=0;
			}
			else if(timecounter>=2)
			{
				if(PowerState==1){//软关机			
					NowBtn=0xA9;
					PB5=!PB5;
					LEDChange(red);
				}
				else if(PowerState==0)//开机
				{
					InPowerStarting=1;
					Btn9timerStart=0;
					LEDChange(green);
					//reset();
					PowerOn();
					powerOnLight();
					InPowerStarting=0;
				}		
			}			
		}
	}
	if(ShutDownFlag)
	{
		ShutDownTime++;
		if(ShutDownTime>=30)
		{
			if(PB4)
			{
				if(!InPowerOffFlag)
				{
					InPowerOffFlag=1;
					PowerOff();  //Shut down the power supply of raspbery and m51.
					InPowerOffFlag=0;
				}
			}
			ShutDownTime=0;
			ShutDownFlag=0;
		}
	}
}


extern uint8_t LEDOnWork;
uint8_t LEDBlinkColor=0;
void LEDBlink()
{	
	if(!LEDOnWork)
	{
		LEDBlinkColor=!LEDBlinkColor;
		LEDChange(LEDBlinkColor);
	}
}


uint8_t Timer0Tick=0;
void Timer0Handler()
{
	if(timer0flag)
	{
		Timer0Tick++;
		if(Timer0Tick>100)
		{
			Timer0Tick=0;
			Btn9LongPressHandler();
			//if(PB12)
				//LEDBlink();
		}
		timer0flag=0;
	}
}
extern uint8_t i2cStartFlag;
extern uint8_t i2cWaitCount;
extern void BtnPressTimeCounter();

void TMR0_IRQHandler(void)                    
{ 
   if(TIMER_GetIntFlag(TIMER0) == 1)
    {
      TIMER_ClearIntFlag(TIMER0);
			timer0flag=1;
			BtnPressTimeCounter();
			if(i2cStartFlag)
			{
			  i2cWaitCount++;
			}
    }
}

void TIMER_Init(void){
	  TIMER_Open(TIMER0, TIMER_PERIODIC_MODE,100);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
}
