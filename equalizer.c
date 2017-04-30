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
#include "scheduler.h"
#include "emp_lcd1602.h"
#include "gfstring.h"

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

typedef enum{
  EQ_ON,
  EQ_OFF
}state_t;
/*****************************   Constants   *******************************/
const char eq_bars[2][16] = { {32,32,32,32,32,32,32,32, 0, 1, 2, 3, 4, 5, 6, 7},
                               { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7} };

/*****************************   Variables   *******************************/
// Equalizer state, default off
state_t equalizer_state = OFF;

// Start pointer to EQ profiles
ep_t *equalizer_profiles = NULL;
ep_t *active_profil = NULL;

// Time (cycles) spent inside the audio interrupt
INT32U audio_int_time = 0;

// Sample holds the current sample filled in by the sample_handler
volatile sample_type sample;

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

void equalizer_lcd_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
{
  INT8U level_left;
  INT8U level_right;

  lcd_clear();

  // VU-meter
  if(sample.left > 2047)
    level_left = (INT8U)((sample.left-2047)>>7);
  else
    level_left = (INT8U)((2047-sample.left)>>7);

  if(sample.right > 2047)
    level_right = (INT8U)((sample.right-2047)>>7);
  else
    level_right = (INT8U)((2047-sample.right)>>7);

  lcd_set_cursor(0, 0);
  lcd_write_char(eq_bars[0][level_left]);
  lcd_write_char(eq_bars[0][level_right]);
  lcd_set_cursor(0, 1);
  lcd_write_char(eq_bars[1][level_left]);
  lcd_write_char(eq_bars[1][level_right]);

  // Profile name
  lcd_set_cursor(2,0);
  lcd_write(active_profil->name);
  lcd_set_cursor(2,1);
  lcd_write("Eq:");

  switch( equalizer_state )
  {
    case EQ_ON:
      lcd_write("On");
      break;
    case EQ_OFF:
      lcd_write("Off");
      break;
  }

  lcd_set_cursor(9,1);
  lcd_write("CPU%:");

  INT32U pct = (INT32U)((100/1814.0)*audio_int_time);
  char b[3];
  lcd_write( itoa(pct, b) );
}

extern void sample_handler()
{
  debug_pins_toggle(DEBUG_P1);
  debug_pins_high(DEBUG_P2);

  timer_set(0);

  pwm_clear_interrupt();

  audio_out(sample);
  audio_in(&sample);

  if( equalizer_state == EQ_ON )
  {
    sample.left = dsp_iir_filter(sample.left);
    sample.right = dsp_iir_filter(sample.right);
  }

  audio_int_time = timer_get();
  debug_pins_low(DEBUG_P2);
}

void equalizer_onoff()
{
  equalizer_state = equalizer_state == EQ_ON ? EQ_OFF : EQ_ON;
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

  band_t *band_4 = band_create();
  band_4->type = iir_peak;
  band_4->bandwidth = 1000;
  band_4->frequency = 9000;
  band_4->gain = 20;
  band_get_coef(band_4);
  profile_add_band( profile, band_4 );

  profile_add( profile );

  // Make second Profile
  ep_t *profile2 = profile_create();
  strcpy(profile2->name, "More music");

  band_t *band_21 = band_create();
  band_21->type = iir_ls;
  band_21->bandwidth = 0;
  band_21->frequency = 200;
  band_21->gain = -10;
  band_get_coef(band_21);
  profile_add_band( profile2, band_21 );

  band_t *band_22 = band_create();
  band_22->type = iir_hs;
  band_22->bandwidth = 0;
  band_22->frequency = 12000;
  band_22->gain = 10;
  band_get_coef(band_22);
  profile_add_band( profile2, band_22 );

  band_t *band_23 = band_create();
  band_23->type = iir_notch;
  band_23->bandwidth = 1000;
  band_23->frequency = 3000;
  band_23->gain = 10;
  band_get_coef(band_23);
  profile_add_band( profile2, band_23 );

  profile_add( profile2 );

  profile_use( profile->id);

}

/****************************** End Of Module *******************************/
