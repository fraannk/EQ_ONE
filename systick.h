/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: systick.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Scheduler module for the EQ_ONE system
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 16. apr. 2017  jorn    Module adopted from MOH systick
*
*****************************************************************************/

#ifndef _EMP_H
  #define _EMP_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void enable_global_int();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Enable global interrupt.
******************************************************************************/

void disable_global_int();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Disable global interrupt.
******************************************************************************/

void systick_init();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the systick interrupt.
******************************************************************************/


/****************************** End Of Module *******************************/
#endif


