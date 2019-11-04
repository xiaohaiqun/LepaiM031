#include <i2c.h>

void I2C0_GPIO_Init(){
	CLK_EnableModuleClock(I2C0_MODULE);
	SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk)) |
                    (SYS_GPC_MFPL_PC0MFP_I2C0_SDA | SYS_GPC_MFPL_PC1MFP_I2C0_SCL);
};

void I2C0_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 400000);
    /* Get I2C1 Bus Clock */
    //printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));
    /* Enable I2C interrupt */
    //I2C_EnableInt(I2C0);
    //NVIC_EnableIRQ(I2C0_IRQn);	
		//I2C_EnableWakeup  ( I2C0 );
}
