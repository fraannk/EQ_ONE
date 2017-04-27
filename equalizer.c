/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: equalizer.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Equalizer module
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 17. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "equalizer.h"
#include <stdint.h>
#include "global.h"
#include "debug.h"
#include "hardware.h"
#include "dsp.h"

/*****************************    Defines    *******************************/
typedef struct band_s{
  INT8U            id;
  INT8U            name[40];
  INT16U           gain;
  INT16U           frequency;
  INT16U           bandwidth;
  INT8U            type;
  FP32             a_coeff[3];
  FP32             b_coeff[3];
  struct band_s    *next_band;
} band_t;

typedef struct ep {
  INT8U      id;
  INT8U      name[40];
  band_t     *band;
  struct ep  *next_profile;
} ep_t;


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U   levels[16];
INT8U   sample_count = 64;
INT32U  sample_sum = 0;

INT8U   eq_on = 0;

ep_t *equalizer_profile;

/*****************************   Functions   *******************************/





extern void sample_handler()
{
  debug_pins_toggle(DEBUG_P1);
  debug_pins_high(DEBUG_P2);

  pwm_clear_interrupt();

  static sample_type sample;
  audio_out(sample);
  audio_in(&sample);

  if( eq_on )
  {
    FP32 left_ch = (FP32)(sample.left-2048);
    FP32 right_ch = (FP32)(sample.right-2048);

    //FP32 left =  iir_filter_sos(left_ch, a, b, W_L);
    //FP32 right = iir_filter_sos(right_ch, a, b, W_R);

    FP32 left =  iir_filter_cascade(left_ch);
    FP32 right = iir_filter_cascade(right_ch);

    sample.left = (INT16U)(left+2048);
    sample.right = (INT16U)(right+2048);
  }

  debug_pins_low(DEBUG_P2);

  // do important stuff here and fast

/*
  if(sample.left > 2048)
    emp_set_led   ( LED_GREEN );
  if(sample.left > 3072)
    emp_set_led ( LED_GREEN | LED_YELLOW);
  if(sample.left > 3500)
    emp_set_led ( LED_GREEN | LED_YELLOW | LED_RED);

  if( sample_count > 0 )
  {
    sample_count--;
    if( sample.left > 2048 )
      sample_sum += (INT32U)( (sample.left>>4)-127 );
  }
  else
  {
    sample_count=64;
    for( INT8U i=16; i>0; i--)
    {
      levels[i] = levels[i-1];
    }
    levels[0] = sample_sum>>2;
    sample_sum = 0;
  }
*/
}

void equalizer_onoff()
{
  eq_on = eq_on ? 0 : 1;
}

void equalizer_init()
{
  // Turn audio on
  line_in( ON );
  line_out( ON );

  iir_init_dsp_states();

}

/****************************** End Of Module *******************************/
