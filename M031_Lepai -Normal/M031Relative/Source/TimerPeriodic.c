#include <sys.h>
#include <stdio.h>
#include <pwm_light.h>
extern void I2C1readAcc(uint8_t* data);
extern void I2C1readGyro(uint8_t* data);
extern void I2C1readMagn(uint8_t* data);
extern uint8_t AccOn;
extern uint8_t GyroOn;
extern uint8_t MagnOn;
extern uint8_t NowBtn;
extern uint8_t PowerOn();
extern uint8_t PowerOff();
extern void  CloseVout1();
extern void	 CloseVout2();
extern void reset();
extern uint8_t PowerState;
uint8_t AccP=0,GyroP=0,MagnP=0;
uint8_t AccData[2][6]={0};
uint8_t GyroData[2][6]={0};
uint8_t MagnData[2][6]={0};

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
extern uint8_t PowerState;
uint32_t timecounter =0;
extern uint8_t  Btn9timerStart;

uint8_t InPowerStarting=0;
uint8_t timer0flag=0;
extern uint8_t powerOnLightFlag;
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
					PowerOff();
			}
			else if(timecounter>=2)
			{
				if(PowerState==1){//软关机			
					NowBtn=0x99;
					PB5=!PB5;
					LEDChange(red);
				}
				else if(PowerState==0)//开机
				{
					InPowerStarting=1;
					Btn9timerStart=0;
					LEDChange(green);
					PowerOn();
					powerOnLight();
					//powerOnLightFlag=1;
					InPowerStarting=0;
				}		
			}			
		}
	}
}
extern void I2C1PowerSpy();
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

uint8_t I2C0InUseFlag=0;
uint8_t Timer0Tick=0;
extern void I2C1readPower();
extern void I2C1readVout1_2_A();
extern void I2C1readBAT_V_I();
void Timer0Handler()
{
	if(timer0flag)
	{
		Timer0Tick++;
		if(Timer0Tick>100)
		{
			Timer0Tick=0;
			Btn9LongPressHandler();
			if(PowerState)
			{
				I2C1PowerSpy();
				I2C1readPower();
				I2C1readVout1_2_A();
				I2C1readBAT_V_I();
			}
			//if(PB12)
				LEDBlink();
		}
		timer0flag=0;
	}
}
extern uint8_t i2cStartFlag;
extern uint8_t i2cWaitCount;
void BtnPressTimeCounter();
void TMR0_IRQHandler(void)                    //used for btn9 long press count
{ 
   if(TIMER_GetIntFlag(TIMER0) == 1)
    {
      TIMER_ClearIntFlag(TIMER0);
			timer0flag=1;
			//Btn9LongPressHandler();
			BtnPressTimeCounter();
			if(i2cStartFlag)
			{
			  i2cWaitCount++;
			}
    }
}
uint8_t timer1flag=0;

void Timer1Handler()
{
	if(timer1flag&&(!I2C0InUseFlag))
	{
		I2C0InUseFlag=1;
		if(AccOn)
			{
				I2C1readAcc(AccData[!AccP]);
				AccP=!AccP;
			}
			if(GyroOn)
			{
				I2C1readGyro(GyroData[!GyroP]);
				GyroP=!GyroP;
			}			
			if(MagnOn)
			{
				//I2C1readMagn(MagnData[!MagnP]);
				MagnP=!MagnP;
			}
		timer1flag=0;
		I2C0InUseFlag=0;
	}
}
void TMR1_IRQHandler(void)                              //used for  9Sensor 
{
		if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
      TIMER_ClearIntFlag(TIMER1);
			timer1flag=1;
    }
}

void TIMER_Init(void){
	  TIMER_Open(TIMER0, TIMER_PERIODIC_MODE,100);
    TIMER_EnableInt(TIMER0);
    /* Open Timer1 in periodic mode, enable interrupt and 2 interrupt ticks per second */    
		//TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 50);
    TIMER_EnableInt(TIMER1);
    /* Open Timer2 in periodic mode, enable interrupt and 4 interrupt ticks per second */    
    /* Enable Timer0 ~ Timer3 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(TMR1_IRQn);
    /* Start Timer0 ~ Timer3 counting */
    TIMER_Start(TIMER0);
    //TIMER_Start(TIMER1);
}
