/*Version: 1.0 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define RS422_1_TX_EN 	HAL_GPIO_WritePin(RS422_1_DE_RE_GPIO_Port,RS422_1_DE_RE_Pin,GPIO_PIN_SET);
#define RS422_1_RX_EN 	HAL_GPIO_WritePin(RS422_1_DE_RE_GPIO_Port,RS422_1_DE_RE_Pin,GPIO_PIN_RESET);

#define RS422_2_TX_EN 	HAL_GPIO_WritePin(RS422_2_DE_RE_GPIO_Port,RS422_2_DE_RE_Pin,GPIO_PIN_SET);
#define RS422_2_RX_EN 	HAL_GPIO_WritePin(RS422_2_DE_RE_GPIO_Port,RS422_2_DE_RE_Pin,GPIO_PIN_RESET);
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
extern uint8_t tx1_data[SIZE];
extern uint8_t tx2_data[SIZE];
extern uint8_t rx1_data[SIZE];
extern uint8_t rx2_data[SIZE];
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern void uartLoopBack_test(uint8_t value);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

