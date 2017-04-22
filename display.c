/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: display.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Module for display driver
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 16. mar. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "display.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "hardware.h"
#include "emp_type.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void eq_build_display(INT8U levels[], INT8U *buffer)
{
    static INT8U bars[2][16] = { {32,32,32,32,32,32,32,32, 0, 1, 2, 3, 4, 5, 6, 7},
                                 { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7} };

    INT8U level;

    for(INT8U i=0; i<16; i++)
    {
        level = levels[i]>>4;
        buffer[i]    = bars[0][level];
        buffer[16+i] = bars[1][level];

    }
}

/****************************** End Of Module *******************************/
