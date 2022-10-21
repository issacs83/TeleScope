/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		Board define
 *
 *   Author peter
 */
#include <string.h>
#include "stm32l0xx.h"
//#include "error.h"
#include "main.h"

#define UART_CR		13
#define UART_LF		10

#define ENTER_CR	1

#define MAX_READ	64

uint8_t RxBuffer[MAX_READ];
volatile uint8_t ReadyRead = 0;

static uint8_t *TxBuffer;
static uint8_t TxBytes;
volatile uint8_t TxOn = 0;

static uint8_t RxReadBytes;
static uint8_t RxBytes = 0;
static uint8_t RxCRLF = 0;

//void InitUART(int baud)
//{
//	// PB6, PB7 AF Mode
////	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7)) | (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
////	GPIOB->AFR[1] = (GPIOB->AFR[1] &~ (0x00000FFF)) | (4 << (0 * 4)) | (4 << (1 * 4)) | (4 << (2 * 4));
////
////	RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN;
////	LPUART1->BRR = SystemCoreClock / baud;
////	LPUART1->CR3 = 0;
////	LPUART1->CR2 = 0;	// USART_CR2_MSBFIRST | USART_CR2_CLKEN | USART_CR2_LBCL;
//	LPUART1->CR1 = USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;
//
//	/* polling idle frame Transmission w/o clock */
////	while((LPUART1->ISR & USART_ISR_TC) != USART_ISR_TC) ;		/* add time out here for a robust application */
////	LPUART1->ICR = USART_ICR_TCCF;		/* clear TC flag */
////	LPUART1->CR1 |= USART_CR1_TCIE;		/* enable TC interrupt */
//
//	NVIC_SetPriority(LPUART1_IRQn, 0);
//	NVIC_EnableIRQ(LPUART1_IRQn);
//}

void PrintUART(const char *buffer)
{
	while(TxOn);
	TxBytes = 0;
	TxBuffer = (uint8_t *)buffer;
	TxOn = 1;

	while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
	LL_LPUART_TransmitData8(LPUART1, TxBuffer[TxBytes++]);
	LL_LPUART_EnableIT_TC(LPUART1);
}

uint8_t isReadyByte()
{
	return(ReadyRead);
}

uint8_t GetReadBytes()
{
	return(RxReadBytes);
}

uint8_t *GetUART()
{
	ReadyRead = 0;
	return(RxBuffer);
}

/**
  * Brief   This function handles LPUART1 interrupt request.
  * Param   None
  * Retval  None
  */
void LPUART1_ISR(void)
{
	uint8_t d = 0;

	if((LPUART1->ISR & USART_ISR_TC) == USART_ISR_TC)
	{
//		NVIC_DisableIRQ(LPUART1_IRQn);/* Disable USART1_IRQn */
		LL_LPUART_DisableIT_RXNE(LPUART1);

		if(TxBytes == strlen((char *)TxBuffer)) {
			LPUART1->ICR = USART_ICR_TCCF;/* clear transfer complete flag */
			TxOn = 0;
		}
		else
		{
			while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
			LL_LPUART_TransmitData8(LPUART1, TxBuffer[TxBytes++]);

			/* Wait for TC flag to be raised for last char */
			while (!LL_LPUART_IsActiveFlag_TC(LPUART1));
		}
		LL_LPUART_EnableIT_RXNE(LPUART1);
	}
	else if((LPUART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE) {

		d = LPUART1->RDR;/* receive data, clear flag */
		/* Clear OVERRUN flag */
		LL_LPUART_ClearFlag_ORE(LPUART1);

		/* Make sure that no LPUART transfer is on-going */
		while(LL_LPUART_IsActiveFlag_BUSY(LPUART1) == 1)
		{
		}
		/* Make sure that LPUART is ready to receive */
		while(LL_LPUART_IsActiveFlag_REACK(LPUART1) == 0)
		{
		}
		if(RxBytes >= MAX_READ) RxBytes = 0;
		if(RxCRLF == 0 && d == UART_CR) {
			RxCRLF = ENTER_CR;
		}
		else if(RxCRLF == ENTER_CR && d == UART_LF) {
			RxBuffer[RxBytes++] = 0;
			RxReadBytes = RxBytes;
			ReadyRead = 1;
			RxCRLF = 0;
			RxBytes = 0;
		}
		else RxBuffer[RxBytes++] = d;
	}
	else {
//		error = ERROR_USART; /* Report an error */
		NVIC_DisableIRQ(LPUART1_IRQn);/* Disable USART1_IRQn */
	}
}
