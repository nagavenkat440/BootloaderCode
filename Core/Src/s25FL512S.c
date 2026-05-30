/*Version: 1.0 */
/*
 * s25FL512S.c
 *
 *  Created on: Jan 23, 2024
 *      Author:
 */

#include "main.h"
#include "s25FL512S.h"
#include "spi.h"
#include "tim.h"

#define CHIP_SELECT_SPI3_LOW		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define CHIP_SELECT_SPI3_HIGH	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)
#define SECTOR_SIZE 256*1024



//
//// Select sector
//address = Sector_Start(255);
//
//while(1){
//	// Write to memory
//	SerialFlash_WriteByte(write,address);
//	delay_ms(1);
//	// Read from memory
//	read = SerialFlash_ReadByte(address);
//	delay_ms(1);
//	// Display results
//	a = write;
//	b = read;
//	LongLongUnsignedToStrWithZeros(address,my_string);
//	Printout(U2,"Write:%u\tRead:%u\t at Address:%s\r\n",a,b,my_string);
//
//	write++;
//	address++;
//	delay_ms(1000);
//}

uint8_t spi_tx[64] = {0};
uint8_t spi_rx[64] = {0};

// Function - Initialise serial flash
//           write enable
//           S25FL512S accepts
//           MODE 0: CPOL=0 and CPHA=0 - use this mode
void SerialFlash_Init(){
	// Write enable
	SerialFlash_WriteEnable();
}

// Function - SerialFlash_IsWriteBusy() by reading bit 0 of status reg 1
uint8_t SerialFlash_IsWriteBusy(){
	uint8_t temp;
	CHIP_SELECT_SPI3_LOW;
	spi_tx[0] = S25FL512S_READ_STATUS_REG1_CMD;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi3,&spi_rx[0], 1, HAL_MAX_DELAY);
	temp = spi_rx[0];
	CHIP_SELECT_SPI3_HIGH;
	return (temp&0x01);
}

// Function - SerialFlash_WriteEnable()
void SerialFlash_WriteEnable(){
	CHIP_SELECT_SPI3_LOW;
	spi_tx[0] = S25FL512S_WRITE_ENABLE_CMD;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 1, HAL_MAX_DELAY);
	CHIP_SELECT_SPI3_HIGH;
}
// Function - SerialFlash_WriteByte() Function that writes a single byte
// Input: Data to be written and the address to which to store the data
void SerialFlash_WriteByte(uint8_t data, uint32_t address){
	SerialFlash_WriteEnable();
	CHIP_SELECT_SPI3_LOW;
	spi_tx[0] = S25FL512S_PAGE_PROG_4_BYTE_ADDR_CMD;// 0x12
	spi_tx[1] = (address>> 24) & 0xFF; //address MSB
	spi_tx[2] = (address>> 16) & 0xFF;
	spi_tx[3] = (address>> 8) & 0xFF;
	spi_tx[4] = (address) & 0xFF; //address LSB
	spi_tx[5] = data;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 6, HAL_MAX_DELAY);
	CHIP_SELECT_SPI3_HIGH;
	// Wait for write end
	while(SerialFlash_IsWriteBusy());
}
// Function - SerialFlash_ReadByte() reads the byte from the address          //
// Input: address to be read. Output: byte data from the address              //
uint8_t SerialFlash_ReadByte(uint32_t address){
	CHIP_SELECT_SPI3_LOW;
	spi_tx[0] = S25FL512S_READ_4_BYTE_ADDR_CMD;// 0x13
	spi_tx[1] = (address>> 24) & 0xFF; //address MSB
	spi_tx[2] = (address>> 16) & 0xFF;
	spi_tx[3] = (address>> 8) & 0xFF;
	spi_tx[4] = (address) & 0xFF; //address LSB
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 5, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi3,&spi_rx[0], 1, HAL_MAX_DELAY);
	CHIP_SELECT_SPI3_HIGH;
	return spi_rx[0];
}

// Function - SerialFlash_ChipErase() sends Chip Erase command
//            This takes about 2 minutes
void SerialFlash_ChipErase(void){
	SerialFlash_WriteEnable();
	CHIP_SELECT_SPI3_LOW;
	spi_tx[0] = S25FL512S_BULK_ERASE_CMD;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 1, HAL_MAX_DELAY);//
	CHIP_SELECT_SPI3_HIGH;
	// Wait for write end
	while(SerialFlash_IsWriteBusy());
}

// Function - SerialFlash_SectorErase() sends Sector Erase command
//            This takes about ?? minutes
void SerialFlash_SectorErase(uint8_t sector){
	uint32_t my_address = 0;
	my_address =  sector*SECTOR_SIZE;
	SerialFlash_WriteEnable();
	spi_tx[0] = S25FL512S_SECTOR_ERASE_4_BYTE_ADDR_CMD; //0xDC
	spi_tx[1] =(my_address>> 24) & 0xFF; //address MSB
	spi_tx[2] =(my_address>> 16) & 0xFF;
	spi_tx[3] =(my_address>> 8) & 0xFF;
	spi_tx[4] =(my_address) & 0xFF; //address LSB
	CHIP_SELECT_SPI3_LOW;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0],5, HAL_MAX_DELAY);//
	CHIP_SELECT_SPI3_HIGH;
	// Wait for write end
	while(SerialFlash_IsWriteBusy());
}
// Function - SerialFlash_ReadID() reads the CHIP ID
// Input: none Output: ID byte value
uint16_t SerialFlash_ReadID(void){
	uint16_t temp;
	SerialFlash_WriteEnable();
	spi_tx[0] = S25FL512S_READ_ID_CMD;// 0x90
	spi_tx[1] = 0;
	spi_tx[2] = 0;
	spi_tx[3] = 0;
	CHIP_SELECT_SPI3_LOW;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 4, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi3, &spi_rx[0], 2, HAL_MAX_DELAY);
	CHIP_SELECT_SPI3_HIGH;
	temp = spi_rx[0];
	temp |= ((spi_rx[1]<<8) & 0xFF00);
	return temp;
}
// Read status register 1, bit 0 is busy signal
uint8_t ReadStatusRegister_1(void)
{
	SerialFlash_WriteEnable();
	spi_tx[0] = S25FL512S_READ_STATUS_REG1_CMD;// 0x05
	CHIP_SELECT_SPI3_LOW;
	HAL_SPI_Transmit(&hspi3, &spi_tx[0], 1, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi3, &spi_rx[0], 1, HAL_MAX_DELAY);
	CHIP_SELECT_SPI3_HIGH;
	return spi_rx[0];
}
//Printout(U2,"Status_Reg_1   :%u\r\n",a);
// Function - calculate sector starting address                               //
//            Sector_n   starts at n*262,144 and ends at (n*262,144)+262,143  //
uint32_t Sector_Start(uint8_t sector){                                      //
	uint32_t start_address = 0;                                            //
	start_address = sector*SECTOR_SIZE;                                              //
	return start_address;                                                       //
}                                                                             //
////////////////////////////////////////////////////////////////////////////////
