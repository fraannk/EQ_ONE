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
#include <stdlib.h>
#include "global.h"
#include "debug.h"
#include "hardware.h"
#include "dsp.h"
#include "string.h"

/*****************************    Defines    *******************************/
typedef struct band_s{
  INT8U            id;
  INT16S           gain;
  INT16U           frequency;
  INT16U           bandwidth;
  iir_filter_type  type;
  FP32             a_coeff[3];
  FP32             b_coeff[3];
  struct band_s    *prev_band;
  struct band_s    *next_band;
} band_t;

typedef struct ep {
  INT8U      id;
  char       name[40];
  band_t     *band;
  struct ep  *prev_profile;
  struct ep  *next_profile;
} ep_t;


/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U   levels[16];
INT8U   sample_count = 64;
INT32U  sample_sum = 0;

INT8U   eq_on = 0;

ep_t *equalizer_profiles = NULL;
ep_t *active_profil = NULL;


/*****************************   Functions   *******************************/
ep_t *profile_by_id(INT8U id)
{
  ep_t *profile = NULL;
  BOOLEAN found = FALSE;

  if( equalizer_profiles )
  {
    profile = equalizer_profiles;

    while( !found && profile )
    {
      if( profile->id == id )
        found = TRUE;
      else
      {
        if( profile->next_profile )
          profile = profile->next_profile;
      }
    }
  }

  return ( found ? profile : NULL );
}

void band_set_coef(band_t *band)
{
  coef_t coef;
  coef.a[0] = band->a_coeff[0];
  coef.a[1] = band->a_coeff[1];
  coef.a[2] = band->a_coeff[2];
  coef.b[0] = band->b_coeff[0];
  coef.b[1] = band->b_coeff[1];
  coef.b[2] = band->b_coeff[2];

  iir_filter_add(coef);
}

void profile_use(INT8U id)
{
  ep_t *profile = profile_by_id(id);

  if(profile)
  {
    active_profil = profile;

    iir_filter_clear();

    if(profile->band)
    {
      band_t *band = profile->band;
      while( band )
      {
        band_set_coef(band);
        band = band->next_band;
      }
    }

    iir_filter_use();
  }
}




INT8U profile_next_id()
{
  ep_t *current_ep = equalizer_profiles;

  INT8U new_id = 0;
  while( current_ep )
  {
    if( current_ep->id == new_id)
    {
      new_id++;
      current_ep = equalizer_profiles;
    }
    else
      current_ep = current_ep->next_profile;
  }

  return(new_id);
}

INT8U band_next_id(band_t *band)
{
  band_t *current_band = band;

  INT8U new_id = 0;
  while( current_band )
  {
    if( current_band->id == new_id)
    {
      new_id++;
      current_band = band;
    }
    else
      current_band = current_band->next_band;
  }

  return(new_id);
}

ep_t* profile_create()
{
  ep_t *eq_profile = (ep_t*)malloc(sizeof(ep_t));
  eq_profile->id = profile_next_id();
  eq_profile->next_profile = NULL;
  eq_profile->prev_profile = NULL;
  eq_profile->band = NULL;
  return(eq_profile);
}

band_t* band_create()
{
  band_t *band = (band_t *)malloc(sizeof(band_t));
  band->id = band_next_id(NULL);
  band->next_band = NULL;
  band->prev_band = NULL;
  band->gain = 0;
  band->frequency = 0;
  band->bandwidth = 0;
  memset(band->a_coeff, 0, sizeof(band->a_coeff));
  memset(band->b_coeff, 0, sizeof(band->b_coeff));
  band->type = iir_none;
  return(band);
}

void band_get_coef(band_t *band )
{
  coef_t *coef  = iir_coef(band->type,
                           band->frequency,
                           band->gain,
                           band->bandwidth);

  band->a_coeff[0] = coef->a[0];
  band->a_coeff[1] = coef->a[1];
  band->a_coeff[2] = coef->a[2];
  band->b_coeff[0] = coef->b[0];
  band->b_coeff[1] = coef->b[1];
  band->b_coeff[2] = coef->b[2];
}



ep_t *profile_last(ep_t *profile )
{
  ep_t *current_profile = profile;

  while( current_profile->next_profile )
  {
    current_profile = current_profile->next_profile;
  }

  return( current_profile );
}

band_t *band_last( band_t *band)
{
  band_t *current_band = band;

  while ( current_band->next_band )
  {
    current_band = current_band->next_band;
  }

  return( current_band );
}

void profile_add(ep_t *profile)
{
  if( equalizer_profiles )
  {
    ep_t *last_profile = profile_last(equalizer_profiles);
    last_profile->next_profile = profile;
    profile->prev_profile = last_profile;
  }
  else
  {
    equalizer_profiles = profile;
    profile->prev_profile = NULL;
  }
}

void profile_add_band( ep_t *profile, band_t *band )
{
  if(profile->band)
  {
    band_t *last_band = band_last(profile->band);
    last_band->next_band = band;
    band->prev_band = last_band;
  }
  else
  {
    profile->band = band;
    band->prev_band = NULL;
  }
}

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

    FP32 left =  dsp_iir_filter(left_ch);
    FP32 right = dsp_iir_filter(right_ch);

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

  // Make first Profile
  ep_t *profile = profile_create();
  strcpy(profile->name, "No bass");

  band_t *band_1 = band_create();
  band_1->type = iir_ls;
  band_1->bandwidth = 0;
  band_1->frequency = 400;
  band_1->gain = -40;
  band_get_coef(band_1);
  profile_add_band( profile, band_1 );

  band_t *band_2 = band_create();
  band_2->type = iir_hs;
  band_2->bandwidth = 0;
  band_2->frequency = 17000;
  band_2->gain = -40;
  band_get_coef(band_2);
  profile_add_band( profile, band_2 );

  band_t *band_3 = band_create();
  band_3->type = iir_peak;
  band_3->bandwidth = 1000;
  band_3->frequency = 3000;
  band_3->gain = 20;
  band_get_coef(band_3);
  profile_add_band( profile, band_3 );

  profile_add( profile );

  profile_use(profile->id);

}

/****************************** End Of Module *******************************/
