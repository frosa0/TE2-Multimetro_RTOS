/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Rx_R_Pin GPIO_PIN_14
#define Rx_R_GPIO_Port GPIOC
#define Rc1_R_Pin GPIO_PIN_15
#define Rc1_R_GPIO_Port GPIOC
#define Ri_R_Pin GPIO_PIN_1
#define Ri_R_GPIO_Port GPIOA
#define Cref_C_Pin GPIO_PIN_3
#define Cref_C_GPIO_Port GPIOA
#define Cx_C_Pin GPIO_PIN_4
#define Cx_C_GPIO_Port GPIOA
#define Ri_C_Pin GPIO_PIN_5
#define Ri_C_GPIO_Port GPIOA
#define Hook_PROCESSING_Pin GPIO_PIN_12
#define Hook_PROCESSING_GPIO_Port GPIOB
#define Hook_DIAGNOSTIC_Pin GPIO_PIN_13
#define Hook_DIAGNOSTIC_GPIO_Port GPIOB
#define Hook_GUI_Pin GPIO_PIN_14
#define Hook_GUI_GPIO_Port GPIOB
#define Hook_DISPLAY_Pin GPIO_PIN_15
#define Hook_DISPLAY_GPIO_Port GPIOB
#define Hook_IDLE_Pin GPIO_PIN_8
#define Hook_IDLE_GPIO_Port GPIOA
#define Encoder_PUSH_Pin GPIO_PIN_12
#define Encoder_PUSH_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
