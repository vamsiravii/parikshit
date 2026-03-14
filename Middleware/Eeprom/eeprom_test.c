#include "eeprom_test.h"
#include "eeprom_driver.h"
#include <string.h>

/*
 * Results are stored here so you can inspect them in the debugger
 * by adding a Watch Expression on `eeprom_test_results`.
 *
 * Each field is 1 if the test passed, 0 if it failed.
 */
EEPROM_TestResults eeprom_test_results = {0};


/* Helper — fills a buffer with a known pattern based on a seed */
static void fill_pattern(uint8_t *buf, uint8_t seed)
{
    for(int i = 0; i < EEPROM_DATA_SIZE; i++)
        buf[i] = (uint8_t)(seed + i);
}

static int check_pattern(const uint8_t *buf, uint8_t seed)
{
    for(int i = 0; i < EEPROM_DATA_SIZE; i++)
        if(buf[i] != (uint8_t)(seed + i))
            return 0;
    return 1;
}


/*
 * TEST 1 — Init
 * Just verifies EE_Init completes without error.
 */
static void test_init(void)
{
    eeprom_test_results.init =
        (EEPROM_Init() == EEPROM_OK) ? 1 : 0;
}


/*
 * TEST 2 — Save then Load
 * Writes a known pattern, reads it back, checks it matches.
 */
static void test_save_load(void)
{
    uint8_t write_buf[EEPROM_DATA_SIZE];
    uint8_t read_buf[EEPROM_DATA_SIZE];

    fill_pattern(write_buf, 0xAA);
    memset(read_buf, 0, sizeof(read_buf));

    if(EEPROM_Save(write_buf) != EEPROM_OK)
    {
        eeprom_test_results.save_load = 0;
        return;
    }

    if(EEPROM_Load(read_buf) != EEPROM_OK)
    {
        eeprom_test_results.save_load = 0;
        return;
    }

    eeprom_test_results.save_load =
        (memcmp(write_buf, read_buf, EEPROM_DATA_SIZE) == 0) ? 1 : 0;
}


/*
 * TEST 3 — Multiple saves, newest wins
 * Saves pattern A then pattern B. Load should return B, not A.
 */
static void test_newest_wins(void)
{
    uint8_t buf_a[EEPROM_DATA_SIZE];
    uint8_t buf_b[EEPROM_DATA_SIZE];
    uint8_t read_buf[EEPROM_DATA_SIZE];

    fill_pattern(buf_a, 0x11);
    fill_pattern(buf_b, 0x22);

    if(EEPROM_Save(buf_a) != EEPROM_OK ||
       EEPROM_Save(buf_b) != EEPROM_OK)
    {
        eeprom_test_results.newest_wins = 0;
        return;
    }

    if(EEPROM_Load(read_buf) != EEPROM_OK)
    {
        eeprom_test_results.newest_wins = 0;
        return;
    }

    /* Should match B, not A */
    eeprom_test_results.newest_wins =
        check_pattern(read_buf, 0x22) ? 1 : 0;
}


/*
 * TEST 4 — Ping-pong across many saves
 * Saves 10 times with incrementing seeds. Each load must return
 * the last written seed. Verifies the counter and target-selection
 * logic holds up over multiple cycles.
 */
static void test_ping_pong(void)
{
    uint8_t write_buf[EEPROM_DATA_SIZE];
    uint8_t read_buf[EEPROM_DATA_SIZE];

    eeprom_test_results.ping_pong = 1;  /* assume pass, fail on first error */

    for(uint8_t seed = 0x30; seed < 0x3A; seed++)
    {
        fill_pattern(write_buf, seed);

        if(EEPROM_Save(write_buf) != EEPROM_OK)
        {
            eeprom_test_results.ping_pong = 0;
            return;
        }

        memset(read_buf, 0, sizeof(read_buf));

        if(EEPROM_Load(read_buf) != EEPROM_OK)
        {
            eeprom_test_results.ping_pong = 0;
            return;
        }

        if(!check_pattern(read_buf, seed))
        {
            eeprom_test_results.ping_pong = 0;
            return;
        }
    }
}


/*
 * Run all tests in sequence.
 * Call this once from main() after SystemClock_Config() and HAL_Init().
 *
 * Then in STM32CubeIDE:
 *   1. Run → Debug
 *   2. Set a breakpoint on the closing brace of this function
 *   3. When it hits, open Expressions and add `eeprom_test_results`
 *   4. All fields should be 1
 */
void EEPROM_RunTests(void)
{
    test_init();
    test_save_load();
    test_newest_wins();
    test_ping_pong();

    /* Breakpoint here — inspect eeprom_test_results in the debugger */
    eeprom_test_results.all_passed =
        eeprom_test_results.init        &&
        eeprom_test_results.save_load   &&
        eeprom_test_results.newest_wins &&
        eeprom_test_results.ping_pong;
}
