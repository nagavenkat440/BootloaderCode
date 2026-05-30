/*Version: 1.0 */
/*
 * Display.c
 *
 *  Created on: 24-Feb-2024
 *      Author:
 */
#include "main.h"

Hex_dis g_Hex_dis;


void sevensegment_update(uint16_t value1,uint16_t value2,uint16_t value3);

void sevensegment_update(uint16_t value1,uint16_t value2,uint16_t value3)
{
	g_Hex_dis.DIS_3 = value3 & 0x0F;
	g_Hex_dis.DIS_2 = (value2) & 0x0F;
	g_Hex_dis.DIS_1 = (value1) & 0x0F;
	//Display-1
	HAL_GPIO_WritePin(SEG1_1_GPIO_Port,SEG1_1_Pin,(GPIO_PinState)((0x1&g_Hex_dis.DIS_1)));
	HAL_GPIO_WritePin(SEG1_2_GPIO_Port,SEG1_2_Pin, (GPIO_PinState)((0x2&g_Hex_dis.DIS_1) >> 1));
	HAL_GPIO_WritePin(SEG1_3_GPIO_Port,SEG1_3_Pin,(GPIO_PinState)((0x4&g_Hex_dis.DIS_1)>>2));
	HAL_GPIO_WritePin(SEG1_4_GPIO_Port,SEG1_4_Pin, (GPIO_PinState)((0x8&g_Hex_dis.DIS_1)>>3));
	HAL_GPIO_WritePin(SEG1_BC_GPIO_Port,SEG1_BC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SEG1_LE_GPIO_Port,SEG1_LE_Pin, GPIO_PIN_RESET);
	//Display-2
	HAL_GPIO_WritePin(SEG2_1_GPIO_Port,SEG2_1_Pin, (GPIO_PinState)((0x1&g_Hex_dis.DIS_2)));
	HAL_GPIO_WritePin(SEG2_2_GPIO_Port,SEG2_2_Pin, (GPIO_PinState)((0x2&g_Hex_dis.DIS_2) >> 1));
	HAL_GPIO_WritePin(SEG2_3_GPIO_Port,SEG2_3_Pin,(GPIO_PinState)((0x4&g_Hex_dis.DIS_2)>>2));
	HAL_GPIO_WritePin(SEG2_4_GPIO_Port,SEG2_4_Pin, (GPIO_PinState)((0x8&g_Hex_dis.DIS_2)>>3));
	HAL_GPIO_WritePin(SEG2_BC_GPIO_Port,SEG2_BC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SEG2_LE_GPIO_Port,SEG2_LE_Pin, GPIO_PIN_RESET);
	//Display-3
	HAL_GPIO_WritePin(SEG3_1_GPIO_Port,SEG3_1_Pin, (GPIO_PinState)((0x1&g_Hex_dis.DIS_3)));
	HAL_GPIO_WritePin(SEG3_2_GPIO_Port,SEG3_2_Pin, (GPIO_PinState)((0x2&g_Hex_dis.DIS_3) >> 1));
	HAL_GPIO_WritePin(SEG3_3_GPIO_Port,SEG3_3_Pin,(GPIO_PinState)((0x4&g_Hex_dis.DIS_3)>>2));
	HAL_GPIO_WritePin(SEG3_4_GPIO_Port,SEG3_4_Pin, (GPIO_PinState)((0x8&g_Hex_dis.DIS_3)>>3));
	HAL_GPIO_WritePin(SEG3_BC_GPIO_Port,SEG3_BC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SEG3_LE_GPIO_Port,SEG3_LE_Pin, GPIO_PIN_RESET);

}

