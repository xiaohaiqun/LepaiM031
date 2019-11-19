/**************************************************************************//**
 * @file     i2c.c
 * @version  V1.00
 * @brief    M031 series I2C driver source file
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/
#include "M031Series.h"

/** @addtogroup Standard_Driver Standard Driver
  @{
*/

/** @addtogroup I2C_Driver I2C Driver
  @{
*/


/** @addtogroup I2C_EXPORTED_FUNCTIONS I2C Exported Functions
  @{
*/

/**
  * @brief      Enable specify I2C Controller and set Clock Divider
  *
  * @param[in]  i2c         Specify I2C port
  * @param[in]  u32BusClock The target I2C bus clock in Hz
  *
  * @return     Actual I2C bus clock frequency
  *
  * @details    The function enable the specify I2C Controller and set proper Clock Divider
  *             in I2C CLOCK DIVIDED REGISTER (I2CLK) according to the target I2C Bus clock.
  *             I2C Bus clock = PCLK / (4*(divider+1).
  *
  */
uint32_t I2C_Open(I2C_T *i2c, uint32_t u32BusClock)
{
    uint32_t u32Div;
    uint32_t u32Pclk;

    if (i2c == I2C1)
    {
        u32Pclk = CLK_GetPCLK1Freq();
    }
    else
    {
        u32Pclk = CLK_GetPCLK0Freq();
    }

    u32Div = (uint32_t)(((u32Pclk * 10U) / (u32BusClock * 4U) + 5U) / 10U - 1U); /* Compute proper divider for I2C clock */
    i2c->CLKDIV = u32Div;

    /* Enable I2C */
    i2c->CTL0 |= I2C_CTL0_I2CEN_Msk;

    return (u32Pclk / ((u32Div + 1U) << 2U));
}

/**
  * @brief      Disable specify I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
    *
  * @return     None
  *
  * @details    Reset I2C Controller and disable specify I2C port.
    *
  */

void I2C_Close(I2C_T *i2c)
{
    /* Reset I2C Controller */
    if (i2c == I2C0)
    {
        SYS->IPRST1 |= SYS_IPRST1_I2C0RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_I2C0RST_Msk;
    }
    else if (i2c == I2C1)
    {
        SYS->IPRST1 |= SYS_IPRST1_I2C1RST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_I2C1RST_Msk;
    }

    /* Disable I2C */
    i2c->CTL0 &= ~I2C_CTL0_I2CEN_Msk;
}

/**
  * @brief      Clear Time-out Counter flag
  *
  * @param[in]  i2c         Specify I2C port
    *
  * @return     None
  *
  * @details    When Time-out flag will be set, use this function to clear I2C Bus Time-out counter flag .
    *
  */
void I2C_ClearTimeoutFlag(I2C_T *i2c)
{
    i2c->TOCTL |= I2C_TOCTL_TOIF_Msk;
}

/**
  * @brief      Set Control bit of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  * @param[in]  u8Start     Set I2C START condition
  * @param[in]  u8Stop      Set I2C STOP condition
  * @param[in]  u8Si        Clear SI flag
  * @param[in]  u8Ack       Set I2C ACK bit
  *
  * @return     None
  *
  * @details    The function set I2C Control bit of I2C Bus protocol.
  *
  */
void I2C_Trigger(I2C_T *i2c, uint8_t u8Start, uint8_t u8Stop, uint8_t u8Si, uint8_t u8Ack)
{
    uint32_t u32Reg = 0U;

    if (u8Start)
    {
        u32Reg |= I2C_CTL_STA;
    }

    if (u8Stop)
    {
        u32Reg |= I2C_CTL_STO;
    }

    if (u8Si)
    {
        u32Reg |= I2C_CTL_SI;
    }

    if (u8Ack)
    {
        u32Reg |= I2C_CTL_AA;
    }

    i2c->CTL0 = (i2c->CTL0 & ~0x3CU) | u32Reg;
}

/**
  * @brief      Disable Interrupt of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  *
  * @return     None
  *
  * @details    The function is used for disable I2C interrupt
  *
  */
void I2C_DisableInt(I2C_T *i2c)
{
    i2c->CTL0 &= ~I2C_CTL0_INTEN_Msk;
}

/**
  * @brief      Enable Interrupt of I2C Controller
  *
  * @param[in]  i2c         Specify I2C port
  *
  * @return     None
  *
  * @details    The function is used for enable I2C interrupt
  *
  */
void I2C_EnableInt(I2C_T *i2c)
{
    i2c->CTL0 |= I2C_CTL0_INTEN_Msk;
}

/**
 * @brief      Get I2C Bus Clock
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     The actual I2C Bus clock in Hz
 *
 * @details    To get the actual I2C Bus Clock frequency.
 */
uint32_t I2C_GetBusClockFreq(I2C_T *i2c)
{
    uint32_t u32Divider = i2c->CLKDIV;
    uint32_t u32Pclk;

    if (i2c == I2C1)
    {
        u32Pclk = CLK_GetPCLK1Freq();
    }
    else
    {
        u32Pclk = CLK_GetPCLK0Freq();
    }

    return (u32Pclk / ((u32Divider + 1U) << 2U));
}

/**
 * @brief      Set I2C Bus Clock
 *
 * @param[in]  i2c          Specify I2C port
 * @param[in]  u32BusClock  The target I2C Bus Clock in Hz
 *
 * @return     The actual I2C Bus Clock in Hz
 *
 * @details    To set the actual I2C Bus Clock frequency.
 */
uint32_t I2C_SetBusClockFreq(I2C_T *i2c, uint32_t u32BusClock)
{
    uint32_t u32Div;
    uint32_t u32Pclk;

    if (i2c == I2C1)
    {
        u32Pclk = CLK_GetPCLK1Freq();
    }
    else
    {
        u32Pclk = CLK_GetPCLK0Freq();
    }

    u32Div = (uint32_t)(((u32Pclk * 10U) / (u32BusClock * 4U) + 5U) / 10U - 1U); /* Compute proper divider for I2C clock */
    i2c->CLKDIV = u32Div;

    return (u32Pclk / ((u32Div + 1U) << 2U));
}

/**
 * @brief      Get Interrupt Flag
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C interrupt flag status
 *
 * @details    To get I2C Bus interrupt flag.
 */
uint32_t I2C_GetIntFlag(I2C_T *i2c)
{
    return ((i2c->CTL0 & I2C_CTL0_SI_Msk) == I2C_CTL0_SI_Msk ? 1U : 0U);
}

/**
 * @brief      Get I2C Bus Status Code
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C Status Code
 *
 * @details    To get I2C Bus Status Code.
 */
uint32_t I2C_GetStatus(I2C_T *i2c)
{
    return (i2c->STATUS0);
}

/**
 * @brief      Read a Byte from I2C Bus
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     I2C Data
 *
 * @details    To read a bytes data from specify I2C port.
 */
uint8_t I2C_GetData(I2C_T *i2c)
{
    return (uint8_t)(i2c->DAT);
}

/**
 * @brief      Send a byte to I2C Bus
 *
 * @param[in]  i2c          Specify I2C port
 * @param[in]  u8Data       The data to send to I2C bus
 *
 * @return     None
 *
 * @details    This function is used to write a byte to specified I2C port
 */
void I2C_SetData(I2C_T *i2c, uint8_t u8Data)
{
    i2c->DAT = u8Data;
}

/**
 * @brief      Set 7-bit Slave Address and GC Mode
 *
 * @param[in]  i2c          Specify I2C port
 * @param[in]  u8SlaveNo    Set the number of I2C address register (0~3)
 * @param[in]  u8SlaveAddr  7-bit slave address
 * @param[in]  u8GCMode     Enable/Disable GC mode (I2C_GCMODE_ENABLE / I2C_GCMODE_DISABLE)
 *
 * @return     None
 *
 * @details    This function is used to set 7-bit slave addresses in I2C SLAVE ADDRESS REGISTER (I2C_ADDR0~3)
 *             and enable GC Mode.
 *
 */
void I2C_SetSlaveAddr(I2C_T *i2c, uint8_t u8SlaveNo, uint8_t u8SlaveAddr, uint8_t u8GCMode)
{
    switch (u8SlaveNo)
    {
    case 1:
        i2c->ADDR1  = ((uint32_t)u8SlaveAddr << 1U) | u8GCMode;
        break;

    case 2:
        i2c->ADDR2  = ((uint32_t)u8SlaveAddr << 1U) | u8GCMode;
        break;

    case 3:
        i2c->ADDR3  = ((uint32_t)u8SlaveAddr << 1U) | u8GCMode;
        break;

    case 0:
    default:
        i2c->ADDR0  = ((uint32_t)u8SlaveAddr << 1U) | u8GCMode;
        break;
    }
}

/**
 * @brief      Configure the mask bits of 7-bit Slave Address
 *
 * @param[in]  i2c              Specify I2C port
 * @param[in]  u8SlaveNo        Set the number of I2C address mask register (0~3)
 * @param[in]  u8SlaveAddrMask  A byte for slave address mask
 *
 * @return     None
 *
 * @details    This function is used to set 7-bit slave addresses.
 *
 */
void I2C_SetSlaveAddrMask(I2C_T *i2c, uint8_t u8SlaveNo, uint8_t u8SlaveAddrMask)
{
    switch (u8SlaveNo)
    {
    case 1:
        i2c->ADDRMSK1  = (uint32_t)u8SlaveAddrMask << 1U;
        break;

    case 2:
        i2c->ADDRMSK2  = (uint32_t)u8SlaveAddrMask << 1U;
        break;

    case 3:
        i2c->ADDRMSK3  = (uint32_t)u8SlaveAddrMask << 1U;
        break;

    case 0:
    default:
        i2c->ADDRMSK0  = (uint32_t)u8SlaveAddrMask << 1U;
        break;
    }
}

/**
 * @brief      Enable Time-out Counter Function and support Long Time-out
 *
 * @param[in]  i2c              Specify I2C port
 * @param[in]  u8LongTimeout    Configure DIV4 to enable Long Time-out (0/1)
 *
 * @return     None
 *
 * @details    This function enable Time-out Counter function and configure DIV4 to support Long
 *             Time-out.
 *
 */
void I2C_EnableTimeout(I2C_T *i2c, uint8_t u8LongTimeout)
{
    if (u8LongTimeout)
    {
        i2c->TOCTL |= I2C_TOCTL_TOCDIV4_Msk;
    }
    else
    {
        i2c->TOCTL &= ~I2C_TOCTL_TOCDIV4_Msk;
    }

    i2c->TOCTL |= I2C_TOCTL_TOCEN_Msk;
}

/**
 * @brief      Disable Time-out Counter Function
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     None
 *
 * @details    To disable Time-out Counter function in I2C_TOCTL register.
 *
 */
void I2C_DisableTimeout(I2C_T *i2c)
{
    i2c->TOCTL &= ~I2C_TOCTL_TOCEN_Msk;
}

/**
 * @brief      Enable I2C Wake-up Function
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     None
 *
 * @details    To enable Wake-up function of I2C Wake-up control register.
 *
 */
void I2C_EnableWakeup(I2C_T *i2c)
{
    i2c->WKCTL |= I2C_WKCTL_WKEN_Msk;
}

/**
 * @brief      Disable I2C Wake-up Function
 *
 * @param[in]  i2c          Specify I2C port
 *
 * @return     None
 *
 * @details    To disable Wake-up function of I2C Wake-up control register.
 *
 */
void I2C_DisableWakeup(I2C_T *i2c)
{
    i2c->WKCTL &= ~I2C_WKCTL_WKEN_Msk;
}

/**
  * @brief      Write a byte to Slave
  *
  * @param[in]  i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  data            Write a byte data to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master write a byte data to Slave.
  *
  */
uint8_t i2cStartFlag=0;
uint8_t i2cWaitCount=0;

/**
  * @brief      Specify a byte register address and write a byte to Slave
  *
  * @param[in]  i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data write to
  * @param[in]  data            A byte data to write it to Slave
  *
  * @retval     0               Write data success
  * @retval     1               Write data fail, or bus occurs error events
  *
  * @details    The function is used for I2C Master specify a address that data write to in Slave.
  *
  */

uint8_t I2C_WriteByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, uint8_t data)
{
    uint8_t u8Xfering = 1U, u8Err = 0U, u8Ctrl = 0U;
    uint32_t u32txLen = 0U;
    I2C_START(i2c);                                        /* Send START */
    while (u8Xfering && (u8Err == 0U))
    {
        i2cStartFlag=1;
				i2cWaitCount=0;
				I2C_WAIT_READY(i2c) 
				{
					if(i2cWaitCount>2)
					{
						I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI); 
						i2cStartFlag=0;						
						return 1;
					}
				}
				i2cStartFlag=0;

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (uint8_t)(u8SlaveAddr << 1U | 0x00U));  /* Send Slave address with write bit */
            u8Ctrl = I2C_CTL_SI;                           /* Clear SI */
            break;

        case 0x18:                                         /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                 /* Write Lo byte address of register */
            break;

        case 0x20:                                         /* Slave Address NACK */
        case 0x30:                                         /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                       /* Clear SI and send STOP */
            u8Err = 1U;
            break;

        case 0x28:
            if (u32txLen < 1U)
            {
                I2C_SET_DATA(i2c, data);
                u32txLen++;
            }
            else
            {
                u8Ctrl = I2C_CTL_STO_SI;                   /* Clear SI and send STOP */
                u8Xfering = 0U;
            }

            break;

        case 0x38:                                         /* Arbitration Lost */
        default:                                           /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);      /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;
            u8Err = 1U;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                  /* Write controlbit to I2C_CTL register */
    }

    return (u8Err | u8Xfering);                            /* return (Success)/(Fail) status */
}


/**
  * @brief      Specify a byte register address and read a byte from Slave
  *
  * @param[in]  i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address(1 byte) of data read from
  *
  * @return     Read a byte data from Slave
  *
  * @details    The function is used for I2C Master specify a byte address that a data byte read from Slave.
  *
  *
  */
uint8_t I2C_ReadByteOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr)
{
    uint8_t u8Xfering = 1U, u8Err = 0U, rdata = 0U, u8Ctrl = 0U;
		I2C_START(i2c);                                          /* Send START */
    while (u8Xfering && (u8Err == 0U))
    {
        
				i2cStartFlag=1;
				i2cWaitCount=0;
				I2C_WAIT_READY(i2c) 
				{
					
					if(i2cWaitCount>2)
					{
						I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI); 
						i2cStartFlag=0;						
						return 1;
					}
				}
				i2cStartFlag=0;
        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (uint8_t)(u8SlaveAddr << 1U | 0x00U));    /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x18:                                           /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */
            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1U;
            break;

        case 0x28:
            u8Ctrl = I2C_CTL_STA_SI;                         /* Send repeat START */
            break;

        case 0x10:
            I2C_SET_DATA(i2c, (uint8_t)((u8SlaveAddr << 1U) | 0x01U));  /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x40:                                           /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x48:                                           /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1U;
            break;

        case 0x58:
            rdata = (uint8_t) I2C_GET_DATA(i2c);             /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Xfering = 0U;
            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);        /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;
            u8Err = 1U;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                    /* Write controlbit to I2C_CTL register */
    }

    if (u8Err)
        rdata = 0U;                                           /* If occurs error, return 0 */

    return rdata;                                            /* Return read data */
}

/**
  * @brief      Specify a byte register address and read multi bytes from Slave
  *
  * @param[in]  i2c            Point to I2C peripheral
  * @param[in]  u8SlaveAddr     Access Slave address(7-bit)
  * @param[in]  u8DataAddr      Specify a address (1 byte) of data read from
  * @param[out] rdata[]         A data array to store data from Slave
  * @param[in]  u32rLen         How many bytes need to read from Slave
  *
  * @return     A length of how many bytes have been received
  *
  * @details    The function is used for I2C Master specify a byte address that multi data bytes read from Slave.
  *
  *
  */
uint32_t I2C_ReadMultiBytesOneReg(I2C_T *i2c, uint8_t u8SlaveAddr, uint8_t u8DataAddr, uint8_t rdata[], uint32_t u32rLen)
{
    uint8_t u8Xfering = 1U, u8Err = 0U, u8Ctrl = 0U;
    uint32_t u32rxLen = 0U;
    I2C_START(i2c);                                          /* Send START */

    while (u8Xfering && (u8Err == 0U))
    {
        i2cStartFlag=1;
				i2cWaitCount=0;
				I2C_WAIT_READY(i2c) 
				{
					if(i2cWaitCount>2)
					{
						I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI); 
						i2cStartFlag=0;						
						return 1;
					}
				}
				i2cStartFlag=0;

        switch (I2C_GET_STATUS(i2c))
        {
        case 0x08:
            I2C_SET_DATA(i2c, (uint8_t)(u8SlaveAddr << 1U | 0x00U));    /* Write SLA+W to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x18:                                           /* Slave Address ACK */
            I2C_SET_DATA(i2c, u8DataAddr);                   /* Write Lo byte address of register */
            break;

        case 0x20:                                           /* Slave Address NACK */
        case 0x30:                                           /* Master transmit data NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1U;
            break;

        case 0x28:
            u8Ctrl = I2C_CTL_STA_SI;                         /* Send repeat START */
            break;

        case 0x10:
            I2C_SET_DATA(i2c, (uint8_t)((u8SlaveAddr << 1U) | 0x01U));  /* Write SLA+R to Register I2CDAT */
            u8Ctrl = I2C_CTL_SI;                             /* Clear SI */
            break;

        case 0x40:                                           /* Slave Address ACK */
            u8Ctrl = I2C_CTL_SI_AA;                          /* Clear SI and set ACK */
            break;

        case 0x48:                                           /* Slave Address NACK */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Err = 1U;
            break;

        case 0x50:
            rdata[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c); /* Receive Data */

            if (u32rxLen < (u32rLen - 1U))
                u8Ctrl = I2C_CTL_SI_AA;                      /* Clear SI and set ACK */
            else
                u8Ctrl = I2C_CTL_SI;                         /* Clear SI */

            break;

        case 0x58:
            rdata[u32rxLen++] = (uint8_t) I2C_GET_DATA(i2c); /* Receive Data */
            u8Ctrl = I2C_CTL_STO_SI;                         /* Clear SI and send STOP */
            u8Xfering = 0U;
            break;

        case 0x38:                                           /* Arbitration Lost */
        default:                                             /* Unknow status */
            I2C_SET_CONTROL_REG(i2c, I2C_CTL_STO_SI);        /* Clear SI and send STOP */
            u8Ctrl = I2C_CTL_SI;
            u8Err = 1U;
            break;
        }

        I2C_SET_CONTROL_REG(i2c, u8Ctrl);                    /* Write controlbit to I2C_CTL register */
    }

    return u32rxLen;                                         /* Return bytes length that have been received */
}

/*@}*/ /* end of group I2C_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group I2C_Driver */

/*@}*/ /* end of group Standard_Driver */

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
