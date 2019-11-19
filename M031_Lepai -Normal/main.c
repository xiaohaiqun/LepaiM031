#include <stdio.h>
#include <sys.h>
#include "SYS_init.h"
#include <LSM6DSL.h>
#include <BMM150.h>
#include <IP5328.h>
#include <ButtonProcess.h>
#include <pwm_light.h>
#include "I2C1Dev.h"
#include "I2C0Dev.h"
#include "TimerPeriodic.h"
//DEBUG_ENABLE_SEMIHOST
/////////////////////////////////////////////////////////
extern volatile uint8_t g_u8SlvPWRDNWK, g_u8SlvI2CWK;

void device_GPIO_Init(){
	Button_GPIO_Init();
	PWM_GPIO_Init();
	I2C0_GPIO_Init();
	I2C1_GPIO_Init();
	TIMER_GPIO_Init();
}
void GPAB_IRQHandler(void){
	Button_IRQHandler();
}
void GPCDEF_IRQHandler(void){
	Button_IRQHandler();
}
void reset(){
		SYS_Init();
		I2C1_Init();
		I2C0_Init();
		powerOnLight();		
		Init_BMM150();
    Init_LSM6DSL();	
		IP5328Init();
    TIMER_Init();//time0 start after ip5328 is ready!		
			
}
extern uint8_t PowerState;
extern void Timer0handler();
extern void Timer1Handler();
extern void BtnLongPressHandler();
//extern void I2C1Handler();
int32_t main()
{
		reset();
		PowerState=1;
		while(1){
			Timer0Handler();
			BtnLongPressHandler();
			Timer1Handler();
			I2C1Handler();
			/*SYS_UnlockReg();
			// Enable power wake-up interrupt 
			CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;
			NVIC_EnableIRQ(PWRWU_IRQn);
			g_u8SlvPWRDNWK = 0;
			// Enable I2C wake-up 
			I2C_EnableWakeup(I2C1);
			g_u8SlvI2CWK = 0;
			//printf("\n");
			//printf("Enter PD 0x%x 0x%x\n", I2C1->CTL0, I2C1->STATUS0);
			//printf("\n");
			if(((I2C1->CTL0)&I2C_CTL0_SI_Msk) != 0)
			{
					I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI);
			}
			// Enter to Power-down mode 
			//powerDownFunction();
			//printf("CHIP enter power down status.\n");
			UART_WAIT_TX_EMPTY(UART0);
			CLK_PowerDown();
			
			//* Waiting for syteem wake-up and I2C wake-up finish
			//while((g_u8SlvPWRDNWK & g_u8SlvI2CWK) == 0);

			// Waitinn for I2C response ACK finish 
			while(!I2C_GET_WAKEUP_DONE(I2C1));
			//printf("CHIP wake up done.\n");
			// Clear Wakeup done flag, I2C will release bus 
			I2C_CLEAR_WAKEUP_DONE(I2C1);

			//* Wake-up Interrupt Message 
			//printf("Power-down Wake-up INT 0x%x\n", (unsigned int)((CLK->PWRCTL) & CLK_PWRCTL_PDWKIF_Msk));
			//printf("I2C1 WAKE INT 0x%x\n", I2C1->WKSTS);

			//* Disable power wake-up interrupt
			CLK->PWRCTL &= ~CLK_PWRCTL_PDWKIEN_Msk;
			NVIC_DisableIRQ(PWRWU_IRQn);

			//* Lock protected registers 
			SYS_LockReg();

			//printf("\n");
			//printf("Slave wake-up from power down status.\n");

			//printf("\n");
			//printf("Slave Waiting for receiving data.\n");
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);
			 CLK_SysTickDelay(1000000000);*/
	}
}
