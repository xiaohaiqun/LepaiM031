// LSM6DSL Driver: 3-axis Gyroscope + 3-axis accelerometer + temperature

#include <stdio.h>
#include <stdint.h>
#include "NuMicro.h"
#include "LSM6DSL.h"

void LSM6DSL_WriteByte(uint8_t LSM6DSL_reg, uint8_t LSM6DSL_data)
{
	uint8_t temp=0,n=0;
	while(1){
		I2C_WriteByteOneReg(I2C0,LSM6DSL_I2C_SLA ,  LSM6DSL_reg, LSM6DSL_data);
		temp=I2C_ReadByteOneReg(LSM6DSL_I2C_PORT,LSM6DSL_I2C_SLA,LSM6DSL_reg);
		n++;
		if(temp==LSM6DSL_data||n>3)
			break;
	}
}

uint8_t LSM6DSL_ReadByte(uint8_t LSM6DSL_reg)
{
	  return I2C_ReadByteOneReg(LSM6DSL_I2C_PORT,LSM6DSL_I2C_SLA,LSM6DSL_reg);
}

void LSM6DSL_Read6Bytes(uint8_t LSM6DSL_reg,uint8_t* data)
{
	I2C_ReadMultiBytesOneReg(LSM6DSL_I2C_PORT,LSM6DSL_I2C_SLA, LSM6DSL_reg,data, 6);
}

void Init_LSM6DSL(void)
{
	//printf("MPU init start******************\n");
	//LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL1_XL, LSM6DSL_ACC_ODR_833_HZ);
	//LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL2_G, LSM6DSL_GYRO_ODR_833_HZ);
	//printf("*************************************\n");
	LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL6_C, 1);  	// open gravity
	LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL7_G, 1);
}


int16_t Read_LSM6DSL_AccX(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTX_L_XL); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTX_H_XL); // read Accelerometer X_High value
	return((HiByte<<8) | LoByte);
}
int16_t Read_LSM6DSL_ACC(void)
{
	uint8_t bytes[2];
	I2C_ReadMultiBytesOneReg(I2C0, LSM6DSL_I2C_SLA, LSM6DSL_ACC_GYRO_OUTX_L_XL, bytes, 2);
	return ((bytes[1]<<8)|bytes[0]);
}
int16_t Read_LSM6DSL_AccY(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTY_L_XL); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTY_H_XL); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}

int16_t Read_LSM6DSL_AccZ(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTZ_L_XL); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTZ_H_XL); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}

int16_t Read_LSM6DSL_GyroX(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTX_L_G); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTX_H_G); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}

int16_t Read_LSM6DSL_GyroY(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTY_L_G); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTY_H_G); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}

int16_t Read_LSM6DSL_GyroZ(void)
{
	uint8_t LoByte, HiByte;
	LoByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTZ_L_G); // read Accelerometer X_Low  value
	HiByte = LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_OUTZ_H_G); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}


uint16_t Read_LSM6DSL_ID(void)
{
	return (LSM6DSL_ReadByte(LSM6DSL_ACC_GYRO_WHO_AM_I_REG));
}

void LSM6DSL_test(){
	 int16_t accX, accY, accZ;
   int16_t gyroX, gyroY, gyroZ;
	
		while(1){
		//accX = Read_LSM6DSL_AccX();
		//accY = Read_LSM6DSL_AccY();
		//accZ = Read_LSM6DSL_AccZ();		
		//printf("%6d, %6d, %6d \n", accX, accY, accZ);
		gyroX = Read_LSM6DSL_GyroX();
		gyroY = Read_LSM6DSL_GyroY();
		gyroZ = Read_LSM6DSL_GyroZ();		
		//printf("%6d, %6d, %6d \n", gyroX, gyroY, gyroZ);	
		CLK_SysTickDelay(1000000000);
		}
}

////////////I2C1 Order Handler////////////////////////////
void I2C1readAcc(uint8_t* data){
	LSM6DSL_Read6Bytes(LSM6DSL_ACC_GYRO_OUTX_L_XL,data);    //LoByte is at first.
}

void I2C1readGyro(uint8_t* data){
	LSM6DSL_Read6Bytes(LSM6DSL_ACC_GYRO_OUTX_L_G,data);  //LoByte is at first.
}

extern void Init_BMM150();
extern void BMM150_ToSleepMOde();
extern void BMM150_ToNormalMode();
uint8_t AccOn=0,GyroOn=0,MagnOn=0;
void SensoODR_ONOFF_Handler(uint8_t u8data){
	int timerPriod=0;
	if(u8data&0x01){
			LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL1_XL, LSM6DSL_ACC_ODR_833_HZ);
			AccOn=1;
	}
	else{
		LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL1_XL,LSM6DSL_ACC_ODR_POWER_DOWN );
		AccOn=0;
	}
	if(u8data&0x02){
		LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL2_G, LSM6DSL_GYRO_ODR_833_HZ);
		GyroOn=1;
	}
	else{	
		LSM6DSL_WriteByte(LSM6DSL_ACC_GYRO_CTRL2_G, LSM6DSL_GYRO_ODR_POWER_DOWN);
		GyroOn=0;
	}
	if(u8data&0x04){
		BMM150_ToNormalMode();
		MagnOn=1;
	}
	else{
		BMM150_ToSleepMOde();
		MagnOn=0;
	}
	switch(u8data&0xF0){                  //handler ODR 
			case 0x10:														//1HZ	
				timerPriod=1;
				break;
			case 0x20:														//10Hz
				timerPriod=5;
				break;
			case 0x30:														//50Hz
				timerPriod=10;
				break;
			case 0x40:														//100Hz
				timerPriod=25;
				break;
			case 0x50:														//1000Hz
				timerPriod=50;
				break;
			default:
				break;
		}
	if(AccOn||GyroOn||MagnOn){
		Init_BMM150();
		Init_LSM6DSL();
	}
	else{
		;
	}
}
