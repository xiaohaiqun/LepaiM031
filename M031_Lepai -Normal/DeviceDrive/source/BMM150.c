#include <stdio.h>
#include <stdint.h>
#include "NuMicro.h"
#include "BMM150.h"

void BMM150_WriteByte(uint8_t LSM6DSL_reg, uint8_t BMM_data)
{
	uint8_t temp=0,n=0;
	while(1){
		temp=I2C_WriteByteOneReg(I2C0,0x10 ,  LSM6DSL_reg, BMM_data);
		n++;
		if(temp==BMM_data|n>10)
			break;
	}
}
//BMM150 READ BYTE
uint8_t BMM_ReadByte(uint8_t BMM_reg)
{
	 return I2C_ReadByteOneReg(I2C0,0x10,BMM_reg);
}

void BMM150_Read6Bytes(uint8_t BMM_reg,uint8_t* data)
{
	I2C_ReadMultiBytesOneReg(I2C0,0x10, BMM_reg,data, 6);
}

void Init_BMM150()
{
	printf("BMM150 init...............\n");
	I2C_WriteByteOneReg(I2C0,0x10,0x4B,0x01);
	I2C_WriteByteOneReg(I2C0,0x10,0x4C,0x38);	
	I2C_WriteByteOneReg(I2C0,0x10,0x4E,0x3F);	
}
uint8_t BMM_whoami()
{
	uint8_t ID=0;
	ID=BMM_ReadByte(BMM050_CHIP_ID);
	printf("BMM's ID is:%x\n",ID);
	return ID;
}

int16_t Read_BMM150_X(void)
{
	uint8_t LoByte, HiByte;
	LoByte = BMM_ReadByte(0x42)>>1; // read Accelerometer X_Low  value
	HiByte = BMM_ReadByte(0x43); // read Accelerometer X_High value
	return((HiByte<<8) | LoByte);
}

int16_t Read_BMM150_Y(void)
{
	uint8_t LoByte, HiByte;
	LoByte = BMM_ReadByte(0x44)>>1; // read Accelerometer X_Low  value
	HiByte = BMM_ReadByte(0X45); // read Accelerometer X_High value
	return ((HiByte<<8) | LoByte);
}

int16_t Read_BMM150_Z(void)
{
	uint8_t LoByte, HiByte;
	LoByte = BMM_ReadByte(0x46); //
	HiByte = BMM_ReadByte(0x47); // 
	return ((HiByte<<8) | LoByte);
}
void BMM150Test(){
	int16_t magx,magy,magz; 
	magx=Read_BMM150_X();
	magy=Read_BMM150_Y();
	magz=Read_BMM150_Z();
	printf("Magn : %6d, %6d, %6d\n", magx, magy, magz);
}

void SPIReadMagnHandler(){
	SPI_WRITE_TX(SPI0,1);
	SPI_WRITE_TX(SPI0,2);
	SPI_WRITE_TX(SPI0,3);
	SPI_WRITE_TX(SPI0,4);
	SPI_WRITE_TX(SPI0,5);
	SPI_WRITE_TX(SPI0,6);
}

/////////////////////I2C read orderhandler/////////////////////////////
void I2C1readMagn(uint8_t* data){
	/*data[0]=BMM_ReadByte(0x43);
	data[1]=BMM_ReadByte(0x42);
	data[2]=BMM_ReadByte(0x45);
	data[3]=BMM_ReadByte(0x44);
	data[4]=BMM_ReadByte(0x47);
	data[5]=BMM_ReadByte(0x46);*/
	BMM150_Read6Bytes(0x42,data);  //LoByte is at first.
}