/*Version: 1.0 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "usbd_core.h"
#include "s25FL512S.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MAJOR 0  //Major version of bootloader
#define MINOR 1  //MInor version of bootloader
#define MAJOR_APP 0  //Major version of bootloader
#define MINOR_APP 1  //MInor version of bootloader
#define POLY  0x1021          /* crc-ccitt mask */
#define CHUNK_SIZE 1024 // Process 1 KB at a time

#define BOOT_ADDRESS_START 0x08000000UL
#define BOOT_ADDRESS_END   0x0801FFFFUL
#define SLOT_IMAGE_SIZE_BYTES (384UL * 1024UL)
#define METADATA_PAGE         448U
#define METADATA_PAGE_COUNT   1U

#define STAY_IN_BOOTLOADER_MODE 5
#define CONTROLLER_IN_BOOTMODE  6
#define ERR_MSG_NO_RESPONSE 7
#define APPLICATION_CODE 8
#define BOOTLOADER_CODE 9
#define UC_FLASH_ERASE 10
#define UC_ERASE 11
#define UC_FLASH_ERASE_SUCCESS 12
#define UC_FLASH_ERASE_FAIL 13
#define APPLICATION_DATA 14
#define APP_WR_DATA_PKT_ACK_PASS 15
#define APP_WR_DATA_PKT_ACK_FAIL 16
#define APP_WRT_TIMEOUT 17
#define APP_SIG_DWORD 18
#define APP_SIG_DWORD_WR_PASS 19
#define APP_SIG_DWORD_WR_FAIL 20
#define APP_SIG_WRT_TIMEOUT 21
#define APP_CRC_VERIFY 22
#define APP_CRC_WRT_TIMEOUT 23
#define ACK_TP_PKT_ID 25
#define SEND_TP_PKT_ID 30
#define EXT_FLASH_ERASE 31
#define EXT_FLASH_ERASE_SUCESS 32
#define EXT_FLASH_ERASE_FAIL 33
#define EXT_FLASH_NOT_EMPTY 34
#define CRC_CHECK_FAIL 35
#define READ_EXT_FLASH 36
#define READ_EXT_FLASH_DATA 37
#define EXT_FLASH_CRC	38
#define EXT_FLASH_CRC_TX	39

#define TP_NUMBER_INVALID 40
#define SOH 0x24
#define INVALID_START_MSG 41
#define INVALID_END_MSG 42
#define INVALID_PACKET_SIZE 43
#define INVALID_MSG_ID 44
#define EOT 0x233B
#define TESTPOINT_SLOT_SIZE 2048


uint32_t post_start_time = 0;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const uint8_t BL_Version[2] = { MAJOR, MINOR };
HAL_StatusTypeDef ret;
uint32_t SectorError;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void goto_application(void);
static void deinit_bootloader_context(void);
static uint8_t is_valid_application(uint32_t app_base);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t App_Sig_Dword[8] = "FTUAPP00";
uint16_t g_update_crc(uint16_t crc,uint8_t newbyte);
uint16_t g_CRC_calc(uint8_t *ptr, uint32_t pkt_size);
uint8_t isFlashEmpty(uint32_t address, uint32_t size);
uint16_t calculate_flash_crc(uint32_t start_addr, uint32_t total_size);
void Enable_PWM(void);
void Disable_PWM(void);
void postCheck(void);
void Brigthness_Control(void);
void ExtDac_Config(void);

uint16_t g_CRC_word = 0;
uint16_t g_Boot_Code_CRC = 0;
uint16_t g_APP_Code_CRC = 0;
uint16_t g_checkCRC_word = 0;
uint8_t g_tx_buffer[64] = {0};
uint8_t g_CTRL_MODE = BOOTLOADER_CODE;
uint32_t g_app_size = 0;


uint8_t loopSwitch = 0;
uint8_t g_TPS_Flag = 0;
uint8_t g_waveform_tx_Flag = 0;
uint8_t g_counter = 0;
uint32_t g_Dwel_counter = 0;


uint16_t flash_M_Device_ID = 0;
uint8_t flashStatusReg_1 = 0;
uint32_t address = 0;
extern USBD_HandleTypeDef hUsbDeviceFS;

BinFileTx g_BinFileTx;
Sty_in_Boot g_Sty_in_Boot;
FlashErase g_FlashErase;
AppSigDword g_AppSigDword;
AppCrcVerify g_AppCrcVerify;
uint32_t g_app_start_address = SLOT_A_START_ADDRESS;
TestPoint_main g_TestPoint_main;
TestPoint_main g_ReceiveTestPoint;
floatToUint g_floatToUint;
ReadDataExtFlash g_ReadDataExtFlash;
ExtFlashCRC g_ExtFlashCRC;

static void metadata_load_or_default(BootMetadata *metadata)
{
	memcpy(metadata, (const void *)METADATA_BASE_ADDRESS, sizeof(BootMetadata));
	if (metadata->magic != BOOT_METADATA_MAGIC) {
		memset(metadata, 0xFF, sizeof(BootMetadata));
		metadata->magic = BOOT_METADATA_MAGIC;
	}
}

static HAL_StatusTypeDef metadata_write(const BootMetadata *metadata)
{
	FLASH_EraseInitTypeDef erase_init = {0};
	uint32_t sector_error = 0U;
	const uint64_t *src = (const uint64_t *)metadata;
	HAL_StatusTypeDef status = HAL_OK;

	HAL_FLASH_Unlock();

	erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
	erase_init.Banks = FLASH_BANK_2;
	erase_init.Page = METADATA_PAGE;
	erase_init.NbPages = METADATA_PAGE_COUNT;

	status = HAL_FLASHEx_Erase(&erase_init, &sector_error);
	if ((status == HAL_OK) && (sector_error == 0xFFFFFFFFU)) {
		//		for (uint32_t i = 0; i < (sizeof(BootMetadata) / sizeof(uint64_t)); i++) {
		//			status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
		//					METADATA_BASE_ADDRESS + (i * sizeof(uint64_t)),
		//					src[i]);
		//			if (status != HAL_OK) {
		//				break;
		//			}
		//		}
		for (uint32_t i = 0;
				i < (sizeof(BootMetadata) / sizeof(uint64_t));
				i++)
		{
			status = HAL_FLASH_Program(
					FLASH_TYPEPROGRAM_DOUBLEWORD,
					METADATA_BASE_ADDRESS +
					(i * sizeof(uint64_t)),
					src[i]);

			if (status != HAL_OK)
			{
				break;
			}

			uint64_t verify =
					*(volatile uint64_t *)
					(METADATA_BASE_ADDRESS +
							(i * sizeof(uint64_t)));

			if(verify != src[i])
			{
				status = HAL_ERROR;
				break;
			}
		}
	} else {
		status = HAL_ERROR;
	}

	HAL_FLASH_Lock();
	return status;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	post_start_time = HAL_GetTick();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	//Enble watch dog timer
	TIM4->CCR1 = 150;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_SPI3_Init();
	MX_TIM1_Init();
	MX_USB_DEVICE_Init();
	MX_TIM4_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_ADC1_Init();
	MX_SPI1_Init();
	MX_SPI2_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM5_Init();
	MX_IWDG_Init();
	/* USER CODE BEGIN 2 */
	// Start timer
	HAL_TIM_Base_Start_IT(&htim5);

	HAL_SPI_Init(&hspi3);

	HAL_UART_Init(&huart1);
	HAL_UART_Init(&huart2);
	//uart receive interrupt config
	HAL_UART_Receive_IT(&huart1, rx1_data, SIZE);
	HAL_UART_Receive_IT(&huart2, rx2_data, SIZE);
	TIM1->CCR1 = 100;
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	TIM2->CCR2 = 100;
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	TIM3->CCR4 = 100;
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	Brigthness_Control();
	ExtDac_Config();
	//-----------------------------------------Calculating CRC of BootApplication ---------------------
	g_CRC_word = g_CRC_calc((uint8_t *)BOOT_ADDRESS_START, 0x1FFFF);
	g_Boot_Code_CRC = g_CRC_word;
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		HAL_IWDG_Refresh(&hiwdg);
		if(g_CTRL_MODE == APPLICATION_CODE)
		{
			BootMetadata metadata;
			uint32_t boot_address = SLOT_A_START_ADDRESS;
			uint64_t readData = 0ULL;
			uint8_t l_arr[8];

			metadata_load_or_default(&metadata);
			readData = metadata.slot_a.signature;
			memcpy(l_arr, &readData, sizeof(l_arr));
			//			if ((memcmp(App_Sig_Dword, l_arr, sizeof(l_arr)) == 0) ||
			//					(is_valid_application(boot_address) == SET))
			if ((memcmp(App_Sig_Dword, l_arr, sizeof(l_arr)) == 0) &&
					(is_valid_application(boot_address) == SET))
			{
				postCheck();

				if(loopSwitch == SET)
				{
					Disable_PWM();

					TIM1->CCR1 = 0;
					TIM2->CCR2 = 0;
					TIM3->CCR4 = 0;

					goto_application();
				}
			}
			else
			{
				postCheck();
				g_CTRL_MODE = BOOTLOADER_CODE;
			}
		}
		if (g_USB_CDC_flag == SET)
		{
			uint16_t volatile l_rx_loop = 0;
			g_USB_CDC_flag = 0;

			uint16_t pktSize = g_rx_buffer[2];

			if((pktSize < 7) || (pktSize > sizeof(g_rx_buffer)))
			{
			    g_tx_buffer[0] = '$';
			    g_tx_buffer[1] = INVALID_PACKET_SIZE;
			    g_tx_buffer[2] = 7;
			    g_tx_buffer[3] = 0;
			    g_tx_buffer[4] = 0;
			    g_tx_buffer[5] = ';';
			    g_tx_buffer[6] = '#';

			    g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

			    g_tx_buffer[3] = g_CRC_word & 0xFF;
			    g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

			    CDC_Transmit_FS(&g_tx_buffer[0], 7);

			    continue;
			}

			if(g_rx_buffer[0] != SOH)
			{
				g_tx_buffer[0] = '$';
				g_tx_buffer[1] = INVALID_START_MSG;
				g_tx_buffer[2] = 7;
				g_tx_buffer[3] = 0;
				g_tx_buffer[4] = 0;
				g_tx_buffer[5] = ';';
				g_tx_buffer[6] = '#';

				g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

				g_tx_buffer[3] = g_CRC_word & 0xFF;
				g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

				CDC_Transmit_FS(&g_tx_buffer[0], 7);

				// break;
				continue ;
			}

			uint16_t endMsg =
					((g_rx_buffer[pktSize - 1] << 8) |
							g_rx_buffer[pktSize - 2]);

			if(endMsg != EOT)
			{
				g_tx_buffer[0] = '$';
				g_tx_buffer[1] = INVALID_END_MSG;
				g_tx_buffer[2] = 7;
				g_tx_buffer[3] = 0;
				g_tx_buffer[4] = 0;
				g_tx_buffer[5] = ';';
				g_tx_buffer[6] = '#';

				g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

				g_tx_buffer[3] = g_CRC_word & 0xFF;
				g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

				CDC_Transmit_FS(&g_tx_buffer[0], 7);

				continue ;
			}

			switch (g_rx_buffer[l_rx_loop+1])
			{
			case STAY_IN_BOOTLOADER_MODE:
			{
				memcpy(&g_Sty_in_Boot,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_Sty_in_Boot));


				g_checkCRC_word = g_Sty_in_Boot.CRC_WORD;
				g_Sty_in_Boot.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_Sty_in_Boot,g_Sty_in_Boot.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					HAL_TIM_Base_Stop_IT(&htim5);
					g_CTRL_MODE = BOOTLOADER_CODE;
					//					HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CONTROLLER_IN_BOOTMODE;
					g_tx_buffer[2] = 11;
					g_tx_buffer[3] = (g_Boot_Code_CRC & 0xFF);
					g_tx_buffer[4] = (g_Boot_Code_CRC>>8) & 0xFF;
					g_tx_buffer[5] = MAJOR;
					g_tx_buffer[6] = MINOR;
					g_tx_buffer[7] = 0;
					g_tx_buffer[8] = 0;
					g_tx_buffer[9] = ';';
					g_tx_buffer[10] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],11);
					g_tx_buffer[7] = g_CRC_word & 0xFF;
					g_tx_buffer[8] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 11);
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case UC_FLASH_ERASE:
			{
				uint8_t l_pass_count = 0;
				memcpy(&g_FlashErase,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_FlashErase));
				g_checkCRC_word = g_FlashErase.CRC_WORD;
				g_FlashErase.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_FlashErase,g_FlashErase.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					uint32_t start_page;
					uint32_t page_count;

					g_app_start_address = SLOT_A_START_ADDRESS;
					start_page = 64U;
					page_count = 192U;

					HAL_FLASH_Unlock();

					FLASH_EraseInitTypeDef EraseInitStruct;

					EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
					EraseInitStruct.Banks = FLASH_BANK_1;
					EraseInitStruct.Page = start_page;
					EraseInitStruct.NbPages = page_count;

					ret = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
					if (ret == HAL_OK) {
						l_pass_count++;
					} else {
						l_pass_count--;
					}
					if ((l_pass_count != 1) && (SectorError != 0xFFFFFFFFU)) {
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = UC_FLASH_ERASE_FAIL;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}
					else if((l_pass_count == 1) && (SectorError == 0xFFFFFFFFU))
					{
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = UC_FLASH_ERASE_SUCCESS;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}
					HAL_FLASH_Lock();
					g_CRC_word = g_CRC_calc((uint8_t *)g_app_start_address, SLOT_IMAGE_SIZE_BYTES - 1U);
					g_APP_Code_CRC = g_CRC_word;
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case APPLICATION_DATA:
			{
				uint8_t l_pass_count = 0;
				memcpy(&g_BinFileTx,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_BinFileTx));
				g_checkCRC_word = g_BinFileTx.CRC_WORD;
				g_BinFileTx.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_BinFileTx,g_BinFileTx.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					HAL_FLASH_Unlock();
					for(uint8_t i =0;i<=g_BinFileTx.Dword_cnt;i++)
					{
						if((g_app_start_address + 8) > SLOT_A_END_ADDRESS)
						{
							HAL_FLASH_Lock();

							g_tx_buffer[0] = '$';
							g_tx_buffer[1] = APP_WR_DATA_PKT_ACK_FAIL;
							g_tx_buffer[2] = 7;
							g_tx_buffer[3] = 0;
							g_tx_buffer[4] = 0;
							g_tx_buffer[5] = ';';
							g_tx_buffer[6] = '#';

							g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

							g_tx_buffer[3] = g_CRC_word & 0xFF;
							g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

							CDC_Transmit_FS(&g_tx_buffer[0], 7);

							break;
						}
						ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, g_app_start_address, g_BinFileTx.Dword[i]);
						if (ret == HAL_OK) {
							l_pass_count++;
						} else {
							l_pass_count--;
						}
						g_app_start_address= g_app_start_address+8;
					}
					HAL_FLASH_Lock();
					if(l_pass_count == (g_BinFileTx.Dword_cnt+1))
					{
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = APP_WR_DATA_PKT_ACK_PASS;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}
					else
					{
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = APP_WR_DATA_PKT_ACK_FAIL;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case APP_SIG_DWORD:
			{
				BootMetadata metadata;
				uint64_t l_AppSigDword = 0;
				uint8_t l_pass_count = 0;
				memcpy(&l_AppSigDword,App_Sig_Dword,8);
				memcpy(&g_AppSigDword,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_AppSigDword));
				g_checkCRC_word = g_AppSigDword.CRC_WORD;
				g_AppSigDword.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_AppSigDword,g_AppSigDword.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					metadata_load_or_default(&metadata);
					metadata.slot_a.signature = l_AppSigDword;
					metadata.slot_a.version = g_AppSigDword.SOFT_VER;
					ret = metadata_write(&metadata);
					if (ret == HAL_OK) {
						l_pass_count = 2;
					}
					if (l_pass_count == 2) {
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = APP_SIG_DWORD_WR_PASS;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					} else {
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = APP_SIG_DWORD_WR_FAIL;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}
					g_CRC_word = g_CRC_calc((uint8_t *)SLOT_A_START_ADDRESS, g_app_size/*40320*/);
					g_APP_Code_CRC = g_CRC_word;
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case APP_CRC_VERIFY:
			{
				BootMetadata metadata;
				uint8_t l_pass_count =0;
				memcpy(&g_AppCrcVerify,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_AppCrcVerify));
				g_checkCRC_word = g_AppCrcVerify.CRC_WORD;
				g_AppCrcVerify.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_AppCrcVerify,g_AppCrcVerify.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					g_app_size = g_AppCrcVerify.appCode_size;

					g_CRC_word = g_CRC_calc((uint8_t *)SLOT_A_START_ADDRESS, g_app_size);
					g_APP_Code_CRC = g_CRC_word;

					metadata_load_or_default(&metadata);
					metadata.slot_a.size = g_app_size;
					metadata.slot_a.crc = g_CRC_word;
					ret = metadata_write(&metadata);
					if (ret == HAL_OK) {
						l_pass_count = 2;
					}
					if (l_pass_count == 2) {
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = APP_CRC_VERIFY;
						g_tx_buffer[2] = 11;
						g_tx_buffer[3] = (g_APP_Code_CRC & 0xFF);
						g_tx_buffer[4] = (g_APP_Code_CRC>>8) & 0xFF;
						g_tx_buffer[5] = MAJOR_APP;
						g_tx_buffer[6] = MINOR_APP;
						g_tx_buffer[7] = 0;
						g_tx_buffer[8] = 0;
						g_tx_buffer[9] = ';';
						g_tx_buffer[10] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],11);
						g_tx_buffer[7] = g_CRC_word & 0xFF;
						g_tx_buffer[8] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 11);
					}
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case EXT_FLASH_ERASE:
			{
			    uint8_t l_status = 0;

			    memcpy(&g_FlashErase,
			           (uint8_t *)&g_rx_buffer[l_rx_loop],
			           sizeof(g_FlashErase));

			    g_checkCRC_word = g_FlashErase.CRC_WORD;

			    g_FlashErase.CRC_WORD = 0;

			    g_CRC_word =
			            g_CRC_calc((uint8_t*)&g_FlashErase,
			                       g_FlashErase.packetSize);

			    if(g_checkCRC_word == g_CRC_word)
			    {
			        // Erase external flash sector
			        SerialFlash_SectorErase(0);

			        uint32_t timeout = HAL_GetTick();

			        // Wait until erase complete
			        while(ReadStatusRegister_1() & 0x01)
			        {
			            if((HAL_GetTick() - timeout) > 5000)
			            {
			                break;
			            }
			        }

			        // Verify erase completed successfully
			        uint8_t erased = 1;

			        for(uint32_t i = 0; i < 256; i++)
			        {
			            if(SerialFlash_ReadByte(i) != 0xFF)
			            {
			                erased = 0;
			                break;
			            }
			        }

			        if(erased == 1)
			        {
			            l_status = 1;
			        }

			        // SUCCESS RESPONSE
			        if(l_status == 1)
			        {
			            g_tx_buffer[0] = '$';
			            g_tx_buffer[1] = EXT_FLASH_ERASE_SUCESS;
			            g_tx_buffer[2] = 7;
			            g_tx_buffer[3] = 0;
			            g_tx_buffer[4] = 0;
			            g_tx_buffer[5] = ';';
			            g_tx_buffer[6] = '#';

			            g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

			            g_tx_buffer[3] = g_CRC_word & 0xFF;

			            g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

			            CDC_Transmit_FS(&g_tx_buffer[0], 7);
			        }
			        else
			        {
			            // FAIL RESPONSE
			            g_tx_buffer[0] = '$';
			            g_tx_buffer[1] = EXT_FLASH_ERASE_FAIL;
			            g_tx_buffer[2] = 7;
			            g_tx_buffer[3] = 0;
			            g_tx_buffer[4] = 0;
			            g_tx_buffer[5] = ';';
			            g_tx_buffer[6] = '#';

			            g_CRC_word =
			                    g_CRC_calc(&g_tx_buffer[0],7);

			            g_tx_buffer[3] =
			                    g_CRC_word & 0xFF;

			            g_tx_buffer[4] =
			                    (g_CRC_word >> 8) & 0xFF;

			            CDC_Transmit_FS(&g_tx_buffer[0], 7);
			        }
			    }
			    else
			    {
			        // CRC FAIL RESPONSE
			        g_tx_buffer[0] = '$';
			        g_tx_buffer[1] = CRC_CHECK_FAIL;
			        g_tx_buffer[2] = 7;
			        g_tx_buffer[3] = 0;
			        g_tx_buffer[4] = 0;
			        g_tx_buffer[5] = ';';
			        g_tx_buffer[6] = '#';

			        g_CRC_word =
			                g_CRC_calc(&g_tx_buffer[0],7);

			        g_tx_buffer[3] =
			                g_CRC_word & 0xFF;

			        g_tx_buffer[4] =
			                (g_CRC_word >> 8) & 0xFF;

			        CDC_Transmit_FS(&g_tx_buffer[0], 7);
			    }

			    break;
			}
			case SEND_TP_PKT_ID:
			{
				uint8_t serializedData[sizeof(TestPoint_main)];
				uint8_t verifyData[sizeof(TestPoint_main)];
				memcpy(&g_TestPoint_main,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(g_TestPoint_main));

				if(g_TestPoint_main.MsgID != SEND_TP_PKT_ID)
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = INVALID_MSG_ID;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

					CDC_Transmit_FS(&g_tx_buffer[0], 7);

					break;
				}

				if(g_TestPoint_main.FTU_Test_No > 999)
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = TP_NUMBER_INVALID;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

					CDC_Transmit_FS(&g_tx_buffer[0], 7);

					break;
				}

				//				g_tx_buffer[1] = INVALID_START_MSG;
				//				CDC_Transmit_FS(&g_tx_buffer[0], 7);

				if(g_TestPoint_main.packetSize != sizeof(TestPoint_main))
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = INVALID_PACKET_SIZE;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

					CDC_Transmit_FS(&g_tx_buffer[0], 7);

					break;
				}

				if(g_TestPoint_main.StartMsg != SOH)
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = INVALID_START_MSG;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

					CDC_Transmit_FS(&g_tx_buffer[0], 7);

					break;
				}

				if(g_TestPoint_main.EndMsg != EOT)
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = INVALID_END_MSG;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);

					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;

					CDC_Transmit_FS(&g_tx_buffer[0], 7);

					break;
				}

				g_checkCRC_word = g_TestPoint_main.CRC_WORD;
				g_TestPoint_main.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_TestPoint_main,g_TestPoint_main.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					//sector select
					// here 2048 bytes of memory is allocated for each test point data size
					//	address = g_TestPoint_main.FTU_Test_No * 100;
					address = g_TestPoint_main.FTU_Test_No * TESTPOINT_SLOT_SIZE;
					// Serialize the structure
					memcpy(serializedData, &g_TestPoint_main, sizeof(TestPoint_main));

					//Verify flash empty status to write data
					if (isFlashEmpty(address, sizeof(TestPoint_main))) {
						// Write to memory
						for (uint16_t i = 0; i < sizeof(TestPoint_main); i++)
						{
							SerialFlash_WriteByte(serializedData[i], address + i);
						}
						//Verify the data
						for (uint16_t i = 0; i < sizeof(TestPoint_main); i++)
						{
							verifyData[i] = SerialFlash_ReadByte(address + i);
						}
						if (memcmp(serializedData, verifyData, sizeof(TestPoint_main)) == 0)
						{
							g_tx_buffer[0] = '$';
							g_tx_buffer[1] = ACK_TP_PKT_ID;
							g_tx_buffer[2] = 7;
							g_tx_buffer[3] = 0;
							g_tx_buffer[4] = 0;
							g_tx_buffer[5] = ';';
							g_tx_buffer[6] = '#';

							g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
							g_tx_buffer[3] = g_CRC_word & 0xFF;
							g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
							CDC_Transmit_FS(&g_tx_buffer[0], 7);
						}
						else
						{
							g_tx_buffer[0] = '$';
							g_tx_buffer[1] = ACK_TP_PKT_ID;
							g_tx_buffer[2] = 7;
							g_tx_buffer[3] = 1	;
							g_tx_buffer[4] = 0;
							g_tx_buffer[5] = ';';
							g_tx_buffer[6] = '#';

							g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
							g_tx_buffer[3] = g_CRC_word & 0xFF;
							g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
							CDC_Transmit_FS(&g_tx_buffer[0], 7);
						}
					}
					else
					{
						g_tx_buffer[0] = '$';
						g_tx_buffer[1] = EXT_FLASH_NOT_EMPTY;
						g_tx_buffer[2] = 7;
						g_tx_buffer[3] = 0;
						g_tx_buffer[4] = 0;
						g_tx_buffer[5] = ';';
						g_tx_buffer[6] = '#';

						g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
						g_tx_buffer[3] = g_CRC_word & 0xFF;
						g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
						CDC_Transmit_FS(&g_tx_buffer[0], 7);
					}

				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			case EXT_FLASH_CRC:
			{
			    uint16_t ExternalFlashCRC = 0;
			    uint16_t ExternalFlashVer = EXT_FLASH_VERSION;
			    uint16_t VerFlashCRC = 0;
			    uint16_t StoredCRC = 0;

			    memcpy(&g_ExtFlashCRC,
			           (uint8_t *)&g_rx_buffer[l_rx_loop],
			           sizeof(g_ExtFlashCRC));

			    g_checkCRC_word = g_ExtFlashCRC.CRC_WORD;

			    g_ExtFlashCRC.CRC_WORD = 0;

			    g_CRC_word =
			            g_CRC_calc((uint8_t*)&g_ExtFlashCRC,
			                       g_ExtFlashCRC.packetSize);

			    if(g_checkCRC_word == g_CRC_word)
			    {
			        // Calculate external flash CRC
			        ExternalFlashCRC =
			                calculate_flash_crc(0,
			                                    (1000 * 2048));

			        // Erase metadata sector
			        SerialFlash_SectorErase(20);

			        uint32_t timeout = HAL_GetTick();

			        while(ReadStatusRegister_1() & 0x01)
			        {
			            if((HAL_GetTick() - timeout) > 5000)
			            {
			                break;
			            }
			        }

			        // Store CRC
			        SerialFlash_WriteByte(
			                (ExternalFlashCRC & 0xFF),
			                (20 * 256 * 1024));

			        SerialFlash_WriteByte(
			                ((ExternalFlashCRC >> 8) & 0xFF),
			                (20 * 256 * 1024) + 1);

			        // Store version
			        SerialFlash_WriteByte(
			                (ExternalFlashVer & 0xFF),
			                (20 * 256 * 1024) + 2);

			        SerialFlash_WriteByte(
			                ((ExternalFlashVer >> 8) & 0xFF),
			                (20 * 256 * 1024) + 3);

			        // Readback verify CRC
			        StoredCRC =
			                (SerialFlash_ReadByte(
			                 (20 * 256 * 1024)) & 0xFF);

			        StoredCRC |=
			                ((SerialFlash_ReadByte(
			                 (20 * 256 * 1024) + 1) << 8)
			                 & 0xFF00);

			        // Readback verify version
			        VerFlashCRC =
			                (SerialFlash_ReadByte(
			                 (20 * 256 * 1024) + 2) & 0xFF);

			        VerFlashCRC |=
			                ((SerialFlash_ReadByte(
			                 (20 * 256 * 1024) + 3) << 8)
			                 & 0xFF00);

			        // Final verification
			        if((ExternalFlashCRC == StoredCRC) &&
			           (ExternalFlashVer == VerFlashCRC))
			        {
			            g_ExtFlashCRC.startByte = '$';

			            g_ExtFlashCRC.MsgID =
			                    EXT_FLASH_CRC_TX;

			            g_ExtFlashCRC.packetSize =
			                    sizeof(g_ExtFlashCRC);

			            g_ExtFlashCRC.extFlash_CRC =
			                    ExternalFlashCRC;

			            g_ExtFlashCRC.TstPointFileCnt = 0;

			            g_ExtFlashCRC.CRC_WORD = 0;

			            g_ExtFlashCRC.endByte = EOT;

			            g_CRC_word =
			                    g_CRC_calc(
			                        (uint8_t*)&g_ExtFlashCRC,
			                        g_ExtFlashCRC.packetSize);

			            g_ExtFlashCRC.CRC_WORD =
			                    g_CRC_word;

			            CDC_Transmit_FS(
			                    (uint8_t *)&g_ExtFlashCRC,
			                    sizeof(ExtFlashCRC));
			        }
			        else
			        {
			            g_tx_buffer[0] = '$';
			            g_tx_buffer[1] = EXT_FLASH_ERASE_FAIL;
			            g_tx_buffer[2] = 7;
			            g_tx_buffer[3] = 0;
			            g_tx_buffer[4] = 0;
			            g_tx_buffer[5] = ';';
			            g_tx_buffer[6] = '#';

			            g_CRC_word =
			                    g_CRC_calc(&g_tx_buffer[0],7);

			            g_tx_buffer[3] =
			                    g_CRC_word & 0xFF;

			            g_tx_buffer[4] =
			                    (g_CRC_word >> 8) & 0xFF;

			            CDC_Transmit_FS(&g_tx_buffer[0], 7);
			        }
			    }
			    else
			    {
			        g_tx_buffer[0] = '$';
			        g_tx_buffer[1] = CRC_CHECK_FAIL;
			        g_tx_buffer[2] = 7;
			        g_tx_buffer[3] = 0;
			        g_tx_buffer[4] = 0;
			        g_tx_buffer[5] = ';';
			        g_tx_buffer[6] = '#';

			        g_CRC_word =
			                g_CRC_calc(&g_tx_buffer[0],7);

			        g_tx_buffer[3] =
			                g_CRC_word & 0xFF;

			        g_tx_buffer[4] =
			                (g_CRC_word >> 8) & 0xFF;

			        CDC_Transmit_FS(&g_tx_buffer[0], 7);
			    }

			    break;
			}
			case READ_EXT_FLASH:
			{
				uint8_t verifyData[sizeof(TestPoint_main)];
				memcpy(&g_ReadDataExtFlash,(uint8_t *)&g_rx_buffer[l_rx_loop],sizeof(ReadDataExtFlash));
				g_checkCRC_word = g_ReadDataExtFlash.CRC_WORD;
				g_ReadDataExtFlash.CRC_WORD = 0;
				g_CRC_word = g_CRC_calc((uint8_t*)&g_ReadDataExtFlash,g_ReadDataExtFlash.packetSize);
				if(g_checkCRC_word == g_CRC_word)
				{
					//sector select
					// here 2048 bytes of memory is allocated for each test point data size
					//address = g_ReadDataExtFlash.FTU_TestNo * 100;
					address = g_ReadDataExtFlash.FTU_TestNo * TESTPOINT_SLOT_SIZE;

					//Verify flash empty status to read data
					if (!isFlashEmpty(address, sizeof(TestPoint_main))) {
						//read the data
						for (uint16_t i = 0; i < sizeof(TestPoint_main); i++)
						{
							verifyData[i] = SerialFlash_ReadByte(address + i);
						}
						memcpy(&g_ReceiveTestPoint,(uint8_t *)&verifyData[0],sizeof(TestPoint_main));
						g_ReceiveTestPoint.MsgID = READ_EXT_FLASH_DATA;
						g_ReceiveTestPoint.CRC_WORD = 0;
						g_CRC_word = g_CRC_calc((uint8_t*)&g_ReceiveTestPoint,g_ReceiveTestPoint.packetSize);
						g_ReceiveTestPoint.CRC_WORD = g_CRC_word;
						CDC_Transmit_FS((uint8_t *)&g_ReceiveTestPoint, sizeof(TestPoint_main));
					}
					else
					{
						memset(&g_ReceiveTestPoint,0,sizeof(TestPoint_main));
						g_ReceiveTestPoint.StartMsg = 0x24;
						g_ReceiveTestPoint.MsgID = READ_EXT_FLASH_DATA;
						g_ReceiveTestPoint.FTU_Test_No = 1001;
						g_ReceiveTestPoint.packetSize = sizeof(TestPoint_main);
						g_ReceiveTestPoint.EndMsg = EOT;
						g_CRC_word = g_CRC_calc((uint8_t*)&g_ReceiveTestPoint,g_ReceiveTestPoint.packetSize);
						g_ReceiveTestPoint.CRC_WORD = g_CRC_word;
						CDC_Transmit_FS((uint8_t *)&g_ReceiveTestPoint, sizeof(TestPoint_main));

					}
				}
				else
				{
					g_tx_buffer[0] = '$';
					g_tx_buffer[1] = CRC_CHECK_FAIL;
					g_tx_buffer[2] = 7;
					g_tx_buffer[3] = 0;
					g_tx_buffer[4] = 0;
					g_tx_buffer[5] = ';';
					g_tx_buffer[6] = '#';

					g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
					g_tx_buffer[3] = g_CRC_word & 0xFF;
					g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
					CDC_Transmit_FS(&g_tx_buffer[0], 7);
				}
				break;
			}
			default:
			{
				g_tx_buffer[0] = '$';
				g_tx_buffer[1] = ERR_MSG_NO_RESPONSE;
				g_tx_buffer[2] = 7;
				g_tx_buffer[3] = 0;
				g_tx_buffer[4] = 0;
				g_tx_buffer[5] = ';';
				g_tx_buffer[6] = '#';

				g_CRC_word = g_CRC_calc(&g_tx_buffer[0],7);
				g_tx_buffer[3] = g_CRC_word & 0xFF;
				g_tx_buffer[4] = (g_CRC_word >> 8) & 0xFF;
				CDC_Transmit_FS(&g_tx_buffer[0], 7);
				break;
			}
			}//switch close
			//					printf("in usb flag trigger UART\n");
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_LSI
			|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
static void goto_application(void)
{
	const uint32_t app_base = SLOT_A_START_ADDRESS;
	const uint32_t app_stack = *(volatile uint32_t *)app_base;
	const uint32_t app_reset = *(volatile uint32_t *)(app_base + 4U);
	void (*app_reset_handler)(void) = (void (*)(void))app_reset;

	deinit_bootloader_context();

	SysTick->CTRL = 0U;
	SysTick->LOAD = 0U;
	SysTick->VAL  = 0U;

	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk | SCB_ICSR_PENDSVCLR_Msk;
	SCB->CFSR = SCB->CFSR;
	SCB->HFSR = SCB->HFSR;
	SCB->DFSR = SCB->DFSR;
	SCB->MMFAR = 0U;
	SCB->BFAR = 0U;

	SCB->VTOR = app_base;
	__set_MSP(app_stack);
	__set_CONTROL(0U);
	__DSB();
	__ISB();
	__enable_irq();

	app_reset_handler();
}

static uint8_t is_valid_application(uint32_t app_base)
{
	const uint32_t app_stack = *(volatile uint32_t *)app_base;
	const uint32_t app_reset = *(volatile uint32_t *)(app_base + 4U);
	const uint32_t app_reset_aligned = app_reset & ~1UL;

	if ((app_stack < 0x20000000UL) || (app_stack > 0x20050000UL)) {
		return RESET;
	}
	if ((app_reset & 1UL) == 0U) {
		return RESET;
	}
	if ((app_reset_aligned < app_base) || (app_reset_aligned > SLOT_A_END_ADDRESS)) {
		return RESET;
	}

	return SET;
}

static void deinit_bootloader_context(void)
{
	__disable_irq();

	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	(void)USBD_Stop(&hUsbDeviceFS);
	(void)USBD_DeInit(&hUsbDeviceFS);

	HAL_TIM_Base_Stop_IT(&htim5);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);

	HAL_UART_DeInit(&huart1);
	HAL_UART_DeInit(&huart2);
	HAL_SPI_DeInit(&hspi1);
	HAL_SPI_DeInit(&hspi2);
	HAL_SPI_DeInit(&hspi3);
	HAL_ADC_DeInit(&hadc1);
	HAL_TIM_Base_DeInit(&htim5);
	HAL_TIM_PWM_DeInit(&htim1);
	HAL_TIM_PWM_DeInit(&htim2);
	HAL_TIM_PWM_DeInit(&htim3);
	HAL_TIM_PWM_DeInit(&htim4);

	for (uint32_t i = 0; i < 8; ++i)
	{
		NVIC->ICER[i] = 0xFFFFFFFFU;
		NVIC->ICPR[i] = 0xFFFFFFFFU;
	}

	HAL_RCC_DeInit();
	HAL_DeInit();
}

uint16_t g_CRC_calc_continue(uint8_t *ptr,
                             uint32_t pkt_size,
                             uint16_t crc)
{
    uint32_t i;

    for(i = 0; i < pkt_size; i++)
    {
        crc = g_update_crc(crc, *ptr++);
    }

    return crc;
}

uint16_t g_CRC_calc(uint8_t *ptr,
                    uint32_t pkt_size)
{
    return g_CRC_calc_continue(ptr,
                               pkt_size,
                               0xFFFF);
}

uint16_t g_update_crc(uint16_t crc,
                      uint8_t data)
{
    crc ^= ((uint16_t)data << 8);

    for(uint8_t i = 0; i < 8; i++)
    {
        if(crc & 0x8000)
        {
            crc = (crc << 1) ^ 0x1021;
        }
        else
        {
            crc <<= 1;
        }
    }

    return crc;
}

void Enable_PWM(void) {
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // Replace TIM_CHANNEL_1 with your channel
}

void Disable_PWM(void) {
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1); // Replace TIM_CHANNEL_1 with your channel
}
void Brigthness_Control(void)
{
	if(HAL_GPIO_ReadPin(DISCRETE_CH3_STS_GPIO_Port, DISCRETE_CH3_STS_Pin) == GPIO_PIN_RESET)
	{
		TIM1->CCR1 = 55;
		TIM2->CCR2 = 55;
		TIM3->CCR4 = 55;
	}
	else
	{
		TIM1->CCR1 = 90;
		TIM2->CCR2 = 90;
		TIM3->CCR4 = 90;
	}
}

// Interrupt Handling Function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t l_time_count = 0;
	l_time_count++;
	if(l_time_count == 05)
	{
		g_CTRL_MODE = APPLICATION_CODE;
		HAL_TIM_Base_Stop_IT(&htim5);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	__HAL_UART_CLEAR_IT(&huart1, UART_CLEAR_OREF);
	__HAL_UART_CLEAR_IT(&huart2, UART_CLEAR_OREF);

	if (huart == &huart1) {
		HAL_UART_Receive_IT(&huart1, rx1_data, 5);
	}
	if (huart == &huart2) {
		HAL_UART_Receive_IT(&huart2, rx2_data, 5);
	}
}

uint8_t isFlashEmpty(uint32_t address, uint32_t size) {
	// Iterate through all bytes in the specified range
	for (uint32_t i = 0; i < size; i++) {
		if (SerialFlash_ReadByte(address + i) != 0xFF) {
			return RESET; // Flash is not empty
		}
	}
	return SET; // Flash is empty
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	__disable_irq();

	Disable_PWM();

	TIM1->CCR1 = 0;
	TIM2->CCR2 = 0;
	TIM3->CCR4 = 0;

	while (1)
	{
		// Display FFF
		sevensegment_update(0xF,0xF,0xF);
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
