/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: systick.h
*
* PROJECT....: EMP
*
* DESCRIPTION: Initialize the systick interrupt.
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150215  MoH    Module created.
*
*****************************************************************************/

#ifndef _EMP_H
  #define _EMP_H

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define         TIM_10_MSEC         2
#define         TIM_25_MSEC         5
#define         TIM_50_MSEC         10
#define         TIM_100_MSEC        20
#define         TIM_250_MSEC        125
#define         TIM_500_MSEC        100
#define         TIM_1_SEC           200
#define         TIM_2_SEC           400
#define         TIM_4_SEC           800


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


void init_systick();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the systick interrupt.
******************************************************************************/


/****************************** End Of Module *******************************/
#endif


