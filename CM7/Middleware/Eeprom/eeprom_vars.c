#include "eeprom_vars.h"

uint16_t VirtAddVarTab[NB_OF_VAR] =
{
#define VAR(x) x
#include "eeprom_vars_list.h"
#undef VAR
};
