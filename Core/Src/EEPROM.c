/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		EEPROM function
 *
 *   Author peter
 */
#include "stm32l0xx.h"
#include "eeprom.h"
#include "error.h"

#define EEPROM_LEVEL		0x08080000
#define EEPROM_MODE			0x08080001
#define EEPROM_LIGHT		0x08080010
#define EEPROM_CDS			0x08080030

uint8_t LedLevel;
uint8_t LedMode;
uint16_t LedLight[MAX_LEVEL];
uint16_t CDSLight[MAX_LEVEL];

uint8_t UnlockEEPROM()
{
	uint32_t primask_bit;

	if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET) {
		/* Disable interrupts to avoid any interruption during unlock sequence */
		primask_bit = __get_PRIMASK();
		__disable_irq();

		/* Unlocking the Data memory and FLASH_PECR register access*/
		FLASH->PEKEYR = 0x89ABCDEF;		// FLASH_PEKEY1;
		FLASH->PEKEYR = 0x02030405;		// FLASH_PEKEY2;

		/* Re-enable the interrupts: restore previous priority mask */
		__set_PRIMASK(primask_bit);

		if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET)
		{
			return ERROR_FLASH_UNLOCK;
		}
	}

	return 0;
}

void LockEEPROM()
{
	SET_BIT(FLASH->PECR, FLASH_PECR_PELOCK);
}

void ProgramEEPROM(uint8_t type, uint32_t address, uint32_t data)
{
	if(type == EEPROM_PROGRAM_DWORD) {
		/* Program word (32-bit) at a specified address.*/
		*(__IO uint32_t *)address = data;
    }
	else if(type == EEPROM_PROGRAM_WORD) {
		/* Program halfword (16-bit) at a specified address.*/
		*(__IO uint16_t *)address = (uint16_t)data;
	}
	else if(type == EEPROM_PROGRAM_BYTE) {
		/* Program byte (8-bit) at a specified address.*/
		*(__IO uint8_t *)address = (uint8_t)data;
    }
}

uint32_t Read32EEPROM(uint32_t address)
{
	return(*(__IO uint32_t *)address);
}

uint16_t Read16EEPROM(uint32_t address)
{
	return(*(__IO uint16_t *)address);
}

uint8_t ReadEEPROM(uint32_t address)
{
	return(*(__IO uint8_t *)address);
}

void ReadSystemConfig()
{
	uint8_t i;

//	LedLevel = ReadEEPROM(EEPROM_LEVEL);
//	LedMode = ReadEEPROM(EEPROM_MODE);
	for(i = 0; i < MAX_LEVEL; i++) {
		LedLight[i] = Read16EEPROM(EEPROM_LIGHT + i * 2);
		CDSLight[i] = Read16EEPROM(EEPROM_CDS + i * 2);
	}
}

void WriteSystemConfig(uint8_t mode)
{
	uint8_t i;

	UnlockEEPROM();
//	if(mode == LED_LEVEL) ProgramEEPROM(EEPROM_PROGRAM_BYTE, EEPROM_LEVEL, (uint32_t)LedLevel);
//	else if(mode == LED_MODE) ProgramEEPROM(EEPROM_PROGRAM_BYTE, EEPROM_MODE, (uint32_t)LedMode);
//	else
		if(mode == LED_LIGHT) for(i = 0; i < MAX_LEVEL; i++) ProgramEEPROM(EEPROM_PROGRAM_WORD, EEPROM_LIGHT + i * 2, (uint32_t)LedLight[i]);
	else if(mode == CDS_LIGHT) for(i = 0; i < MAX_LEVEL; i++) ProgramEEPROM(EEPROM_PROGRAM_WORD, EEPROM_CDS + i * 2, (uint32_t)CDSLight[i]);
	else if(mode == LED_ALL) {
//		ProgramEEPROM(EEPROM_PROGRAM_BYTE, EEPROM_LEVEL, (uint32_t)LedLevel);
//		ProgramEEPROM(EEPROM_PROGRAM_BYTE, EEPROM_MODE, (uint32_t)LedMode);
		for(i = 0; i < MAX_LEVEL; i++) {
			ProgramEEPROM(EEPROM_PROGRAM_WORD, EEPROM_LIGHT + i * 2, (uint32_t)LedLight[i]);
			ProgramEEPROM(EEPROM_PROGRAM_WORD, EEPROM_CDS + i * 2, (uint32_t)CDSLight[i]);
		}
	}
	LockEEPROM();
}
