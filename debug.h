/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: debug.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 16. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef DEBUG_H_
  #define DEBUG_H_

/***************************** Include files *******************************/
#include "tm4c123gh6pm.h"
#include "emp_type.h"

/*****************************    Defines    *******************************/
#define       DEBUG_PORT      GPIO_PORTB_DATA_R

// Debug PIN-MAP
#define       DEBUG_P1        0x1
#define       DEBUG_P2        0x2
#define       DEBUG_P3        0x8



/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void debug_pins_high(INT8U pins);
void debug_pins_low(INT8U pins);
void debug_pins_toggle(INT8U pins);

/****************************** End Of Module *******************************/
#endif /* DEBUG_H_ */
