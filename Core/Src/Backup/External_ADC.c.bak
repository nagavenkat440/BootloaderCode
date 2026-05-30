/*Version: 1.0 */
/*
 * External_ADC.c
 *
 *  Created on: Dec 3, 2024
 *      Author:
 */
#include "main.h"
#include "spi.h"
#include "External_ADC.h"
#include "string.h"



ExtReceive ADC_ExtReceive;
ManualMode ADC_ManualMode_Bits;
ExtADC_Value g_ExtADC_Value;

void ExADC_ReadChannel(uint8_t channel){

	ADC_ManualMode_Bits.Set_mode = 0x01;
	ADC_ManualMode_Bits.Prog_Enable = 0;
	ADC_ManualMode_Bits.CH_No = channel & 0x0F;
	ADC_ManualMode_Bits.IN_Range = 0;
	ADC_ManualMode_Bits.PWR_Mode = 0;
	ADC_ManualMode_Bits.IO_Enable = 0;
	ADC_ManualMode_Bits.IO_Status = 0;
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&ADC_ManualMode_Bits, (uint8_t*)&ADC_ExtReceive,1,0);
	memset((uint8_t*)&ADC_ManualMode_Bits,0,sizeof(ManualMode));
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&ADC_ManualMode_Bits, (uint8_t*)&ADC_ExtReceive,1,0);
	HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&ADC_ManualMode_Bits, (uint8_t*)&ADC_ExtReceive,1,0);

	switch (ADC_ExtReceive.ADC_CH_ID)
	{
	case (POSITIVE_5V):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + OFFSET_5V);
		break;
	}
	case (POSITIVE_3P3V):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + OFFSET_3P3V_ANA);
		break;
	}
	case (NEGATIVE_5V):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + OFFSET_N5V);
		break;
	}
	case (TEMPERATURE):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + (OFFSET_TEMP));
		break;
	}
	case (NEGATIVE_4P6V):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + OFFSET_N4P6);
		break;
	}
	case (POSITIVE_4P6V):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT) + OFFSET_4P6);
		break;
	}
	case (DAC_CH1):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH2):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH3):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH4):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH5):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH6):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH7):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	case (DAC_CH8):
	{
		g_ExtADC_Value.AnaVoltages[ADC_ExtReceive.ADC_CH_ID] =  ((ADC_ExtReceive.ADC_Data_RX * EXT_ADC_1BIT));
		break;
	}
	default:
		break;
	}
}


