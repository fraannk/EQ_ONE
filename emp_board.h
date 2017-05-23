/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: emp_board.c
 *
 * PROJECT....: EMP
 *
 * DESCRIPTION: Module to handle EMP functions
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 170210  JJA   Module created.
 *
 *****************************************************************************/

#ifndef EMP_BOARD_
#define EMP_BOARD_

/***************************** Include files *******************************/
#include "emp_type.h"

/*****************************    Defines    *******************************/

#define LED_RED         0b100
#define LED_YELLOW      0b010
#define LED_GREEN       0b001

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void emp_set_led(INT8U led);
/*****************************************************************************
 *   Input    : INT8U : LED_RED | LED_YELLOW | LED_GREEN
 *   Output   : void
 *   Function : Set the selected LED combination on the EMP board
 ******************************************************************************/

void emp_toggle_status_led();
/*****************************************************************************
 *   Input    : -
 *   Output   : void
 *   Function : Toggle the EMP board status LED
 ******************************************************************************/

void emp_clear_all_led(void);
/*****************************************************************************
 *   Input    : void
 *   Output   : void
 *   Function : Clears all LEDs on the EMP board
 ******************************************************************************/

void emp_toggle_led(INT8U led);
/*****************************************************************************
 *   Input    : INT8U : LED_RED | LED_YELLOW | LED_GREEN
 *   Output   : void
 *   Function : Toggle the selected LED combination on the EMP board
 ******************************************************************************/


/****************************** End Of Module *******************************/
#endif
