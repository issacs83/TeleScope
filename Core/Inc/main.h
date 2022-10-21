/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_gpio.h"

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
extern LPTIM_HandleTypeDef hlptim1;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CDS_En_Pin GPIO_PIN_0
#define CDS_En_GPIO_Port GPIOA
#define KEY_UP_Pin GPIO_PIN_1
#define KEY_UP_GPIO_Port GPIOA
#define KEY_UP_EXTI_IRQn EXTI0_1_IRQn
#define KEY_ON_Pin GPIO_PIN_4
#define KEY_ON_GPIO_Port GPIOA
#define KEY_ON_EXTI_IRQn EXTI4_15_IRQn
#define KEY_DOWN_Pin GPIO_PIN_5
#define KEY_DOWN_GPIO_Port GPIOA
#define KEY_DOWN_EXTI_IRQn EXTI4_15_IRQn
#define BATT_DET_Pin GPIO_PIN_6
#define BATT_DET_GPIO_Port GPIOA
#define BATT_DET_EXTI_IRQn EXTI4_15_IRQn
#define LED_PWM_Pin GPIO_PIN_7
#define LED_PWM_GPIO_Port GPIOA
#define L_SENSOR_Pin GPIO_PIN_1
#define L_SENSOR_GPIO_Port GPIOB
#define CH_SEL1_Pin GPIO_PIN_9
#define CH_SEL1_GPIO_Port GPIOA
#define CH_SEL2_Pin GPIO_PIN_10
#define CH_SEL2_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
