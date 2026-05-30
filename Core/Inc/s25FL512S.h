/*Version: 1.0 */
/*
 * s25FL512S.h
 *
 *  Created on: Jan 23, 2024
 *      Author:
 */

#ifndef S25FL512S_H_
#define S25FL512S_H_

/*--------------------Memory size   ------------------------------------------
 512Mbits = 512*1024*1024   = 536,870,912 bits
 512Mbits = 512*1024*1024/8 = 67,108,864 bytes = 0x3FFFFFFh 18 bit addresses
 512Mbits in sectors of 256kbytes = 256*1024 = 262,144 bytes per sector
 Sector_0           0 to 262,143              0x0 to 0x3ffff
 Sector_1     262,144 to 524,287              0x40000 to 0x7ffff
 Sector_2     524,288 to 786,431              0x80000 to 0xbffff
 Sector_3     786,432 to 1,048,575            0xc0000 to 0xfffff
 Sector_n   n*262,144 to (n*262,144)+262,143  n*0x4000 to (n*0x4000)+3ffff
-----------------------------------------------------------------------------*/

//--------------------------S25FL512S Commands----------------------------//
/* Reset Operations */
#define S25FL512S_SOFTWARE_RESET_CMD                    0xF0
#define S25FL512S_MODE_BIT_RESET_CMD                    0xFF
/* Identification Operations */
#define S25FL512S_READ_ID_CMD							0x90
#define S25FL512S_READ_ID_CMD2                          0x9F
/* Register Operations */
#define S25FL512S_READ_STATUS_REG1_CMD                  0x05
#define S25FL512S_READ_STATUS_REG2_CMD                  0x07
#define S25FL512S_READ_CONFIGURATION_REG1_CMD           0x35
#define S25FL512S_WRITE_STATUS_CMD_REG_CMD              0x01
#define S25FL512S_WRITE_DISABLE_CMD                     0x04
#define S25FL512S_WRITE_ENABLE_CMD                      0x06
#define S25FL512S_CLEAR_STATUS_REG1_CMD                 0x30
#define S25FL512S_READ_AUTOBOOT_REG_CMD                 0x14
#define S25FL512S_WRITE_AUTOBOOT_REG_CMD                0x15
#define S25FL512S_READ_BANK_REG_CMD                     0x16
#define S25FL512S_WRITE_BANK_REG_CMD                    0x17
#define S25FL512S_ACCESS_BANK_REG_CMD                   0xB9
#define S25FL512S_READ_DATA_LEARNING_PATTERN_CMD        0x41
#define S25FL512S_PGM_NV_DATA_LEARNING_REG_CMD          0x43
#define S25FL512S_WRITE_VOL_DATA_LEARNING_REG_CMD       0x4A
/* Read Operations */
#define S25FL512S_READ_CMD                              0x03
#define S25FL512S_READ_4_BYTE_ADDR_CMD                  0x13
#define S25FL512S_FAST_READ_CMD                         0x0B
#define S25FL512S_FAST_READ_4_BYTE_ADDR_CMD             0x0C
#define S25FL512S_FAST_READ_DDR_CMD                     0x0D
#define S25FL512S_FAST_READ__DDR_4_BYTE_ADDR_CMD        0x0E
/* Program Operations */
#define S25FL512S_PAGE_PROG_CMD                         0x02
#define S25FL512S_PAGE_PROG_4_BYTE_ADDR_CMD             0x12
/* Erase Operations */
#define S25FL512S_SECTOR_ERASE_CMD                      0xD8
#define S25FL512S_SECTOR_ERASE_4_BYTE_ADDR_CMD          0xDC
#define S25FL512S_BULK_ERASE_CMD                        0x60
#define S25FL512S_BULK_ERASE_ALTERNATE_CMD              0xC7
//----------------------------------------------------------------------------//



void SerialFlash_Init();
// Function - SerialFlash_IsWriteBusy() by reading bit 0 of status reg 1
uint8_t SerialFlash_IsWriteBusy();
// Function - SerialFlash_WriteEnable()
void SerialFlash_WriteEnable();
// Function - SerialFlash_WriteByte() Function that writes a single byte
// Input: Data to be written and the address to which to store the data
void SerialFlash_WriteByte(uint8_t data, uint32_t address);
// Function - SerialFlash_ReadByte() reads the byte from the address
// Input: address to be read. Output: byte data from the address
uint8_t SerialFlash_ReadByte(uint32_t address);
// Function - SerialFlash_ChipErase() sends Chip Erase command
//            This takes about 2 minutes
void SerialFlash_ChipErase(void);
// Function - SerialFlash_SectorErase() sends Sector Erase command
//            This takes about ?? minutes
void SerialFlash_SectorErase(uint8_t sector);
// Function - SerialFlash_ReadID() reads the CHIP ID
// Input: none Output: ID byte value
uint16_t SerialFlash_ReadID(void);
// Function - calculate sector starting address
//            Sector_n   starts at n*262,144 and ends at (n*262,144)+262,143
uint32_t Sector_Start(uint8_t sector);
uint8_t ReadStatusRegister_1(void);

#endif /* S25FL512S_H_ */
