/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: debug.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 16. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "debug.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "hardware.h"
#include "emp_type.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void debug_pins_high(INT8U pins)
{
  DEBUG_PORT |= pins;
}

void debug_pins_low(INT8U pins)
{
  DEBUG_PORT &= ~pins;
}

void debug_pins_toggle(INT8U pins)
{
  DEBUG_PORT ^= pins;
}

/****************************** End Of Module *******************************/
