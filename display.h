/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: display.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Module for display driver
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 17. mar. 2017	jorn    Module created.
 *
 *****************************************************************************/

#ifndef DISPLAY_H_
#define DISPLAY_H_

/***************************** Include files *******************************/
#include "emp_type.h"
#include "scheduler.h"

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void eq_build_display(INT8U levels[], INT8U *buffer);
/*****************************************************************************
 *   Input    : Array of 16 level between 0..255
 *            : Pointer to a LCD-display buffer
 *   Output   : -
 *   Function : Renders a horizontal waveform based on the levels array
 ******************************************************************************/

void display_task ( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : Task to handle the display mode select
 ******************************************************************************/


/****************************** End Of Module *******************************/
#endif /* DISPLAY_H_ */
