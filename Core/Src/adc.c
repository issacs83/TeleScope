/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		ADC
 *
 *   Author peter
 */
#include "stm32l0xx.h"
#include "adc.h"
//#include "timer.h"
#include "error.h"
#include "main.h"

unsigned short ADC_array[2];
unsigned char CurrentChannel = 0;

void InitADC()
{
	// Enable the peripheral clock of the ADC and SYSCFG
//	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SYSCFGEN;

	// PA6 AN Mode
//	GPIOA->MODER &= ~(GPIO_MODER_MODE6);
//	GPIOA->MODER |= (GPIO_MODER_MODE6);

	// PA6 Pull-Up
//	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD6);
//	GPIOA->PUPDR |= GPIO_PUPDR_PUPD6_0;

	// PB1 AN Mode
//	GPIOB->MODER &= ~(GPIO_MODER_MODE1);
//	GPIOB->MODER |= (GPIO_MODER_MODE1);

	// PB1 Pull-Up
//	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1);
//	GPIOB->PUPDR |= GPIO_PUPDR_PUPD1_0;

	// Configure
	/* (1) Select HSI16 by writing 00 in CKMODE (reset value) */
	/* (2) Select the continuous mode and scanning direction */
	/* (3) Select CHSEL4, CHSEL9 and CHSEL17 */
	/* (4) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 5 us */
	/* (5) Enable interrupts on EOC, EOSEQ and overrrun */
	/* (6) Wake-up the VREFINT (only for VLCD, Temp sensor and VRefInt) */

	// ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;								/* (1) */
	ADC1->CFGR1 |= ADC_CFGR1_WAIT | ADC_CFGR1_CONT | ADC_CFGR1_SCANDIR;	/* (2) */
	ADC1->CHSELR = /* ADC_CHSELR_CHSEL6 | */ADC_CHSELR_CHSEL9;			/* (3) */
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;		/* (4) */
	ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE;		/* (5) */
	ADC->CCR |= ADC_CCR_VREFEN;											/* (6) */

	/* Configure NVIC for ADC */
	/* (1) Enable Interrupt on ADC */
	/* (2) Set priority for ADC */
	NVIC_EnableIRQ(ADC1_COMP_IRQn);										/* (1) */
	NVIC_SetPriority(ADC1_COMP_IRQn,0);									/* (2) */

	// Calibrate
	/* (1) Ensure that ADEN = 0 */
	/* (2) Clear ADEN */
	/* (3) Set ADCAL=1 */
	/* (4) Wait until EOCAL=1 */
	/* (5) Clear EOCAL */

	if ((ADC1->CR & ADC_CR_ADEN) != 0) {								/* (1) */
		ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);							/* (2) */
	}

	ADC1->CR |= ADC_CR_ADCAL;											/* (3) */
	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) {							/* (4) */
		/* For robust implementation, add here time-out management */
	}
	ADC1->ISR |= ADC_ISR_EOCAL;											/* (5) */
}

void EnableADC()
{
	/* (1) Enable the ADC */
	/* (2) Wait until ADC ready if AUTOFF is not set */

	ADC1->CR |= ADC_CR_ADEN;											/* (1) */
	if ((ADC1->CFGR1 & ADC_CFGR1_AUTOFF) == 0) {
		while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {						/* (2) */
			/* For robust implementation, add here time-out management */
		}
	}

	ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversions */
}

void DisableADC()
{
	/* (1) Ensure that no conversion on going */
	/* (2) Stop any ongoing conversion */
	/* (3) Wait until ADSTP is reset by hardware i.e. conversion is stopped */
	/* (4) Disable the ADC */
	/* (5) Wait until the ADC is fully disabled */

	if ((ADC1->CR & ADC_CR_ADSTART) != 0) {								/* (1) */
		ADC1->CR |= ADC_CR_ADSTP;										/* (2) */
	}

	while ((ADC1->CR & ADC_CR_ADSTP) != 0) {							/* (3) */
		/* For robust implementation, add here time-out management */
	}

	ADC1->CR |= ADC_CR_ADDIS;											/* (4) */
	while ((ADC1->CR & ADC_CR_ADEN) != 0) {								/* (5) */
		/* For robust implementation, add here time-out management */
	}
}

uint16_t GetADC(int ch)
{
	uint16_t adc[8], temp;
	uint16_t sum = 0;
	uint8_t i = 0, j;

//	while(CurrentChannel != 0);
	for(i = 0; i < 8; i++) {

		LL_ADC_Enable(ADC1);

		if ((LL_ADC_IsEnabled(ADC1) == 1) && (LL_ADC_REG_IsConversionOngoing(ADC1) == 0))
		{
			// Begin a conversion
			LL_ADC_REG_StartConversion(ADC1);

			// Wait until a conversion is done
			while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0);

			// Read the conversion Result (ADC Value)
			adc[i] = LL_ADC_REG_ReadConversionData12(ADC1);

			// Clear End of conversion Flag
			LL_ADC_ClearFlag_EOC(ADC1);
		}

		//		LL_ADC_REG_SetSequencerChRem(ADC1, LL_ADC_CHANNEL_TEMPSENSOR);
		LL_ADC_Disable(ADC1);

//		adc[i] = ADC_array[ch];
		sum += adc[i];
		HAL_Delay(1);
	}

	// sort
	for(i = 0; i < 8; i++) {
		for(j = i + 1; j < 8; j++) {
			if(adc[i] < adc[j]) {
				temp = adc[i];
				adc[i] = adc[j];
				adc[j] = temp;
			}
		}
	}

	sum -= adc[0] + adc[1] + adc[6] + adc[7];
	sum /= 4;

	return(sum);
}

//void ADC1_COMP_IRQHandler(void)
void ADC1_COMP_ISR(void)
{
	if ((ADC1->ISR & (ADC_ISR_EOC | ADC_ISR_EOSEQ | ADC_ISR_OVR)) == 0) {	/* Check if one the expected flag is set */
		error |= ERROR_UNEXPECTED_ADC_IT;									/* Report an error */
	}
	else {
		if ((ADC1->ISR & ADC_ISR_OVR) != 0) {								/* Check OVR has triggered the IT */
			ADC1->ISR |= ADC_ISR_EOC | ADC_ISR_EOSEQ | ADC_ISR_OVR;			/* Clear all pending flags */
			ADC1->CR |= ADC_CR_ADSTP;										/* Stop the sequence conversion */
		}
		else {
			if ((ADC1->ISR & ADC_ISR_EOC) != 0) {							/* Check EOC has triggered the IT */
				ADC_array[CurrentChannel] = ADC1->DR;						/* Read data and clears EOC flag */
//				CurrentChannel++;											/* Increment the index on ADC_array */
			}

			if ((ADC1->ISR & ADC_ISR_EOSEQ) != 0) {							/* Check EOSEQ has triggered the IT */
				ADC1->ISR |= ADC_ISR_EOSEQ; /* Clear the pending bit */
				CurrentChannel = 0; /* Reinitialize the CurrentChannel */
			}
		}
	}
}
