/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		Board define
 *
 *   Author peter
 */
#pragma once
#include <stdint.h>

extern volatile uint8_t TxOn;

void InitUART(int baud);
void PrintUART(const char *buffer);
uint8_t isReadyByte(void);
uint8_t *GetUART(void);
uint8_t GetReadBytes(void);
void LPUART1_ISR(void);
//void LPUART1_IRQHandler(void);
void test(void);
