/*
 * operation.h
 *
 *  Created on: Sep 17, 2022
 *      Author: issacs
 */

#ifndef INC_OPERATION_H_
#define INC_OPERATION_H_

#include "stm32l0xx_hal.h"
//#include "stm32l0xx_ll_adc.h"
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

#define LEVEL1	247
#define LEVEL2	242
#define LEVEL3	234
#define LEVEL4	226
#define LEVEL5	222
#define LEVEL6	211
#define LEVEL7	195
#define LEVEL8	187
#define LEVEL9	176
#define LEVEL10	165
#define LEVEL11	155
#define LEVEL12	135

#define PERIOD	616//617-1

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
#define LED_PWM_Pin GPIO_PIN_7
#define LED_PWM_GPIO_Port GPIOA
#define L_SENSOR_Pin GPIO_PIN_1
#define L_SENSOR_GPIO_Port GPIOB
#define CH_SEL1_Pin GPIO_PIN_9
#define CH_SEL1_GPIO_Port GPIOA
#define CH_SEL2_Pin GPIO_PIN_10
#define CH_SEL2_GPIO_Port GPIOA

// Parameters which stored in EEPROM (structure must be aligned to 4 byte)
typedef struct {
	uint32_t  brightness_dot[12];       // Dot led
	uint32_t  brightness_level[12];       // CD sensor
} Settings_TypeDef;

extern Settings_TypeDef Settings;                  // Settings which stored in EEPROM

void ReadSettings_EEPROM(void);
void SaveSettings_EEPROM(void);

//int __attribute__((optimize("O0"))) CalDuty(uint8_t arg)
int	CalDuty(uint8_t arg);
void StopMode(void);
//void SystemClock_Config(void);
void Pro_operation(void);
#endif /* INC_OPERATION_H_ */
