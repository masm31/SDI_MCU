/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define WaterValve_Pin GPIO_PIN_13
#define WaterValve_GPIO_Port GPIOC
#define temp_water_Pin GPIO_PIN_2
#define temp_water_GPIO_Port GPIOC
#define temp_amb_Pin GPIO_PIN_3
#define temp_amb_GPIO_Port GPIOC
#define Level_500_Pin GPIO_PIN_0
#define Level_500_GPIO_Port GPIOA
#define Level_500_EXTI_IRQn EXTI0_IRQn
#define Level_100_Pin GPIO_PIN_1
#define Level_100_GPIO_Port GPIOA
#define Level_100_EXTI_IRQn EXTI1_IRQn
#define Buzzer_Pin GPIO_PIN_2
#define Buzzer_GPIO_Port GPIOA
#define Level_000_Pin GPIO_PIN_3
#define Level_000_GPIO_Port GPIOA
#define Level_000_EXTI_IRQn EXTI3_IRQn
#define TFT_CS_Pin GPIO_PIN_4
#define TFT_CS_GPIO_Port GPIOA
#define Key4_Pin GPIO_PIN_4
#define Key4_GPIO_Port GPIOC
#define Key4_EXTI_IRQn EXTI4_IRQn
#define Key3_Pin GPIO_PIN_5
#define Key3_GPIO_Port GPIOC
#define Key3_EXTI_IRQn EXTI9_5_IRQn
#define PRSR1_Pin GPIO_PIN_0
#define PRSR1_GPIO_Port GPIOB
#define PRSR2_Pin GPIO_PIN_1
#define PRSR2_GPIO_Port GPIOB
#define Key1_Pin GPIO_PIN_12
#define Key1_GPIO_Port GPIOB
#define Key1_EXTI_IRQn EXTI15_10_IRQn
#define Key2_Pin GPIO_PIN_13
#define Key2_GPIO_Port GPIOB
#define Key2_EXTI_IRQn EXTI15_10_IRQn
#define DrainVave_Pin GPIO_PIN_15
#define DrainVave_GPIO_Port GPIOB
#define Pump_Pin GPIO_PIN_15
#define Pump_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
