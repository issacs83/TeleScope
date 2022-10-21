/*
 *	SPACETECH Company LTD.
 *		Copyright 2022 all right reserved.
 *
 *		EEPROM define
 *
 *   Author peter
 */
#pragma once
#include <stdint.h>
//#include "board.h"

#define MAX_LEVEL	12
#define LED_LEVEL	0
#define LED_MODE	1
#define LED_LIGHT	2
#define CDS_LIGHT	3
#define LED_ALL		4

#define EEPROM_PROGRAM_DWORD	0
#define EEPROM_PROGRAM_WORD		1
#define EEPROM_PROGRAM_BYTE		2

// external variables
extern uint8_t LedLevel;
extern uint8_t LedMode;
extern uint16_t LedLight[12];
extern uint16_t CDSLight[12];

uint8_t UnlockEEPROM(void);
void LockEEPROM(void);
void ProgramEEPROM(uint8_t type, uint32_t address, uint32_t data);
uint32_t Read32EEPROM(uint32_t address);
uint16_t Read16EEPROM(uint32_t address);
uint8_t ReadEEPROM(uint32_t address);
void ReadSystemConfig(void);
void WriteSystemConfig(uint8_t mode);
