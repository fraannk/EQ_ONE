/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: hardware.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Hardware module for EQ_ONE project
 *              Project Mode
 *                Left-in    PB5 (AIN11)   ADC0
 *                Right-in   PB4 (AIN10)   ADC0
 *
 *                PWM Mode
 *                Left-out   PB7 (M0PWM1)  PWM module 0 PWM generator 0
 *                Right-out  PB6 (M0PWM0)  PWM module 0 PWM generator 0
 *
 *                DAC Mode
 *                PA2  (SCK)
 *                PA3  (CS)
 *                PA5  (SDA)
 *                PE1  (LDAC)    // Todo:Not yet implemented
 *                PE2  (SHDN)    // Todo:Not yet implemented
 *
 *              EMP Mode (with #define EMP) for on board audio circuit
 *                in         PE5 (AIN8)    ADC0
 *                out        PE4 (M0PWM4)  PWM module 0 PWM generator 2
 *
 * Change Log:
 *****************************************************************************
 * Date    Id    Change
 * --------------------
 * 29. mar. 2017  jorn    Module created.
 *
 *****************************************************************************/

#ifndef HARDWARE_H_
#define HARDWARE_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "emp_type.h"

/*****************************    Defines    *******************************/

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

void timer_set(INT32U time );
/*****************************************************************************
 *   Input    : Start timer counter
 *   Output   : -
 *   Function : Set timer to time value and start counting
 ******************************************************************************/

INT32U timer_get();
/*****************************************************************************
 *   Input    : Current count from timer
 *   Output   : -
 *   Function : Get the current count from timer i ticks
 ******************************************************************************/

void delay_us(INT32U time);
/*****************************************************************************
 *   Input    : Delay timer in approx microseconds
 *   Output   : -
 *   Function : delays for approx time in microseconds
 ******************************************************************************/

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
