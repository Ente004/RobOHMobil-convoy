/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IR_Empfaenger_R_Pin GPIO_PIN_0
#define IR_Empfaenger_R_GPIO_Port GPIOA
#define IR_Empfaenger_L_Pin GPIO_PIN_1
#define IR_Empfaenger_L_GPIO_Port GPIOA
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define Motor_R_Pin GPIO_PIN_4
#define Motor_R_GPIO_Port GPIOA
#define Motor_L_Pin GPIO_PIN_5
#define Motor_L_GPIO_Port GPIOA
#define Distanz_R_Pin GPIO_PIN_6
#define Distanz_R_GPIO_Port GPIOA
#define Distanz_L_Pin GPIO_PIN_7
#define Distanz_L_GPIO_Port GPIOA
#define SDA_Distanz_L_Pin GPIO_PIN_8
#define SDA_Distanz_L_GPIO_Port GPIOA
#define SCL_Distanz_L_Pin GPIO_PIN_9
#define SCL_Distanz_L_GPIO_Port GPIOA
#define Status_R_Pin GPIO_PIN_10
#define Status_R_GPIO_Port GPIOA
#define Status_G_Pin GPIO_PIN_11
#define Status_G_GPIO_Port GPIOA
#define Status_B_Pin GPIO_PIN_12
#define Status_B_GPIO_Port GPIOA
#define SCL_Distanz_R_Pin GPIO_PIN_13
#define SCL_Distanz_R_GPIO_Port GPIOA
#define SDA_Distanz_R_Pin GPIO_PIN_14
#define SDA_Distanz_R_GPIO_Port GPIOA
#define IR_Beacon_CLK_Pin GPIO_PIN_15
#define IR_Beacon_CLK_GPIO_Port GPIOA
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_8
#define LD2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
