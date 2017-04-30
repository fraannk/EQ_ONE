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

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/


/*************************  Function interfaces ****************************/

void eq_build_display(INT8U levels[], INT8U *buffer);


/****************************** End Of Module *******************************/
#endif /* DISPLAY_H_ */
