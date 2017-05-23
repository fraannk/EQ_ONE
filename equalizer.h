/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: equalizer.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Equalizer module
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 17. apr. 2017	jorn    Module created.
 *
 *****************************************************************************/

#ifndef EQUALIZER_H_
#define EQUALIZER_H_

/***************************** Include files *******************************/
#include "emp_type.h"
#include "scheduler.h"

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void equalizer_init();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Initialize the equalizer module
 ******************************************************************************/

void equalizer_onoff();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Toggle the equalizer state
 ******************************************************************************/

void equalizer_on();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Turn the equalizer state to on
 ******************************************************************************/

void equalizer_off();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Turn the equalizer state to off
 ******************************************************************************/

void equalizer_lcd_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : Task to view default equalizer information on LCD
 ******************************************************************************/

void equalizer_lcd_profile_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : Task to view setected profile frequency spectrum on LCD
 ******************************************************************************/

void profile_use(INT8U id);
/*****************************************************************************
 *   Input    : Profile id
 *   Output   : -
 *   Function : Use profile by id
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* EQUALIZER_H_ */
