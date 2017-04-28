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
void equalizer_onoff();
void equalizer_lcd_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );

/****************************** End Of Module *******************************/
#endif /* EQUALIZER_H_ */
