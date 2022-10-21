/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		Error Define
 *
 *   Author peter
 */
#pragma once
#include <stdint.h>

extern volatile uint16_t error;

/* Error codes used to make the orange led blinking */
#define ERROR_USART 			0x01
#define ERROR_HSI_TIMEOUT 		0x02
#define ERROR_PLL_TIMEOUT 		0x03
#define ERROR_CLKSWITCH_TIMEOUT	0x04

#define ERROR_UNEXPECTED_ADC_IT	0x10

#define ERROR_FLASH_UNLOCK		0x20
