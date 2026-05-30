/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define MAJOR 0  //App Major version of Application
#define MINOR 1  //App MInor version of Application
#define SIZE 5
#define POLY  0x1021          /* crc-ccitt mask */
#define WF_PKT_SIZE 128
#define APP_START_ADDRESS  0x08020000
#define CHUNK_SIZE 1024 // Process 1 KB at a time
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern void sevensegment_update(uint16_t value1,uint16_t value2,uint16_t value3);
extern uint16_t g_update_crc(uint16_t crc, uint8_t newbyte);
extern uint16_t g_CRC_calc(uint8_t *ptr, uint32_t pkt_size);
extern void Enable_PWM(void);
extern void Disable_PWM(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ABORT_LED_IO1_SET		HAL_GPIO_WritePin(ABORT_LED_IO1_GPIO_Port,ABORT_LED_IO1_Pin,GPIO_PIN_SET)
#define ABORT_LED_IO1_RESET 	HAL_GPIO_WritePin(ABORT_LED_IO1_GPIO_Port,ABORT_LED_IO1_Pin,GPIO_PIN_RESET)
#define ABORT_LED_IO2_SET		HAL_GPIO_WritePin(ABORT_LED_IO2_GPIO_Port,ABORT_LED_IO2_Pin,GPIO_PIN_SET)
#define ABORT_LED_IO2_RESET 	HAL_GPIO_WritePin(ABORT_LED_IO2_GPIO_Port,ABORT_LED_IO2_Pin,GPIO_PIN_RESET)
#define READY_LED_IO1_SET		HAL_GPIO_WritePin(READY_LED_IO1_GPIO_Port,READY_LED_IO1_Pin,GPIO_PIN_SET)
#define READY_LED_IO1_RESET 	HAL_GPIO_WritePin(READY_LED_IO1_GPIO_Port,READY_LED_IO1_Pin,GPIO_PIN_RESET)
#define READY_LED_IO2_SET		HAL_GPIO_WritePin(READY_LED_IO2_GPIO_Port,READY_LED_IO2_Pin,GPIO_PIN_SET)
#define READY_LED_IO2_RESET 	HAL_GPIO_WritePin(READY_LED_IO2_GPIO_Port,READY_LED_IO2_Pin,GPIO_PIN_RESET)
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define SET 1
#define RESET 0
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ABORT_STATUS_Pin GPIO_PIN_2
#define ABORT_STATUS_GPIO_Port GPIOE
#define READY_INIT_STATUS_Pin GPIO_PIN_4
#define READY_INIT_STATUS_GPIO_Port GPIOE
#define PREP_STATUS_Pin GPIO_PIN_6
#define PREP_STATUS_GPIO_Port GPIOE
#define DAC1_Trigger_Pin GPIO_PIN_0
#define DAC1_Trigger_GPIO_Port GPIOF
#define DAC2_Trigger_Pin GPIO_PIN_1
#define DAC2_Trigger_GPIO_Port GPIOF
#define DAC1_RESET_Pin GPIO_PIN_2
#define DAC1_RESET_GPIO_Port GPIOF
#define DAC2_RESET_Pin GPIO_PIN_3
#define DAC2_RESET_GPIO_Port GPIOF
#define SEG3_2_Pin GPIO_PIN_4
#define SEG3_2_GPIO_Port GPIOF
#define SEG3_3_Pin GPIO_PIN_5
#define SEG3_3_GPIO_Port GPIOF
#define DIS_CH1_OUT_Pin GPIO_PIN_6
#define DIS_CH1_OUT_GPIO_Port GPIOF
#define DIS_CH2_OUT_Pin GPIO_PIN_7
#define DIS_CH2_OUT_GPIO_Port GPIOF
#define DIS_CH3_OUT_Pin GPIO_PIN_8
#define DIS_CH3_OUT_GPIO_Port GPIOF
#define DIS_CH4_OUT_Pin GPIO_PIN_9
#define DIS_CH4_OUT_GPIO_Port GPIOF
#define DIS_CH5_OUT_Pin GPIO_PIN_10
#define DIS_CH5_OUT_GPIO_Port GPIOF
#define RS422_2_DE_RE_Pin GPIO_PIN_3
#define RS422_2_DE_RE_GPIO_Port GPIOC
#define RS422_LOOP_IN4_Pin GPIO_PIN_0
#define RS422_LOOP_IN4_GPIO_Port GPIOA
#define RS422_LOOP_IN3_Pin GPIO_PIN_1
#define RS422_LOOP_IN3_GPIO_Port GPIOA
#define SPI1_CS1_Pin GPIO_PIN_4
#define SPI1_CS1_GPIO_Port GPIOA
#define SPI1_CS2_Pin GPIO_PIN_4
#define SPI1_CS2_GPIO_Port GPIOC
#define SEG3_LE_Pin GPIO_PIN_0
#define SEG3_LE_GPIO_Port GPIOB
#define SEG2_BC_Pin GPIO_PIN_1
#define SEG2_BC_GPIO_Port GPIOB
#define SEG1_BC_Pin GPIO_PIN_2
#define SEG1_BC_GPIO_Port GPIOB
#define ROTRY_ADD0_Pin GPIO_PIN_11
#define ROTRY_ADD0_GPIO_Port GPIOF
#define ROTRY_ADD1_Pin GPIO_PIN_12
#define ROTRY_ADD1_GPIO_Port GPIOF
#define ROTRY_ADD2_Pin GPIO_PIN_13
#define ROTRY_ADD2_GPIO_Port GPIOF
#define ROTRY_OE1_Pin GPIO_PIN_14
#define ROTRY_OE1_GPIO_Port GPIOF
#define ROTRY_OE2_Pin GPIO_PIN_15
#define ROTRY_OE2_GPIO_Port GPIOF
#define ROTRY_OE3_Pin GPIO_PIN_0
#define ROTRY_OE3_GPIO_Port GPIOG
#define ROTRY_OE4_Pin GPIO_PIN_1
#define ROTRY_OE4_GPIO_Port GPIOG
#define SEG1_1_Pin GPIO_PIN_7
#define SEG1_1_GPIO_Port GPIOE
#define SEG1_2_Pin GPIO_PIN_8
#define SEG1_2_GPIO_Port GPIOE
#define SEG1_3_Pin GPIO_PIN_9
#define SEG1_3_GPIO_Port GPIOE
#define SEG1_4_Pin GPIO_PIN_10
#define SEG1_4_GPIO_Port GPIOE
#define SEG2_1_Pin GPIO_PIN_11
#define SEG2_1_GPIO_Port GPIOE
#define SEG2_2_Pin GPIO_PIN_12
#define SEG2_2_GPIO_Port GPIOE
#define SEG2_3_Pin GPIO_PIN_13
#define SEG2_3_GPIO_Port GPIOE
#define SEG2_4_Pin GPIO_PIN_14
#define SEG2_4_GPIO_Port GPIOE
#define SEG3_1_Pin GPIO_PIN_15
#define SEG3_1_GPIO_Port GPIOE
#define WD_SET2_Pin GPIO_PIN_9
#define WD_SET2_GPIO_Port GPIOD
#define WD_SET1_Pin GPIO_PIN_10
#define WD_SET1_GPIO_Port GPIOD
#define WD_SET0_Pin GPIO_PIN_11
#define WD_SET0_GPIO_Port GPIOD
#define ROTRY_MUX_IN1_Pin GPIO_PIN_2
#define ROTRY_MUX_IN1_GPIO_Port GPIOG
#define ROTRY_MUX_IN2_Pin GPIO_PIN_3
#define ROTRY_MUX_IN2_GPIO_Port GPIOG
#define ROTRY_MUX_IN3_Pin GPIO_PIN_4
#define ROTRY_MUX_IN3_GPIO_Port GPIOG
#define ROTRY_MUX_IN4_Pin GPIO_PIN_5
#define ROTRY_MUX_IN4_GPIO_Port GPIOG
#define SEG3_4_Pin GPIO_PIN_6
#define SEG3_4_GPIO_Port GPIOG
#define SEG1_LE_Pin GPIO_PIN_7
#define SEG1_LE_GPIO_Port GPIOG
#define SEG2_LE_Pin GPIO_PIN_8
#define SEG2_LE_GPIO_Port GPIOG
#define READY_LED_IO2_Pin GPIO_PIN_6
#define READY_LED_IO2_GPIO_Port GPIOC
#define ABORT_LED_IO2_Pin GPIO_PIN_7
#define ABORT_LED_IO2_GPIO_Port GPIOC
#define SPI3_CS_Pin GPIO_PIN_15
#define SPI3_CS_GPIO_Port GPIOA
#define DISCRETE_CH3_STS_Pin GPIO_PIN_5
#define DISCRETE_CH3_STS_GPIO_Port GPIOD
#define RS422_1_DE_RE_Pin GPIO_PIN_7
#define RS422_1_DE_RE_GPIO_Port GPIOD
#define RS422_LOOP_IN1_Pin GPIO_PIN_11
#define RS422_LOOP_IN1_GPIO_Port GPIOG
#define RS422_LOOP_IN2_Pin GPIO_PIN_12
#define RS422_LOOP_IN2_GPIO_Port GPIOG
#define SEG3_BC_Pin GPIO_PIN_4
#define SEG3_BC_GPIO_Port GPIOB
#define ABORT_LED_IO1_Pin GPIO_PIN_8
#define ABORT_LED_IO1_GPIO_Port GPIOB
#define READY_LED_IO1_Pin GPIO_PIN_9
#define READY_LED_IO1_GPIO_Port GPIOB
#define DIS_CH2_IN_Pin GPIO_PIN_1
#define DIS_CH2_IN_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
#define NUM_OF_DWORDS 7
#define WF_PKT_SIZE 128



#define SLOT_A_START_ADDRESS 0x08020000UL
#define SLOT_A_END_ADDRESS   0x0807FFFFUL

#define METADATA_BASE_ADDRESS 0x080E0000UL

#define EXT_FLASH_VERSION      0xE00
#define INTERNAL_FLASH_VERSION 0xC00

#define BOOT_METADATA_MAGIC 0x424F4F54534C4F54ULL


#pragma pack(push, 1)

typedef struct
{
    uint64_t signature;
    uint64_t version;
    uint64_t size;
    uint64_t crc;
} SlotMetadata;

typedef struct
{
    uint64_t magic;
    SlotMetadata slot_a;
} BootMetadata;

#pragma pack(pop)


extern uint32_t post_start_time;

uint16_t g_CRC_calc_continue(uint8_t *ptr,
                             uint32_t pkt_size,
                             uint16_t crc);

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint16_t CRC_WORD;
    uint16_t endByte;
}Sty_in_Boot;

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint16_t CRC_WORD;
    uint16_t endByte;
}FlashErase;

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint16_t SOFT_VER;
    uint16_t CRC_WORD;
    uint16_t endByte;
}AppSigDword;

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint32_t appCode_size;
    uint16_t CRC_WORD;
    uint16_t endByte;
}AppCrcVerify;

#pragma pack(1)
typedef struct
{
   uint8_t StartMsg;
   uint8_t MsgID;
   uint8_t packetSize;
   uint8_t Dword_cnt;
   uint64_t Dword[NUM_OF_DWORDS];
   uint16_t CRC_WORD;
   uint16_t EndMsg;
}BinFileTx;

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint16_t FTU_TestNo;
    uint16_t CRC_WORD;
    uint16_t endByte;
}ReadDataExtFlash;

#pragma pack(1)
typedef struct
{
    uint8_t startByte;
    uint8_t MsgID;
    uint16_t packetSize;
    uint16_t extFlash_CRC;
    uint16_t TstPointFileCnt;
    uint16_t CRC_WORD;
    uint16_t endByte;
}ExtFlashCRC;

typedef struct
{
	uint8_t DIS_1:4;
	uint8_t DIS_2:4;
	uint8_t DIS_3:4;
}Hex_dis;
extern Hex_dis g_Hex_dis;

#pragma pack(1)
typedef struct {
	    uint8_t StartMsg;
        uint8_t MsgID;
	    uint8_t packetSize;
	    uint16_t FTU_Test_No;
	    uint8_t Signal_Type;
	    uint8_t Signal_Injection_Point;
	    uint8_t TransmissionMode;
	    float Safe_engage_Time_Secs;
	    uint8_t Invert_Flag;
	    float Slat_Input;
	    uint8_t No_of_Repeats;
	    float Time_Between_Repeats;
	    float Amplitude_Vpp_mm;
	    float C0;
	    float C1;
	    float Start_FreqHz_PulseWidth;
	    float End_FreqHz;
	    float Freq_StepHz;
	    float DwellDuration_Secs;
	    float Wo;
	    float Wf;
	    float Sweep_Duration;
	    uint8_t CH1 : 1;
	    uint8_t CH2 : 1;
	    uint8_t CH3 : 1;
	    uint8_t CH4 : 1;
	    uint8_t CH5 : 1;
	    uint8_t CH6 : 1;
	    uint8_t CH7 : 1;
	    uint8_t CH8 : 1;
	    uint8_t waveformSelect1 : 1;
	    uint8_t waveformSelect2 : 1;
	    uint8_t waveformSelect3 : 1;
	    uint8_t waveformSelect4 : 1;
	    uint16_t CRC_WORD;
	    uint16_t EndMsg;
    } TestPoint_main;
extern TestPoint_main g_TestPoint_main;
extern TestPoint_main g_ReceiveTestPoint;

typedef enum
{
	POWER_ON_SELF_TEST_PHASE =1,
	TEST_POINT_SELECTION_PHASE,
	PREPARATION_PHASE,
	TEST_IN_PROGRESS_PHASE,
	ABORT_PHASE1,
	ABORT_PHASE2,
	ABORT_PHASE3,
	ABORT_PHASE4,
}MainLoopSwitch;

typedef union {
    float f;
    uint32_t u;
} floatToUint;

/*
 * Bootloader single-slot reference
 * =================================
 *
 * This bootloader now uses only Slot A. Slot B is not used for programming or boot.
 *
 * 1. Flash map used by this bootloader
 * ------------------------------------
 * 0x08000000 - 0x0801FFFF : bootloader image
 * 0x08020000 - 0x0807FFFF : application image (Slot A only)
 * 0x08080000 - 0x080FFFFF : reserved / unused by this boot flow
 * 0x080E0000 - ...        : boot metadata area used by this bootloader
 *
 * STM32L496 flash page size = 2 KB (0x800).
 * Slot A is in Bank 1 and starts at page 64.
 *
 * 2. What makes an application a valid Slot A image
 * -------------------------------------------------
 * The application must be linked for 0x08020000. The following three settings inside
 * the application project must always match:
 * - linker FLASH ORIGIN = 0x08020000
 * - APP_START_ADDRESS   = 0x08020000
 * - vector table offset = 0x00020000
 *
 * If these do not match the physical flash address where the image is written, the
 * bootloader jump may succeed but the application will fault very early because the
 * vector table and absolute addresses will be wrong.
 *
 * 3. How the bootloader decides whether to boot the application
 * -------------------------------------------------------------
 * The bootloader always checks only Slot A at 0x08020000.
 *
 * A "valid application" means:
 * - the first word at 0x08020000 looks like a RAM stack pointer
 * - the second word at 0x08020004 points back inside the Slot A address range
 *
 * This sanity check is done by is_valid_application() in main.c.
 * If Slot A is valid, goto_application() jumps to it.
 * If Slot A is not valid, the bootloader stays in boot mode.
 *
 * 4. How the bootloader programs the application
 * ----------------------------------------------
 * The programming destination is always Slot A.
 *
 * During erase/program flow:
 * - erase starts from 0x08020000
 * - the Slot A flash pages are erased
 * - incoming firmware bytes are written to Slot A only
 * - CRC is calculated for Slot A only
 * - metadata is updated for Slot A only
 *
 * There is no inactive-slot selection anymore.
 *
 * 5. What bootMagic is and why it exists
 * --------------------------------------
 * BOOT_METADATA_MAGIC = 0x424F4F54534C4F54ULL, which is the ASCII text "BOOTSLOT".
 *
 * This value is stored in the metadata block and is used as the first validity check.
 * It tells the bootloader:
 * - this metadata block was initialized using the expected format
 * - the stored Slot A metadata may be trusted
 *
 * If the magic does not match:
 * - metadata is treated as blank or invalid
 * - metadata_load_or_default() creates a safe default in RAM
 * - the bootloader still programs Slot A
 *
 * Without bootMagic, erased flash (0xFF) or corrupted contents could be misread as
 * valid metadata.
 *
 * 6. Boot metadata format stored at 0x080E0000
 * --------------------------------------------
 * main.c defines:
 *
 * typedef struct
 * {
 *     uint64_t signature;
 *     uint64_t version;
 *     uint64_t size;
 *     uint64_t crc;
 * } SlotMetadata;
 *
 * typedef struct
 * {
 *     uint64_t magic;
 *     SlotMetadata slot_a;
 * } BootMetadata;
 *
 * Meaning of each field:
 * - magic            : must equal BOOT_METADATA_MAGIC
 * - slot_a.signature : application signature word
 * - slot_a.version   : application version
 * - slot_a.size      : programmed application size in bytes
 * - slot_a.crc       : CRC calculated for the Slot A image
 *
 * 7. Signature word used for the application
 * ------------------------------------------
 * The signature accepted from the GUI is stored only in:
 * - metadata.slot_a.signature
 *
 * When an APP_SIG_DWORD packet is received:
 * - the received signature is written to slot_a.signature
 * - the received version is written to slot_a.version
 *
 * The code also defines:
 * - App_Sig_Dword[8] = "FTUAPP00"
 *
 * That array is a local constant/pattern used by the bootloader code. The metadata
 * value that matters for the programmed image is metadata.slot_a.signature.
 *
 * 8. CRC and size stored for the application
 * ------------------------------------------
 * After programming completes:
 * - the image size is stored in metadata.slot_a.size
 * - the calculated CRC is stored in metadata.slot_a.crc
 *
 * This metadata belongs only to Slot A in the current design.
 *
 * 9. Practical procedure for programming through GUI
 * --------------------------------------------------
 * To produce a valid image for this bootloader:
 * - build the application for 0x08020000
 * - send erase/program packets through the GUI
 * - the bootloader writes the image into Slot A only
 * - APP_SIG_DWORD updates signature/version for Slot A
 * - APP_CRC_VERIFY updates size/crc for Slot A
 * - on next boot, the bootloader checks Slot A and jumps to it if valid
 *
 * 10. Important operating rule
 * ----------------------------
 * Any application image loaded through this bootloader must be linked for
 * 0x08020000. A binary linked for some other base address must not be programmed
 * through this Slot A-only boot flow.
 */

//----serial Flash---------------
extern void SerialFlash_Init();
extern uint8_t SerialFlash_IsWriteBusy();
extern void SerialFlash_WriteEnable();
extern void SerialFlash_WriteByte(uint8_t data, uint32_t address);
extern uint8_t SerialFlash_ReadByte(uint32_t address);
extern void SerialFlash_ChipErase(void);
extern void SerialFlash_SectorErase(uint8_t sector);
extern uint16_t SerialFlash_ReadID(void);
extern uint32_t Sector_Start(uint8_t sector);
extern uint8_t ReadStatusRegister_1(void);
//----------serial Flash End------------

extern uint16_t waveFormTableCH1[WF_PKT_SIZE];
extern uint16_t waveFormTableCH2[WF_PKT_SIZE];
extern uint16_t waveFormTableCH3[WF_PKT_SIZE];
extern uint16_t waveFormTableCH4[WF_PKT_SIZE];
extern uint8_t loopSwitch;
extern uint8_t g_TPS_Flag;

extern uint8_t g_waveform_tx_Flag;
extern uint8_t g_counter;
extern uint32_t g_Dwel_counter;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
