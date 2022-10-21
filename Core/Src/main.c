/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
#include "EEPROM.h"
#include "time.h"
#include "usart.h"
#include "errno.h"
#include "adc.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PERIOD 616
#define TWO_BUTTON

#define KEY_FUNC_DELAY	200
#define KEY_ADC_DELAY	KEY_FUNC_DELAY
//#define CalDuty( x )	( (PERIOD+1) - ( ( (PERIOD+1) * x ) * 0.01f) )
#define Default_LED	8
//#define LEVEL1	247
//#define LEVEL2	242
//#define LEVEL3	234
//#define LEVEL4	226
//#define LEVEL5	222
//#define LEVEL6	211
//#define LEVEL7	195
//#define LEVEL8	187
//#define LEVEL9	176
//#define LEVEL10	165
//#define LEVEL11	155
//#define LEVEL12	135
//
//#define PERIOD	616//617-1
//#define BOUNCE_CNT	10
//#define MAX_LEVEL			12
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

LPTIM_HandleTypeDef hlptim1;

/* USER CODE BEGIN PV */
uint16_t timeout;
volatile uint32_t last_time;
//volatile uint16_t error;

char CommandBuffer[64];
char IntBuffer[8];
char ByteBuffer[16];
//uint8_t flag_uart_on=0;


uint8_t flag_3_repeat = 0;
bool flag_led_on_off = 0;
bool flag_stop_mode = 1;
bool flag_change_mode = 0;
__IO uint8_t flag_led_mode = 1;
bool flag_power_on = 1;
bool flag_low_batt = 0;

bool LuxOut = 0;

uint8_t  key_pos = Default_LED;
uint8_t  temp_key_pos = 0;
bool  key_done = 1;
__IO uint16_t key_pressed = 0;
__IO uint16_t key_adc_pressed = 0;
__IO uint16_t count_1s = 0;
__IO uint16_t count_500ms = 0;

//uint8_t LuxMode = 0;
//uint8_t OnLux = 0;
bool SetLuxMode = 0;

//const char *ModelName = "CircleDot Rev 0.00";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPTIM1_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */
//unsigned char d=0; //ring buffer
//uint8_t lpuart1Idle = 0; //idle flag
//uint8_t txData; // byte to send
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int ParsingData(char *buffer, char sep, int pos, char *result)
{
	int i, c;
	int n = 0;
	char found = 0;

	c = 0;
	for(i = 0; i < strlen(buffer); i++) {
		if(buffer[i] == sep) {
			if(pos == c++) {
				ByteBuffer[n] = 0;
				found = 1;
				break;
			}
			n = 0;
		}
		else ByteBuffer[n++] = buffer[i];
	}
	if(pos == c) found = 1;
	ByteBuffer[n] = 0;

	if(found) memcpy(result, ByteBuffer, n + 1);
	else {
		result[0] = 0;
		n = 0;
	}
	return(n);
}

int StringToInt(char *buffer)
{
	int ret = 0;
	int i, len, rt;

	for(i = 0; i < strlen(buffer); i++) {
		if(buffer[i] < '0' || buffer[i] > '9') break;
	}
	len = i;

	rt = 1;
	for(i = 0; i < len; i++) {
		ret += rt * (buffer[(len - 1) - i] - '0');
		rt *= 10;
	}
	return(ret);
}

char *IntToString(int data)
{
	int i, len, rt;

	memset(ByteBuffer, 0, sizeof(ByteBuffer));
	len = 0;
	rt = data;
	while(1) {
		len++;
		rt /= 10;
		if(rt <= 0) break;
	}

	rt = data;
	for(i = len - 1; i >= 0; i--) {
		rt = data % 10;
		ByteBuffer[i] = rt + '0';
		data /= 10;
	}

	return(ByteBuffer);
}

void Protocol(char *cmd)
{
	uint8_t i;

	ParsingData(cmd, ',', 0, CommandBuffer);
//	if(strcmp(CommandBuffer, "*IDN?") == 0) {
//		PrintUART(ModelName);
////		HAL_Delay(10);
//		PrintUART("\r\n");
//	}
//	else
		if(strcmp(CommandBuffer, ":READLEVEL") == 0) {
		PrintUART(":LEVEL");
		for(i = 0; i < MAX_LEVEL; i++) {
			IntToString(LedLight[i]);
			PrintUART(",");
			PrintUART(ByteBuffer);
			HAL_Delay(10);
		}

		PrintUART("\r\n");
	}
	else if(strcmp(CommandBuffer, ":READLUX") == 0) {
		PrintUART(":LUX");
		for(i = 0; i < MAX_LEVEL; i++) {
			IntToString(CDSLight[i]);
			PrintUART(",");
			PrintUART(ByteBuffer);
			HAL_Delay(10);
		}

		PrintUART("\r\n");
	}
	else if(strcmp(CommandBuffer, ":WRITELEVEL") == 0) {
		for(i = 0; i < MAX_LEVEL; i++) {
			ParsingData(cmd, ',', i + 1, CommandBuffer);
			LedLight[i] = StringToInt(CommandBuffer);
			//duty ratio transformation
		}

		WriteSystemConfig(LED_LIGHT);
		//SaveBuffer_EEPROM(DATA_EEPROM_START_ADDR + 0x40, (uint32_t *)&Settings, sizeof(Settings));
		HAL_Delay(100);
		PrintUART(":ACK,LEVEL\r\n");
		HAL_Delay(100);
	}
	else if(strcmp(CommandBuffer, ":WRITELUX") == 0) {
		for(i = 0; i < MAX_LEVEL; i++) {
			ParsingData(cmd, ',', i + 1, CommandBuffer);
			CDSLight[i] = StringToInt(CommandBuffer);
		}

		WriteSystemConfig(CDS_LIGHT);
		HAL_Delay(100);
		PrintUART(":ACK,LUX\r\n");
		HAL_Delay(100);
	}
	else if(strcmp(CommandBuffer, ":LUXON") == 0) {
//		SetLuxMode = 1;
		LuxOut = 1;
	}
	else if(strcmp(CommandBuffer, ":LUXOFF") == 0) {
//		SetLuxMode = 0;
		LuxOut = 0;
	}
}

void SleepMode(void)
{
	/* Set LED1 */
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);

	/* Suspend SysTick */
	HAL_SuspendTick();

	/* Enable Power Peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Sleep Mode */
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);//PWR_MAINREGULATOR_ON

	/* Resume SysTick When System Wake-up */
	HAL_ResumeTick();

	/* PLL Clock Recovery */
	SystemClock_Config();
}

void StopMode(void)
{
	/* Suspend Tick increment to prevent wakeup by Systick interrupt.
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
	HAL_SuspendTick();

	/* We enable again the PWR peripheral */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* Request to enter SLEEP mode */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);

	//  SystemClock_Config_low();

	/* Resume Tick interrupt if disabled prior to sleep mode entry*/
	HAL_ResumeTick();

	/* PLL Clock Recovery */
	SystemClock_Config();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	key_done = 0;
	if(GPIO_Pin == KEY_UP_Pin)
	{
		//POWER ON
		if( (!flag_power_on) && (!flag_stop_mode) )
		{
			key_pos = Default_LED;
//			flag_stop_mode = 1;
		}
		//Brightness up
		else
		{
			key_pos++;
//			flag_stop_mode = 0;
		}
	}
#ifndef TWO_BUTTON
	else if(GPIO_Pin == KEY_ON_Pin)
	{
		//POWER ON
		if( (!flag_power_on) && (!flag_stop_mode) )
		{
			key_pos = Default_LED;
//			flag_stop_mode = 1;
		}
		else
		{
			flag_change_mode = 1;
		}
//		{
//			//MODE change dot, circle, cross
//			flag_led_mode++;
//			if(flag_led_mode > 3) flag_led_mode = 1;
//
//			flag_stop_mode = 0;
//		}
	}
#endif
	else if(GPIO_Pin == KEY_DOWN_Pin)
	{
		//POWER ON
		if( (!flag_power_on) && (!flag_stop_mode) )
		{
			key_pos = Default_LED;
//			flag_stop_mode = 1;
		}
		//Brightness down
		else
		{
			key_pos--;
//			flag_stop_mode = 0;
		}
	}
//	flag_stop_mode = 1;
	//POWER off
	if(!flag_power_on)
	{
		if(!HAL_GPIO_ReadPin(BATT_DET_GPIO_Port, BATT_DET_Pin))
		{
			flag_low_batt = 1;
			flag_stop_mode = 0;
		}
	}
	//	else if(GPIO_Pin == BATT_DET_Pin)
	//	{
	//		flag_low_batt = 1;
	//	}

	if (key_pos < 1) key_pos = 1;
	else if( key_pos == 255) key_pos = 1;
	else if (key_pos > 12) key_pos = 12;

	flag_power_on = 1;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int i = 0;
	uint8_t level;
	uint16_t cds = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  MX_LPTIM1_Init();
	/* Init Timer */
  time_setup();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  ReadSystemConfig();

//  TEST CODE
//  ReadBuffer_EEPROM(DATA_EEPROM_START_ADDR + 0x40, (uint32_t *)&Settings.brightness_dot, sizeof(Settings.brightness_dot));
  //  LedLight[0] = 308;
  //  LedLight[1] = 290;
  //  LedLight[2] = 271;
  //  LedLight[3] = 253;
  //  LedLight[4] = 241;
  //  LedLight[5] = 216;
  //  LedLight[6] = 197;
  //  LedLight[7] = 148;
  //  LedLight[8] = 136;
  //  LedLight[9] = 123;
  //  LedLight[10] = 111;
  //  LedLight[11] = 99;
//  WriteSystemConfig(LED_LIGHT);

  //EEPROM?óê ?ç∞?ù¥?Ñ∞Í∞? ?óÜ?úºÎ©? ?ûÑ?ùò?ùò Í∞íÏúºÎ°? Ï≤òÍ∏∞?ôî ?ïú?ã§
  for(i = 0; i < 12; i++)
  {
	  if( ( LedLight[i] == 0x0 ) || ( LedLight[i] == 0xFFFFFFFF ) )
	  {
		  LedLight[i] = 123;
	  }

	  if( ( CDSLight[i] == 0x0 ) || ( CDSLight[i] == 0xFFFFFFFF ) )
	  {
		  CDSLight[i] = 800;
	  }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		// Ï°∞ÎèÑÎ™®Îìú ?ùº ?ïå ?Üµ?ã†Ï≤òÎ¶¨Í∞? Í∞??ä•?ï®, ?àò?ã†Î∞õÏ? ?îÑÎ°úÌÜ†ÏΩúÏùÑ Ï≤òÎ¶¨
		if(isReadyByte() && SetLuxMode)
		{
			Protocol((char *)GetUART());
		}

		// key up / down ?Ç§Î°? Ï°∞ÎèÑ Î™®ÎìúÎ°? ÏßÑÏûÖÍ∞??ä•, Ï°∞ÎèÑ?óê ?î∞?ùº Î∞ùÍ∏∞ Î≥??ôî
		if(SetLuxMode)
		{
			level = MAX_LEVEL;
			cds = GetADC(0);
			if(CDSLight[0] >= cds) level = 0;
			else {
				for(i = 1; i < MAX_LEVEL; i++) {
					if(CDSLight[i - 1] < cds && CDSLight[i] >= cds) {
						level = i;
						break;
					}
				}
			}

			if(level == MAX_LEVEL) level = MAX_LEVEL - 1;

			LedLevel = level;
//			WriteSystemConfig(LED_LEVEL);

			HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, LedLight[LedLevel]);
			//
			switch(flag_led_mode)
			{
			case 1 :
				HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
				HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, RESET);
				break;
			case 2 :
				HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
				HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
				break;
			case 3 :
				HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, SET);
				HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
				break;
			default : break;
			}

			flag_stop_mode = 0;
		}

		// ???†Ñ?†• Î™®Îìú
		if(flag_power_on)
		{
			if(flag_stop_mode)
			{
				HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, LedLight[key_pos-1]);

				flag_stop_mode = 0;

				StopMode();
			}
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		/* Every 10ms */
		if ( (time_ms() - last_time) > 10-1 ) //10mS
		{
			//Ï°∞ÎèÑ Î™®Îìú?ùº ?ïå 250mS ÎßàÎã§ Ï°∞ÎèÑ Í∞íÏùÑ ?†Ñ?Ü°
			if(count_500ms > 25) //100mS
			{
				if(LuxOut) {
					PrintUART(":CDS,");
					IntToString(cds);
					PrintUART(ByteBuffer);
					PrintUART("\r\n");
					//				LuxOut = 0;
				}
				count_500ms = 0;
			}

			//Î∞∞ÌÑ∞Î¶¨Í? ?óÜ?ùÑ ?ïå 1Ï¥? Ï£ºÍ∏∞Î°? LEDÎ•? 3Î≤? ?†êÎ©∏Ìïú?ã§
			if( (flag_power_on) && (flag_low_batt) && (count_1s > 100-1) )
			{
				if(flag_3_repeat > 5)
				{
					flag_3_repeat = 0;
					flag_led_on_off = 0;

					flag_low_batt = 0;
					count_1s = 0;

					goto sub_led_proc;
				}
				else
				{

					flag_3_repeat++;
					flag_led_on_off ^= 0x01;

					if(flag_led_on_off)
						HAL_LPTIM_PWM_Start(&hlptim1, PERIOD, LedLight[key_pos-1]);
					else
						HAL_LPTIM_PWM_Stop(&hlptim1);

					HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, flag_led_on_off);
					HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, flag_led_on_off);
				}

				count_1s = 0;
			}
			else
			{
				// +/-?Ç§Í∞? ?èô?ãú?óê ?Ç§Í∞? ?àå?ü¨Ï°åÎäîÏß? ?ôï?ù∏
				if( (flag_power_on) && (!flag_low_batt) && ( !HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) && !HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) ) && !key_done )
				{
					key_adc_pressed++;
				}
				// Any?Ç§Í∞? ?àå?ü¨Ï°åÎäîÏß? ?ôï?ù∏
				else if( (flag_power_on) && (!flag_low_batt) && !( HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) && HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) && HAL_GPIO_ReadPin(KEY_ON_GPIO_Port, KEY_ON_Pin) ) && !key_done )
				{
					key_pressed++;
				}
#ifdef TWO_BUTTON
				// +/- ?Ç§Í∞? ?èô?ãú?óê ÏßßÍ≤å ?àå?ü¨Ï°åÏúºÎ©??
				else if((key_adc_pressed > 10) && (key_adc_pressed < 100))
				{
					//MODE change dot, circle, cross
					if(flag_change_mode)
					{
						flag_led_mode++;
						if(flag_led_mode > 3) flag_led_mode = 1;
					}

					//POWER ON
					if( (flag_power_on) && (!flag_stop_mode) && (!flag_change_mode) )
					{
						key_pos = Default_LED;
						flag_change_mode = 1;
						flag_stop_mode = 1;
					}

					if(flag_change_mode)
					{
						//					flag_change_mode = 0;
						sub_led_proc:
						switch(flag_led_mode)
						{
						case 1 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, RESET);
							break;
						case 2 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
							break;
						case 3 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, SET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
							break;
						default : break;
						}
					}
					key_adc_pressed =0;
				}
#endif
				// Any?Ç§Í∞? ÏßßÍ≤å ?àå?ü¨Ï°åÏúºÎ©?
				else if((key_pressed > 10) && (key_pressed < 100))
				{

#ifndef TWO_BUTTON
					sub_led_proc:

					if(flag_change_mode)
					{
						//MODE change dot, circle, cross
						flag_led_mode++;
						if(flag_led_mode > 3) flag_led_mode = 1;
						flag_change_mode = 0;

						switch(flag_led_mode)
						{
						case 1 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, RESET);
							break;
						case 2 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
							break;
						case 3 :
							HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, SET);
							HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, SET);
							break;
						default : break;
						}
					}
#endif

					if(SetLuxMode)	flag_stop_mode = 0;
					else			flag_stop_mode = 1;

					key_pressed = 0;

				}

				if( key_adc_pressed < 0 ) key_adc_pressed = 0;
				else if( key_adc_pressed > (KEY_ADC_DELAY-1) )
				{
					//flag_power_on = 0;
					key_done = 1;
					key_adc_pressed = 0;

					SetLuxMode ^= 0x1;

//					SleepMode();

					if(SetLuxMode)
					{
						HAL_GPIO_WritePin(CDS_En_GPIO_Port, CDS_En_Pin, SET);
						temp_key_pos = key_pos;
//						PrintUART(ModelName);
					}
					else
					{
						//ReadSystemConfig();
						HAL_GPIO_WritePin(CDS_En_GPIO_Port, CDS_En_Pin, RESET);
						key_pos = temp_key_pos;
						flag_stop_mode = 1;
					}
					//key_pos = 0;

					last_time = time_ms();
				}

				if( key_pressed < 0 ) key_pressed = 0;
				else if( key_pressed > (KEY_FUNC_DELAY-1) )
				{
					key_done = 1;
					HAL_LPTIM_PWM_Stop(&hlptim1);
//					HAL_GPIO_WritePin(CH_SEL1_GPIO_Port, CH_SEL1_Pin, RESET);
//					HAL_GPIO_WritePin(CH_SEL2_GPIO_Port, CH_SEL2_Pin, RESET);
					flag_change_mode = 0;
					flag_power_on = 0;
					key_pressed = 0;
					key_pos = 0;
					last_time = time_ms();

					StopMode();
				}
			}

			if(flag_low_batt)	count_1s++;
			if(SetLuxMode)	count_500ms++;

			last_time = time_ms();
		}
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }
  LL_SetSystemCoreClock(16000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_HSI);
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_InitTypeDef ADC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /**ADC GPIO Configuration
  PB1   ------> ADC_IN9
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* ADC interrupt Init */
  NVIC_SetPriority(ADC1_COMP_IRQn, 0);
  NVIC_EnableIRQ(ADC1_COMP_IRQn);

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure Regular Channel
  */
  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_9);

  /** Common config
  */
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_1RANK;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_160CYCLES_5);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
  LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
  LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
  LL_ADC_DisableIT_EOC(ADC1);
  LL_ADC_DisableIT_EOS(ADC1);
  ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);

  /* Enable ADC internal voltage regulator */
  LL_ADC_EnableInternalRegulator(ADC1);
  /* Delay for ADC internal voltage regulator stabilization. */
  /* Compute number of CPU cycles to wait for, from delay in us. */
  /* Note: Variable divided by 2 to compensate partially */
  /* CPU processing cycles (depends on compilation optimization). */
  /* Note: If system core clock frequency is below 200kHz, wait time */
  /* is only a few CPU processing cycles. */
  uint32_t wait_loop_index;
  wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  /* USER CODE BEGIN ADC_Init 2 */
  LL_ADC_Enable(ADC1);
  LL_ADC_StartCalibration(ADC1);            //
  while(LL_ADC_IsCalibrationOnGoing(ADC1)); //
  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief LPTIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPTIM1_Init(void)
{

  /* USER CODE BEGIN LPTIM1_Init 0 */

  /* USER CODE END LPTIM1_Init 0 */

  /* USER CODE BEGIN LPTIM1_Init 1 */

  /* USER CODE END LPTIM1_Init 1 */
  hlptim1.Instance = LPTIM1;
  hlptim1.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
  hlptim1.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
  hlptim1.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
  hlptim1.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
  hlptim1.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
  hlptim1.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
  if (HAL_LPTIM_Init(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPTIM1_Init 2 */

  /* USER CODE END LPTIM1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPUART1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /**LPUART1 GPIO Configuration
  PB6   ------> LPUART1_TX
  PB7   ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* LPUART1 interrupt Init */
  NVIC_SetPriority(LPUART1_IRQn, 2);
  NVIC_EnableIRQ(LPUART1_IRQn);

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  LPUART_InitStruct.BaudRate = 9600;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  /* USER CODE BEGIN LPUART1_Init 2 */
	LL_LPUART_Enable(LPUART1);
	LL_LPUART_EnableIT_RXNE(LPUART1);
//	LL_LPUART_EnableIT_TXE(LPUART1);
//	LL_LPUART_ClearFlag_TC(LPUART1);
//	LL_LPUART_EnableIT_TC(LPUART1);
//	LL_LPUART_EnableIT_IDLE(LPUART1);
	LL_LPUART_EnableDirectionTx(LPUART1);
	LL_LPUART_EnableDirectionRx(LPUART1);
	/* Polling LPUART1 initialisation */
	while((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))))
	{
	}
  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CDS_En_Pin|CH_SEL1_Pin|CH_SEL2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CDS_En_Pin CH_SEL1_Pin CH_SEL2_Pin */
  GPIO_InitStruct.Pin = CDS_En_Pin|CH_SEL1_Pin|CH_SEL2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_UP_Pin KEY_ON_Pin KEY_DOWN_Pin BATT_DET_Pin */
  GPIO_InitStruct.Pin = KEY_UP_Pin|KEY_ON_Pin|KEY_DOWN_Pin|BATT_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
