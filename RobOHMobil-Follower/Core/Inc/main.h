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

void Range_Data_Handler_R(int16_t _Range_R);
void Range_Data_Handler_L(int16_t _Range_L);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TOF_R_EN_Pin GPIO_PIN_2
#define TOF_R_EN_GPIO_Port GPIOA
#define TOF_L_EXTI_3_Pin GPIO_PIN_3
#define TOF_L_EXTI_3_GPIO_Port GPIOA
#define TOF_L_EXTI_3_EXTI_IRQn EXTI3_IRQn
#define TOF_L_EN_Pin GPIO_PIN_4
#define TOF_L_EN_GPIO_Port GPIOA
#define TOF_R_EXTI7_Pin GPIO_PIN_7
#define TOF_R_EXTI7_GPIO_Port GPIOA
#define TOF_R_EXTI7_EXTI_IRQn EXTI9_5_IRQn
#define IR_Blink_Pin GPIO_PIN_0
#define IR_Blink_GPIO_Port GPIOB
#define IR_Sensor_R_Pin GPIO_PIN_9
#define IR_Sensor_R_GPIO_Port GPIOA
#define IR_Sensor_L_Pin GPIO_PIN_10
#define IR_Sensor_L_GPIO_Port GPIOA
#define RGB_Rot_Pin GPIO_PIN_11
#define RGB_Rot_GPIO_Port GPIOA
#define RGB_Blau_Pin GPIO_PIN_4
#define RGB_Blau_GPIO_Port GPIOB
#define RGB_Gelb_Pin GPIO_PIN_5
#define RGB_Gelb_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_8
#define LD2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
