/*Version: 1.0 */
/*
 * Post.c
 *
 *  Created on: Dec 18, 2024
 *      Author: RK
 */

#include "main.h"
#include "Post.h"
#include "External_ADC.h"
#include "usart.h"
#include <string.h>
#include "tim.h"
#include "External_DAC.h"
#include "adc.h"


uint16_t calculate_flash_crc(uint32_t start_addr, uint32_t total_size);
void postCheck();
static void Check_POST_Timeout(void);
uint8_t TestRAM(void);


uint16_t post_CRC_word = 0;
POST_Status g_POST_Status;
POST_Status POST_SUCCESS = {
		.IntFlashVer = 1,
		.ExtFlashVer = 1,
		.IntFlashCRC = 1,
		.RS422_CH1_Status = 1,
		.RS422_CH2_Status = 1,
		.POSITIVE_5V = 1,
		.POSITIVE_3P3V = 1,
		.NEGATIVE_5V = 1,
		.NEGATIVE_4P6V = 1,
		.POSITIVE_4P6V = 1,
		.DAC_CH1 = 1,
		.DAC_CH2 = 1,
		.DAC_CH3 = 1,
		.DAC_CH4 = 1,
		.DAC_CH5 = 1,
		.DAC_CH6 = 1,
		.DAC_CH7 = 1,
		.DAC_CH8 = 1,
		.POSITIVE_DIGITAL3P3V = 1,
};


uint16_t calculate_flash_crc(uint32_t start_addr,
		uint32_t total_size)
{
	//uint8_t buffer[CHUNK_SIZE];

	static uint8_t buffer[CHUNK_SIZE];

	uint32_t remaining = total_size;
	uint32_t addr = start_addr;

	uint16_t crc = 0xFFFF;

	while (remaining > 0)
	{
		uint32_t chunk_size =
				(remaining > CHUNK_SIZE) ?
						CHUNK_SIZE :
						remaining;

		for (uint32_t i = 0; i < chunk_size; i++)
		{
			buffer[i] =
					SerialFlash_ReadByte(addr + i);
		}

		crc = g_CRC_calc_continue(buffer,
				chunk_size,
				crc);

		addr += chunk_size;

		remaining -= chunk_size;
	}

	return crc;
}

void postCheck()
{
	/*------------------ POWER ON SELF TEST-------------------------- */
	Disable_PWM();//Disable watch dog reset

	memset(&g_POST_Status,
			RESET,
			sizeof(g_POST_Status));

	// Optional but recommended
	g_TPS_Flag = RESET;
	loopSwitch = RESET;

//	uint32_t App_size_Addr = 0x080E0020;
//	uint32_t App_CRC_Addr = 0x080E0030;
//	uint32_t softVer_address = 0x080E0010;
//	uint64_t readData = 0;

	BootMetadata *meta =
	        (BootMetadata *)METADATA_BASE_ADDRESS;

	uint32_t appCodeSize =
	        (uint32_t)meta->slot_a.size;

	uint32_t AppCrcFlash =
	        (uint32_t)meta->slot_a.crc;

	uint32_t appVerFlash =
	        (uint32_t)meta->slot_a.version;

	uint16_t VerFlashCRC = 0;
	sevensegment_update(8,8,8);
	Enable_PWM();//Enables watch dog reset
	HAL_Delay(1000);
	Check_POST_Timeout();
	//-----------------------------------------Calculating CRC of ApplicationCode Flash ---------------------
	// Calculation of Microcontroller internal Flash CRC and Version check
	Disable_PWM();//Disable watch dog reset
//	readData = *(uint64_t *)App_size_Addr;
//	uint32_t appCodeSize = (uint32_t)readData;
//	readData = *(uint64_t *)App_CRC_Addr;
//	uint32_t AppCrcFlash = (uint32_t)readData;
//	readData = *(uint64_t *)softVer_address;
//	uint32_t appVerFlash = (uint32_t)readData;
	if(appVerFlash == INTERNAL_FLASH_VERSION)
	{
		g_POST_Status.IntFlashVer = SET;

		sevensegment_update(0xC,0,0);
	}
	else
	{
		g_POST_Status.IntFlashVer = RESET;
	}


	//	else
		//	{
	//		g_POST_Status.IntFlashVer = RESET;
	//	}
	//	post_CRC_word = g_CRC_calc((uint8_t *)APP_START_ADDRESS, appCodeSize);
	//	if(AppCrcFlash == post_CRC_word)
	//{
	//	g_POST_Status.IntFlashCRC = SET;
	//}
	//	else
	//	{
	//		g_POST_Status.IntFlashCRC = RESET;
	//	}

	post_CRC_word =
//			g_CRC_calc((uint8_t *)APP_START_ADDRESS,
//					appCodeSize);
			g_CRC_calc((uint8_t *)SLOT_A_START_ADDRESS,
								appCodeSize);

	if(AppCrcFlash == post_CRC_word)
	{
		g_POST_Status.IntFlashCRC = SET;
	}
	else
	{
		g_POST_Status.IntFlashCRC = RESET;
	}
	Enable_PWM();//Enables watch dog reset
	HAL_Delay(1000);
	Check_POST_Timeout();
	//External Flash Version check
	Disable_PWM();//Disable watch dog reset
	VerFlashCRC = (SerialFlash_ReadByte((20*256*1024)+2) & 0xFF);
	VerFlashCRC |= (SerialFlash_ReadByte((20*256*1024)+3) << 8) & 0xFF00;
	if(VerFlashCRC == EXT_FLASH_VERSION)
	{
		g_POST_Status.ExtFlashVer = SET;
		sevensegment_update(0xE,0,0);
	}
	else
	{
		g_POST_Status.ExtFlashVer = RESET;
	}

	Enable_PWM();//Enables watch dog reset
	HAL_Delay(1000);
	Check_POST_Timeout();

	//Abort, Ready Init LED control and Uart loopback check
	Disable_PWM();//Disable watch dog reset
	sevensegment_update(0xA,0xA,0xA);
	ABORT_LED_IO1_SET;
	ABORT_LED_IO2_SET;
	READY_LED_IO1_SET;
	READY_LED_IO2_SET;
	HAL_Delay(1000);
	Check_POST_Timeout();
	ABORT_LED_IO1_RESET;
	ABORT_LED_IO2_RESET;
	READY_LED_IO1_RESET;
	READY_LED_IO2_RESET;
	//RS422 loopback test enable
	uartLoopBack_test(SET);
	Check_POST_Timeout();
	if(memcmp(tx1_data,rx2_data,SIZE) == 0)
	{
		g_POST_Status.RS422_CH1_Status = SET;
	}
	else{
		g_POST_Status.RS422_CH1_Status = RESET;
	}
	if(memcmp(tx2_data,rx1_data,SIZE) == 0)
	{
		g_POST_Status.RS422_CH2_Status = SET;
	}
	else{
		g_POST_Status.RS422_CH2_Status = RESET;
	}
	//RS422 LOOPBACK test disable
	uartLoopBack_test(RESET);
	Check_POST_Timeout();
	Enable_PWM();//Enables watch dog reset

	//Read 3.3V digital voltage through internal ADC
	//HAL_ADC_Start(&hadc1); // start the adc

	if(HAL_ADC_Start(&hadc1) != HAL_OK)
	{
		g_POST_Status.POSITIVE_DIGITAL3P3V = RESET;
	}
	//HAL_ADC_PollForConversion(&hadc1, 100); // poll for conversion

	if(HAL_ADC_PollForConversion(&hadc1,100) == HAL_OK)
	{
		g_ExtADC_Value.Dig3P3 =
				((float)HAL_ADC_GetValue(&hadc1)
						* (3.3/4096))
						+ OFFSET_3V3_DIG;
	}
	else
	{
		g_POST_Status.POSITIVE_DIGITAL3P3V = RESET;
	}

	//	temp = HAL_ADC_GetValue(&hadc1); // get the adc value
	//	g_ExtADC_Value.Dig3P3 = ((float)HAL_ADC_GetValue(&hadc1) * (3.3/4096)) + OFFSET_3V3_DIG;
	//HAL_ADC_Stop(&hadc1); // stop adc
	if(HAL_ADC_Stop(&hadc1) != HAL_OK)
	{
	    g_POST_Status.POSITIVE_DIGITAL3P3V = RESET;
	}
	//Internal Power Supplies and DAC loopback checks
	Disable_PWM();//Disable watch dog reset

	//int16_t zero_code = 2/0.001167 ; //((scaled_value + 2.40) / 4.78) * 4095;
	int16_t zero_code = (int16_t)(2.0f / 0.001167f);
	if(zero_code < 0)
	{
		zero_code += 0x8000;
	}
	zero_code <<= 4;
	AD9106_WriteRegister(ADDR_DAC1_CST,zero_code);
	AD9106_WriteRegister(ADDR_DAC2_CST,zero_code);
	AD9106_WriteRegister(ADDR_DAC3_CST,zero_code);
	AD9106_WriteRegister(ADDR_DAC4_CST,zero_code);
	AD9106_WriteRegister(0x001D, 0x0001);

	g_counter = RESET;
	g_Dwel_counter = RESET;
	//	ReconfigureTimer5(10240);
	//	HAL_Delay(1000);
	for (uint8_t i = 0; i <= 14; i++)
	{
		Check_POST_Timeout();

		ExADC_ReadChannel(i);
	}
	for(uint8_t i =0;i<15;i++)
	{
		Check_POST_Timeout();

		switch (i)
		{
		case POSITIVE_5V:
		{

			if((g_ExtADC_Value.AnaVoltages[i] <= 5.2) && (g_ExtADC_Value.AnaVoltages[i] >= 4.8))
			{
				g_POST_Status.POSITIVE_5V = SET;
			}
			else
			{
				g_POST_Status.POSITIVE_5V = RESET;
			}
			break;
		}
		case POSITIVE_3P3V:
		{
			if((g_ExtADC_Value.AnaVoltages[i] <= 3.7) && (g_ExtADC_Value.AnaVoltages[i] >= 3.1))
			{
				g_POST_Status.POSITIVE_3P3V = SET;
			}
			else
			{
				g_POST_Status.POSITIVE_3P3V = RESET;
			}
			break;
		}
		case NEGATIVE_5V:
		{
			if((g_ExtADC_Value.AnaVoltages[i] <= 5.2) && (g_ExtADC_Value.AnaVoltages[i] >= 4.8))
			{
				g_POST_Status.NEGATIVE_5V = SET;
			}
			else{
				g_POST_Status.NEGATIVE_5V = RESET;
			}
			i++;
			break;
		}
		case NEGATIVE_4P6V:
		{
			if((g_ExtADC_Value.AnaVoltages[i] <= 4.8) && (g_ExtADC_Value.AnaVoltages[i] >= 4.2))
			{
				g_POST_Status.NEGATIVE_4P6V = SET;
			}else{
				g_POST_Status.NEGATIVE_4P6V = RESET;
			}
			break;
		}
		case POSITIVE_4P6V:
		{
			if((g_ExtADC_Value.AnaVoltages[i] <= 4.8) && (g_ExtADC_Value.AnaVoltages[i] >= 4.2))
			{
				g_POST_Status.POSITIVE_4P6V = SET;
			}else{
				g_POST_Status.POSITIVE_4P6V = RESET;
			}
			break;
		}
		case DAC_CH1:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH1 = SET;
			}else
			{
				g_POST_Status.DAC_CH1 = RESET;
			}
			break;
		}
		case DAC_CH2:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH2 = SET;
			}else{
				g_POST_Status.DAC_CH2 = RESET;
			}
			break;
		}
		case DAC_CH3:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH3 = SET;
			}else
			{
				g_POST_Status.DAC_CH3 = RESET;
			}
			break;
		}
		case DAC_CH4:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH4 = SET;
			}else
			{
				g_POST_Status.DAC_CH4 = RESET;
			}
			break;
		}
		case DAC_CH5:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH5 = SET;
			}else
			{
				g_POST_Status.DAC_CH5 = RESET;
			}
			break;
		}
		case DAC_CH6:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH6 = SET;
			}else{
				g_POST_Status.DAC_CH6 = RESET;
			}
			break;
		}
		case DAC_CH7:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH7 = SET;
			}else
			{
				g_POST_Status.DAC_CH7 = RESET;
			}
			break;
		}
		case DAC_CH8:
		{
			if((g_ExtADC_Value.AnaVoltages[i] >= 0.45) && (g_ExtADC_Value.AnaVoltages[i] <= 0.55))
			{
				g_POST_Status.DAC_CH8 = SET;
			}else
			{
				g_POST_Status.DAC_CH8 = RESET;
			}
			break;
		}
		case POSITIVE_DIGITAL3P3V:
		{
			if((g_ExtADC_Value.Dig3P3 <= 3.7) &&(g_ExtADC_Value.Dig3P3  >= 3.1))
			{
				g_POST_Status.POSITIVE_DIGITAL3P3V = SET;
			}else{
				g_POST_Status.POSITIVE_DIGITAL3P3V = RESET;
			}
			break;
		}
		default:
		{
			break;
		}
		}
	}
	g_waveform_tx_Flag = RESET;
	g_counter = RESET;
	g_Dwel_counter = RESET;
	g_TestPoint_main.CH1 = 0;
	g_TestPoint_main.CH2 = 0;
	g_TestPoint_main.CH3 = 0;
	g_TestPoint_main.CH4 = 0;
	g_TestPoint_main.CH5 = 0;
	g_TestPoint_main.CH6 = 0;
	g_TestPoint_main.CH7 = 0;
	g_TestPoint_main.CH8 = 0;
	//all analog channels to zero volts
	AD9106_WriteRegister(ADDR_DAC1_CST,0xFFF0);
	AD9106_WriteRegister(ADDR_DAC2_CST,0xFFF0);
	AD9106_WriteRegister(ADDR_DAC3_CST,0xFFF0);
	AD9106_WriteRegister(ADDR_DAC4_CST,0xFFF0);
	AD9106_WriteRegister(0x001D, 0x0001);
	//enable rotary position movement with switch case enable
	//	g_TPS_Flag = SET;
	//	if (memcmp(&g_POST_Status, &POST_SUCCESS, sizeof(POST_Status)) == 0) {
	//		loopSwitch = SET;
	//	}
	//	else
	//	{
	//		sevensegment_update(0xF,0xF,0xF);
	//		//		loopSwitch = 0;
	//		//		g_TPS_Flag = RESET;
	//		loopSwitch = RESET;
	//	}

	if (memcmp(&g_POST_Status,
			&POST_SUCCESS,
			sizeof(POST_Status)) == 0)
	{
		g_TPS_Flag = SET;

		loopSwitch = SET;
	}
	else
	{
		g_TPS_Flag = RESET;

		loopSwitch = RESET;

		sevensegment_update(0xF,0xF,0xF);
	}

	Enable_PWM();//Enables watch dog reset

}

static void Check_POST_Timeout(void)
{
	if((HAL_GetTick() - post_start_time) > 10000)
	{
		Error_Handler();
	}
}
