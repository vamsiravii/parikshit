#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include <stdint.h>

#define EEPROM_MAGIC        0xDEADBEEF
#define EEPROM_VERSION      1
#define EEPROM_DATA_SIZE    200

typedef enum
{
    EEPROM_OK = 0,
    EEPROM_ERROR
} EEPROM_Status;

EEPROM_Status EEPROM_Init(void);

EEPROM_Status EEPROM_Load(uint8_t *data);

EEPROM_Status EEPROM_Save(const uint8_t *data);

#endif
