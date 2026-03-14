#include "eeprom_driver.h"
#include "../../Middleware/Eeprom/eeprom.h"

#include <string.h>

typedef struct __attribute__((packed))
{
    uint32_t magic;
    uint16_t version;
    uint16_t length;

    uint32_t counter;

    uint8_t data[EEPROM_DATA_SIZE];

    uint32_t crc;

} EEPROM_Block;


/* ----- Derived constants ----- */

#define EEPROM_BLOCK_SIZE   ((sizeof(EEPROM_Block)+3)&~3)
#define EEPROM_BLOCK_WORDS  (EEPROM_BLOCK_SIZE/4)

/*
 * Each copy occupies EEPROM_BLOCK_WORDS 32-bit words = EEPROM_BLOCK_WORDS*2
 * 16-bit virtual addresses.
 *
 * sizeof(EEPROM_Block) = 4+2+2+4+200+4 = 216 bytes  =>  EEPROM_BLOCK_SIZE = 216
 * EEPROM_BLOCK_WORDS   = 54
 *
 * Copy A virtual addresses: 1  .. 108   (byte offset 0   .. 215)
 * Copy B virtual addresses: 109 .. 216  (byte offset 216 .. 431)
 *
 * NB_OF_VAR in eeprom.h must therefore be 216.
 */

#define COPY_A_BASE         0                        /* byte offset of copy A */
#define COPY_B_BASE         (EEPROM_BLOCK_SIZE)      /* byte offset of copy B */



static uint32_t EEPROM_CalcCRC(uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;

    for(uint32_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for(uint8_t j = 0; j < 8; j++)
        {
            if(crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }

    return crc;
}


/*
 * EEPROM_ReadRaw
 *
 * Reads 'length' bytes from virtual-address space starting at byte offset
 * 'addr'.  Uses the 16-bit EE_ReadVariable API: every 4-byte (32-bit) word
 * is read as two consecutive 16-bit virtual variables.
 *
 * Virtual address mapping (1-based, matching VirtAddVarTab):
 *   vaddr = addr / 2 + 1   for the low  16-bit half of each 32-bit word
 *   vaddr = addr / 2 + 2   for the high 16-bit half
 */
static EEPROM_Status EEPROM_ReadRaw(uint16_t addr,
                                     uint8_t  *data,
                                     uint16_t  length)
{
    for(uint16_t i = 0; i < length; i += 4)
    {
        uint16_t lo = 0;
        uint16_t hi = 0;
        uint16_t vaddr = (uint16_t)((addr + i) / 2 + 1);  /* 1-based */

        if(EE_ReadVariable(vaddr,     &lo) != EE_OK)  return EEPROM_ERROR;
        if(EE_ReadVariable(vaddr + 1, &hi) != EE_OK)  return EEPROM_ERROR;

        data[i + 0] =  (uint8_t)(lo & 0xFF);
        data[i + 1] =  (uint8_t)(lo >> 8);
        data[i + 2] =  (uint8_t)(hi & 0xFF);
        data[i + 3] =  (uint8_t)(hi >> 8);
    }

    return EEPROM_OK;
}


/*
 * EEPROM_WriteRaw
 *
 * Writes 'length' bytes to virtual-address space starting at byte offset
 * 'addr'.  Mirrors the layout described in EEPROM_ReadRaw.
 *
 * IRQs are disabled for the whole sequence so that a power-loss mid-write
 * leaves the CRC invalid and the other copy untouched.
 */
static EEPROM_Status EEPROM_WriteRaw(uint16_t       addr,
                                      const uint8_t *data,
                                      uint16_t       length)
{
    __disable_irq();

    for(uint16_t i = 0; i < length; i += 4)
    {
        uint16_t lo    = ((uint16_t)data[i + 1] << 8) | data[i + 0];
        uint16_t hi    = ((uint16_t)data[i + 3] << 8) | data[i + 2];
        uint16_t vaddr = (uint16_t)((addr + i) / 2 + 1);  /* 1-based */

        if(EE_WriteVariable(vaddr,     lo) != EE_OK)
        {
            __enable_irq();
            return EEPROM_ERROR;
        }

        if(EE_WriteVariable(vaddr + 1, hi) != EE_OK)
        {
            __enable_irq();
            return EEPROM_ERROR;
        }
    }

    __enable_irq();

    return EEPROM_OK;
}



static int EEPROM_BlockValid(EEPROM_Block *block)
{
    if(block->magic   != EEPROM_MAGIC)    return 0;
    if(block->version != EEPROM_VERSION)  return 0;
    if(block->length  != EEPROM_DATA_SIZE) return 0;

    uint32_t crc = EEPROM_CalcCRC((uint8_t *)block, EEPROM_BLOCK_SIZE - 4);

    if(crc != block->crc)  return 0;

    return 1;
}



EEPROM_Status EEPROM_Init(void)
{
    if(EE_Init() != EE_OK)
        return EEPROM_ERROR;

    return EEPROM_OK;
}



EEPROM_Status EEPROM_Load(uint8_t *data)
{
    EEPROM_Block blockA;
    EEPROM_Block blockB;

    int validA = 0;
    int validB = 0;

    if(EEPROM_ReadRaw(COPY_A_BASE,
                      (uint8_t *)&blockA,
                      EEPROM_BLOCK_SIZE) == EEPROM_OK)
    {
        validA = EEPROM_BlockValid(&blockA);
    }

    if(EEPROM_ReadRaw(COPY_B_BASE,
                      (uint8_t *)&blockB,
                      EEPROM_BLOCK_SIZE) == EEPROM_OK)
    {
        validB = EEPROM_BlockValid(&blockB);
    }

    if(validA && validB)
    {
        /* Both valid: take the one with the higher (newer) counter.
         * On a tie (should never happen after first save) prefer A. */
        EEPROM_Block *newest =
            (blockB.counter > blockA.counter) ? &blockB : &blockA;

        memcpy(data, newest->data, EEPROM_DATA_SIZE);
        return EEPROM_OK;
    }

    if(validA)
    {
        memcpy(data, blockA.data, EEPROM_DATA_SIZE);
        return EEPROM_OK;
    }

    if(validB)
    {
        memcpy(data, blockB.data, EEPROM_DATA_SIZE);
        return EEPROM_OK;
    }

    return EEPROM_ERROR;
}



EEPROM_Status EEPROM_Save(const uint8_t *data)
{
    EEPROM_Block blockA;
    EEPROM_Block blockB;
    EEPROM_Block newBlock;

    int validA = 0;
    int validB = 0;

    if(EEPROM_ReadRaw(COPY_A_BASE,
                      (uint8_t *)&blockA,
                      EEPROM_BLOCK_SIZE) == EEPROM_OK)
    {
        validA = EEPROM_BlockValid(&blockA);
    }

    if(EEPROM_ReadRaw(COPY_B_BASE,
                      (uint8_t *)&blockB,
                      EEPROM_BLOCK_SIZE) == EEPROM_OK)
    {
        validB = EEPROM_BlockValid(&blockB);
    }

    /* Determine current highest counter so new write is unambiguously newer */
    uint32_t counter = 0;

    if(validA && validB)
        counter = (blockA.counter >= blockB.counter) ?
                   blockA.counter : blockB.counter;
    else if(validA)
        counter = blockA.counter;
    else if(validB)
        counter = blockB.counter;

    /* Build new block */
    newBlock.magic   = EEPROM_MAGIC;
    newBlock.version = EEPROM_VERSION;
    newBlock.length  = EEPROM_DATA_SIZE;
    newBlock.counter = counter + 1;

    memcpy(newBlock.data, data, EEPROM_DATA_SIZE);

    newBlock.crc = EEPROM_CalcCRC((uint8_t *)&newBlock, EEPROM_BLOCK_SIZE - 4);

    /* Overwrite the older copy (ping-pong) */
    uint16_t target;

    if(validA && validB)
    {
        /* Overwrite whichever has the lower counter */
        target = (blockA.counter <= blockB.counter) ?
                  COPY_A_BASE : COPY_B_BASE;
    }
    else if(validA)
    {
        /* A is the only valid copy; write new data to B */
        target = COPY_B_BASE;
    }
    else
    {
        /* Neither valid (first boot or both corrupt); start with A */
        target = COPY_A_BASE;
    }

    return EEPROM_WriteRaw(target,
                           (uint8_t *)&newBlock,
                           EEPROM_BLOCK_SIZE);
}
