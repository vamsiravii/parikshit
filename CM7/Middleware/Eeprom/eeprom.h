/**
  ******************************************************************************
  * @file    EEPROM_Emulation/inc/eeprom.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for the EEPROM
  *          emulation firmware library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm32h7xx_hal.h"
#include "main.h"

/* EEPROM status */
#define EE_OK      HAL_OK
#define EE_ERROR   HAL_ERROR
#define EE_BUSY    HAL_BUSY
#define EE_TIMEOUT HAL_TIMEOUT


/* Flash sector size (STM32H7 = 128 KB) */
#define PAGE_SIZE  ((uint32_t)0x20000)


/* Voltage range */
#define VOLTAGE_RANGE  VOLTAGE_RANGE_3


/* Flash sector base addresses for STM32H755 (Bank 2, 128KB sectors) */
/* Bank 2 starts at 0x08100000, each sector is 0x20000 (128KB)        */
#ifndef ADDR_FLASH_SECTOR_6_BANK2
#define ADDR_FLASH_SECTOR_6_BANK2  ((uint32_t)0x081C0000)
#endif

/* EEPROM start location (Bank2 Sector6) */
#define EEPROM_START_ADDRESS  ADDR_FLASH_SECTOR_6_BANK2


/* Page addresses */
#define PAGE0_BASE_ADDRESS  ((uint32_t)(EEPROM_START_ADDRESS))
#define PAGE0_END_ADDRESS   ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE - 1))

#define PAGE1_BASE_ADDRESS  ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS   ((uint32_t)(EEPROM_START_ADDRESS + 2*PAGE_SIZE - 1))


/* Flash sectors used */
#define PAGE0_ID  FLASH_SECTOR_6
#define PAGE1_ID  FLASH_SECTOR_7


/* Page numbers */
#define PAGE0  ((uint16_t)0x0000)
#define PAGE1  ((uint16_t)0x0001)

#define NO_VALID_PAGE ((uint16_t)0x00AB)


/* Page states */
#define ERASED        ((uint16_t)0xFFFF)
#define RECEIVE_DATA  ((uint16_t)0xEEEE)
#define VALID_PAGE    ((uint16_t)0x0000)


#define READ_FROM_VALID_PAGE  ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE   ((uint8_t)0x01)

#define PAGE_FULL ((uint8_t)0x80)


/*
 * EEPROM capacity — virtual address table sizing
 *
 * The driver stores two ping-pong copies of EEPROM_Block (216 bytes each).
 * Each copy is read/written as 16-bit virtual variables (two per 32-bit word):
 *
 *   EEPROM_BLOCK_SIZE  = 216 bytes  =>  108 16-bit variables per copy
 *   Two copies         =>  216 virtual addresses total  (1 .. 216)
 *
 * NB_OF_VAR MUST match the number of entries in eeprom_vars_list.h.
 */
#define EEPROM_TOTAL_BYTES  200          /* user payload size */
#define NB_OF_VAR           216          /* Fixed: was 100, must be 216 */


/* API */

uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t *Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif /* __EEPROM_H */
