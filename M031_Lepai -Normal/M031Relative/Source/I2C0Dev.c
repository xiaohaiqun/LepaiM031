#include <i2c.h>

void I2C0_GPIO_Init(){
	CLK_EnableModuleClock(I2C0_MODULE);
	SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk)) |
                    (SYS_GPC_MFPL_PC0MFP_I2C0_SDA | SYS_GPC_MFPL_PC1MFP_I2C0_SCL);
};

void I2C0_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 8000);
    /* Enable I2C interrupt */
    I2C_DisableInt(I2C0);
		I2C_DisableTimeout(I2C0);
    NVIC_DisableIRQ(I2C0_IRQn);	
}
/*void I2C0_IRQHandler(void){
	I2C_ClearTimeoutFlag(I2C0);
	//I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);  
	//I2C_Trigger(I2C0,0, 1,1,0);
	//I2C_STOP(I2C0);
}*/