/*
 * operation.c
 *
 *  Created on: Sep 17, 2022
 *      Author: issacs
 */
//https://github.com/LonelyWolf/stm32/blob/7ec39663ec37427e9d90c97582b0b36ce6e80a57/stm32l-dosfs/wolk.h

#include "operation.h"

Settings_TypeDef Settings;                  // Settings which stored in EEPROM
//
//extern LPTIM_HandleTypeDef hlptim1;
//
//uint8_t key_pos = 0;
//__IO uint8_t flag_stop_mode = 0;
//
//void Error_Handler(void)
//{
//  /* USER CODE BEGIN Error_Handler_Debug */
//  /* User can add his own implementation to report the HAL error return state */
//  __disable_irq();
//  while (1)
//  {
//  }
//  /* USER CODE END Error_Handler_Debug */
//}
//
//void SystemClock_Config(void)
//{
//  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
//  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
//  {
//  }
//  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
//  LL_RCC_HSI_Enable();
//
//   /* Wait till HSI is ready */
//  while(LL_RCC_HSI_IsReady() != 1)
//  {
//
//  }
//  LL_RCC_HSI_SetCalibTrimming(16);
//  LL_RCC_LSI_Enable();
//
//   /* Wait till LSI is ready */
//  while(LL_RCC_LSI_IsReady() != 1)
//  {
//
//  }
//  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_2);
//  LL_RCC_PLL_Enable();
//
//   /* Wait till PLL is ready */
//  while(LL_RCC_PLL_IsReady() != 1)
//  {
//
//  }
//  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
//  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
//  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
//  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
//
//   /* Wait till System clock is ready */
//  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
//  {
//
//  }
//  LL_SetSystemCoreClock(32000000);
//
//   /* Update the time base */
//  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
//  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);
//}

//int __attribute__((optimize("O0"))) CalDuty(uint8_t arg)
int	CalDuty(uint8_t arg)
{
	return ((PERIOD+1) - (((PERIOD+1) * arg) * 0.01f));
}
//
//void StopMode(void)
//{
////  GPIO_InitTypeDef GPIO_InitStruct;
////
////  /* Disable all GPIOs to reduce power */
////  HAL_GPIO_DeInit(GPIOA, KEY_UP_Pin);
////
////  /* Configure User push-button as external interrupt generator */
////  __HAL_RCC_GPIOC_CLK_ENABLE();
////  GPIO_InitStruct.Pin = KEY_UP_Pin;
////  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
////  GPIO_InitStruct.Pull = GPIO_NOPULL;
////  HAL_GPIO_Init(KEY_UP_GPIO_Port, &GPIO_InitStruct);
//
////  HAL_LPTIM_MspInit(&hlpuart1);
//
//  /* Suspend Tick increment to prevent wakeup by Systick interrupt.
//     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
//  HAL_SuspendTick();
//
//  /* We enable again the PWR peripheral */
//  __HAL_RCC_PWR_CLK_ENABLE();
//
//  /* Request to enter SLEEP mode */
//  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
//
////  SystemClock_Config_low();
//
//  /* Resume Tick interrupt if disabled prior to sleep mode entry*/
//  HAL_ResumeTick();
//
//  /* Reinitialize GPIOs */
////  MX_GPIO_Init();
//
//  /* Reinitialize LPUART1 */
////  MX_LPUART1_UART_Init();
//
//  /* Reinitialize LPTIM1 */
////  MX_LPTIM1_Init();
//
//  /* Reinitialize ADC */
////  MX_ADC_Init();
////  MX_LPUART1_UART_Init();
//
//  /* PLL Clock Recovery */
//  SystemClock_Config();
//}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
////	static uint32_t debounce = 0;
////	static uint32_t now = 0;
//
//	if(GPIO_Pin == KEY_UP_Pin)
//	{
////		/* anti-chattering */
////		now = HAL_GetTick();
////		if(now - debounce < BOUNCE_CNT) return;
////
////		debounce = HAL_GetTick();
//
//		key_pos++;
//	}
//	else if(GPIO_Pin == KEY_ON_Pin)
//	{
////		/* anti-chattering */
////		now = HAL_GetTick();
////		if(now - debounce < BOUNCE_CNT) return;
////
////		debounce = HAL_GetTick();
//	}
//	else if(GPIO_Pin == KEY_DOWN_Pin)
//	{
////		/* anti-chattering */
////		now = HAL_GetTick();
////		if(now - debounce < BOUNCE_CNT) return;
////
////		debounce = HAL_GetTick();
//
//		key_pos--;
//	}
//
//	if (key_pos < 1) key_pos = 1;
//	else if (key_pos > 12) key_pos = 12;
//
//	flag_stop_mode = 1;
////		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//}

void Pro_operation( void )
{
//	if(flag_stop_mode)
//	{
//		switch (key_pos)
//		{
//		case 1 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 2 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 3 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 4 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 5 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 6 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 7 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 8 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 9 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 10 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 11 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		case 12 :
//			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, Settings.brightness_dot[key_pos-1]);
//			break;
//		default : break;
//		}
//
//		flag_stop_mode = 0;
//
//		StopMode();
//
//	}
}


