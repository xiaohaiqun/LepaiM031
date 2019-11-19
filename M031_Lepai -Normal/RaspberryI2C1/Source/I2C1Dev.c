#include <i2c.h>
#include <stdio.h>
//#include <ip5328.h>
//i2cset -y 1 0x15 0x02 0x33
//i2cget -y 1 0x15 0x82
extern uint8_t NowBtn;
extern void I2C1readAcc(uint8_t* data);
extern void I2C1readGyro(uint8_t* data);
extern void I2C1readMagn(uint8_t* data);

extern void I2C1readPower();
extern void I2C1readVout1_2_A();
extern void I2C1readBAT_V_I();

extern void PowerHandler(uint8_t u8data);
extern void SensoODR_ONOFF_Handler(uint8_t u8data);

extern uint8_t AccP,GyroP,MagnP,powerP,batDataP,vout12AP;
extern uint8_t AccData[2][6];
extern uint8_t GyroData[2][6];
extern uint8_t MagnData[2][6];
extern uint8_t powerData[2][4];
extern uint8_t vout12AData[2][4];
extern uint8_t batData[2][4];


void I2C1_GPIO_Init(void)
{
		CLK_EnableModuleClock(I2C1_MODULE);
		SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk)) |
                    (SYS_GPB_MFPL_PB2MFP_I2C1_SDA | SYS_GPB_MFPL_PB3MFP_I2C1_SCL);
}

void I2C1_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);
    /* Get I2C1 Bus Clock */
    //printf("I2C1 clock %d Hz\n", I2C_GetBusClockFreq(I2C1));
    /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x15, 0);   /* Slave Address : 0x15 */
    /* Enable I2C interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);	
		I2C_EnableWakeup  ( I2C1 );
		/* I2C enter no address SLV mode */
    I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
		I2C_EnableTimeout(I2C1,1);
}


void I2C1_Close(void)
{
    /* Disable I2C1 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C1);
    NVIC_DisableIRQ(I2C1_IRQn);
    /* Disable I2C1 and close I2C1 clock */
    I2C_Close(I2C1);
    CLK_DisableModuleClock(I2C1_MODULE);
}

static uint8_t Order=0;
static uint8_t *data=NULL;
static uint8_t datapoint=0;
extern uint8_t BMM_whoami();
extern uint8_t I2C0InUseFlag;

void ReadOrderHandler(uint8_t Order)
{
	uint8_t u8data=0;	
	switch(Order){
		case 0x82:
			I2C_SET_DATA(I2C1, NowBtn);//button read
			break;
		case 0x83:
			//I2C1readAcc(data);       //Acc read
			data=AccData[AccP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		case 0x84:                   //Gyro read
			//I2C1readGyro(data);
			data=GyroData[GyroP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		case 0x85:                    //Magn read
			//I2C1readMagn(data);
			data=MagnData[MagnP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		case 0x8A:                   //Battery Powerdata read  
			data=powerData[powerP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		case 0x8B:                   //vout1 and vout2 I read, 
			//I2C1readMagn(data);
			data=vout12AData[vout12AP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		case 0x8C:                  //
			data=batData[batDataP];
			datapoint=0;
			I2C_SET_DATA(I2C1, data[datapoint++]);
			break;
		default:
			I2C_SET_DATA(I2C1, 0x00);
			break;		
	}	
}

void WriteOrderHandler(uint8_t Order,uint8_t u8data)
{
	switch(Order)
	{
		case 0x41:
			PowerHandler(u8data);
			break;
		case 0x46:
			//printf("Sensensor ONOff handler!\n");
			SensoODR_ONOFF_Handler(u8data);
			break;
		default:
			break;
	}	
}
uint8_t I2C1ReadFlag=0;
uint8_t I2C1WriteFlag=0;
uint8_t readOrder=0;
uint8_t writeOrder=0;
uint8_t writeData=0;
void I2C1Handler()
{
	/*if(I2C1ReadFlag)
	{
		ReadOrderHandler(readOrder);
		I2C1ReadFlag=0;
	}*/
	if(I2C1WriteFlag&(!I2C0InUseFlag))
	{		
		I2C0InUseFlag=1;
		WriteOrderHandler(writeOrder,writeData);
		I2C1WriteFlag=0;
		I2C0InUseFlag=0;
	}
}

void I2C_SlaveTRx(uint32_t u32Status)
{
	uint8_t u8data;
	if(u32Status == 0x60)                       /* Own SLA+W has been receive; ACK has been return */
	{
		I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0x80)                 /* Previously address with own SLA address
																								 Data has been received; ACK has been returned*/
	{
		u8data = (unsigned char) I2C_GET_DATA(I2C1);
		I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
		if((Order&0xC0)==0x40){
			//WriteOrderHandler(Order,u8data);
			I2C1WriteFlag=1;
			writeOrder=Order;
			writeData=u8data;
			Order=0x00;
		}
		else{
			Order=u8data;
		}
			//I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xA8)                  /* Own SLA+R has been receive; ACK has been return */
	{
		if((Order&0xC0)==0x80){
			ReadOrderHandler(Order);
				//I2C1ReadFlag=1;
				//readOrder=Order;
		}
		else{
			I2C_SET_DATA(I2C1, 0x00);
		}
		I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xB8)                  /* Data byte in I2CDAT has been transmitted ACK has been received */
	{
			I2C_SET_DATA(I2C1, data[datapoint++]);
			I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xC0)                 /* Data byte or last data in I2CDAT has been transmitted
																								 Not ACK has been received */
	{
			//printf("5 ");
			I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0x88)                 /* Previously addressed with own SLA address; NOT ACK has
																								 been returned */
	{
			 //printf("6 ");
			I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else if(u32Status == 0xA0)                 /* A STOP or repeated START has been received while still
																								 addressed as Slave/Receiver*/
	{
			// printf("7 \n");
			I2C_SET_CONTROL_REG(I2C1, I2C_CTL_SI_AA);
	}
	else
	{
			/* TO DO */
			//printf("Status 0x%x is NOT processed\n", u32Status);
	}
}


volatile uint8_t g_u8SlvPWRDNWK, g_u8SlvI2CWK;
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;
		 if(I2C_GET_WAKEUP_FLAG(I2C1))
    {
        /* Clear I2C Wake-up interrupt flag */
        I2C_CLEAR_WAKEUP_FLAG(I2C1);
        g_u8SlvI2CWK = 1;
				//printf("I2C1 wake up\n");
    }
    u32Status = I2C_GET_STATUS(I2C1);

    if(I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C1 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
			I2C_SlaveTRx(u32Status);
    }
		
}
/*---------------------------------------------------------------------------------------------------------*/
/*  Power Wake-up IRQ Handler                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void PWRWU_IRQHandler(void)
{
    /* Check system power down mode wake-up interrupt flag */
    if(((CLK->PWRCTL) & CLK_PWRCTL_PDWKIF_Msk) != 0)
    {
        /* Clear system power down wake-up interrupt flag */
        CLK->PWRCTL |= CLK_PWRCTL_PDWKIF_Msk;
        g_u8SlvPWRDNWK = 1;
				//printf("powerdown wake up\n");

    }
}
