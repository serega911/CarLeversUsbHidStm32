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
#define BLINK_1_Pin GPIO_PIN_0
#define BLINK_1_GPIO_Port GPIOA
#define BLINK_2_Pin GPIO_PIN_1
#define BLINK_2_GPIO_Port GPIOA
#define MARKERS_Pin GPIO_PIN_2
#define MARKERS_GPIO_Port GPIOA
#define LOW_BEAM_Pin GPIO_PIN_3
#define LOW_BEAM_GPIO_Port GPIOA
#define HIGH_BEAM_Pin GPIO_PIN_4
#define HIGH_BEAM_GPIO_Port GPIOA
#define FOG_FRONT_Pin GPIO_PIN_5
#define FOG_FRONT_GPIO_Port GPIOA
#define FOG_REAR_Pin GPIO_PIN_6
#define FOG_REAR_GPIO_Port GPIOA
#define JOYSTICK_X_Pin GPIO_PIN_7
#define JOYSTICK_X_GPIO_Port GPIOA
#define JOYSTICK_Y_Pin GPIO_PIN_0
#define JOYSTICK_Y_GPIO_Port GPIOB
#define JOYSTICK_BTN_Pin GPIO_PIN_1
#define JOYSTICK_BTN_GPIO_Port GPIOB
#define WIPERS_INT_Pin GPIO_PIN_3
#define WIPERS_INT_GPIO_Port GPIOB
#define WIPERS_LO_Pin GPIO_PIN_4
#define WIPERS_LO_GPIO_Port GPIOB
#define WIPERS_HI_Pin GPIO_PIN_5
#define WIPERS_HI_GPIO_Port GPIOB
#define WIPERS_REAR_Pin GPIO_PIN_6
#define WIPERS_REAR_GPIO_Port GPIOB
#define SPRAY_FRONT_Pin GPIO_PIN_7
#define SPRAY_FRONT_GPIO_Port GPIOB
#define SPRAY_REAR_Pin GPIO_PIN_8
#define SPRAY_REAR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
