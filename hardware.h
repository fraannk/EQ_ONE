/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: hardware.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
******************************************************************************
* Date    Id    Change
* --------------------
* 29. mar. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef HARDWARE_H_
  #define HARDWARE_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "emp_type.h"

/*****************************    Defines    *******************************/

// Use this #define to enable the EMP board
#define EMP

// Switch enumerator to be used on IO
enum io_state
{
  ON,
  OFF
};

// Data structure to hold a audio sample pair
typedef struct {
  INT16U left;
  INT16U right;
} sample_type ;

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void hardware_init( INT16U sample_freq );
/*****************************************************************************
*   Input    : The sample frequency
*   Output   : -
*   Function : Initialize all hardware
*              Note: Use #define EMP, to use the EMP onboard audio
******************************************************************************/

void pwm_clear_interrupt();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Clear the interrupt flags for the sample_handler()
******************************************************************************/

void line_in( enum io_state state );
/*****************************************************************************
*   Input    : ON / OFF
*   Output   : -
*   Function : Turns the line_in on or off
******************************************************************************/

void line_out( enum io_state state );
/*****************************************************************************
*   Input    : ON / OFF
*   Output   : -
*   Function : Turns the line_out on or off
******************************************************************************/

void audio_in(sample_type *sample);
/*****************************************************************************
*   Input    : Ptr to sample data struct
*   Output   : -
*   Function : Get the next audio sample
******************************************************************************/

void audio_out(sample_type sample);
/*****************************************************************************
*   Input    : Sample data struct
*   Output   : -
*   Function : Set the next audio sample to output
******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* HARDWARE_H_ */
