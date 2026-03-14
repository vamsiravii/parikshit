#ifndef EEPROM_TEST_H
#define EEPROM_TEST_H

#include <stdint.h>

typedef struct
{
    uint8_t init;           /* 1 = EEPROM_Init() returned OK */
    uint8_t save_load;      /* 1 = saved pattern matched loaded pattern */
    uint8_t newest_wins;    /* 1 = after two saves, load returned the second one */
    uint8_t ping_pong;      /* 1 = 10 consecutive save/load cycles all correct */

    uint8_t all_passed;     /* 1 = all of the above are 1 */

} EEPROM_TestResults;

extern EEPROM_TestResults eeprom_test_results;

void EEPROM_RunTests(void);

#endif
