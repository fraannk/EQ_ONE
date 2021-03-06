/*****************************************************************************
 * University of Southern Denmark
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
// Equalizer profile band data strucure
typedef struct band_s{
  INT8U            id;              // Band id
  FP32             gain;            // Gain in dB
  INT16U           frequency;       // Frequency in Hz
  INT16U           bandwidth;       // Bandwidth in Hz
  iir_filter_type  type;            // Filter type used on band
  FP32             a_coeff[3];      // DSP calculated A coefficients
  FP32             b_coeff[3];      // DSP calculated B coefficients
  struct band_s    *prev_band;      // Pointer to prev. band
  struct band_s    *next_band;      // Pointer to next. band
} band_t;

// Equalizer profile  data strucure
typedef struct ep {
  INT8U      id;                    // Equalizer profile id
  char       name[40];              // Profile name
  FP32       gain;                  // Master gain in dB
  INT8U      spectrum[16];          // Stored equalizer spectrum
  band_t     *band;                 // Pointer to first eq. band
  struct ep  *prev_profile;         // Pointer to prev. eq. profile
  struct ep  *next_profile;         // Pointer to next. eq. profile
} ep_t;

// Simple OnOff enum
typedef enum{
  EQ_ON,
  EQ_OFF
}state_t;
/*****************************   Constants   *******************************/
const char eq_bars[2][16] = { {32,32,32,32,32,32,32,32, 0, 1, 2, 3, 4, 5, 6, 7},
                              { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7} };

const char eq_profil[2][16] = { {32,32,32,32,32,32,32,32, 0, 1, 2, 3, 4, 5, 6, 7},
                                { 0, 1, 2, 3, 4, 5, 6, 7,32,32,32,32,32,32,32,32} };

/*****************************   Variables   *******************************/
// Equalizer state, default off
state_t equalizer_state = OFF;

// Start pointer to EQ profiles
ep_t *equalizer_profiles = NULL;
ep_t *active_profil = NULL;

// Time (cycles) spent inside the audio interrupt
INT32U audio_int_time = 0;

// Sample holds the current sample filled in by the sample_handler
sample_type sample;

INT16U eq_display_freq_log[16];

/*****************************   Functions   *******************************/

ep_t *profile_by_id(INT8U id)
/*****************************************************************************
 *   Input    : Profile id
 *   Output   : Pointer to eq. profile or NULL
 *   Function : Get a eq. profile by id
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : Pointer to a band
 *   Output   : -
 *   Function : Set stored filter coefficients on DSP module
 ******************************************************************************/
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
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  // store eq. state for later
  state_t eq_state = equalizer_state;

  // turn off the equalizer
  equalizer_state = EQ_OFF;

  ep_t *profile = profile_by_id(id);

  if(profile)
  {
    active_profil = profile;

    iir_filter_clear();

    iir_filter_master_gain( profile->gain );

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

  // restore prev. eq state
  equalizer_state = eq_state;
}


INT8U profile_next_id()
/*****************************************************************************
 *   Input    : -
 *   Output   : Profile id
 *   Function : Get the first free profile id
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : Pointer to a eq. band
 *   Output   : Band id
 *   Function : Get the first free band id
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : -
 *   Output   : Pointer to an eq. profile
 *   Function : Creates an empty eq. profile
 ******************************************************************************/
{
  ep_t *eq_profile = (ep_t*)malloc(sizeof(ep_t));
  eq_profile->id = profile_next_id();
  eq_profile->gain = 0;
  eq_profile->next_profile = NULL;
  eq_profile->prev_profile = NULL;
  eq_profile->band = NULL;
  return(eq_profile);
}

band_t* band_create()
/*****************************************************************************
 *   Input    : -
 *   Output   : Pointer to an eq. band
 *   Function : Creates an empty eq. band
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : Pointer to an eq. band
 *   Output   : -
 *   Function : Fill out A and B coefficients from DSP module on eq. band
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : Pointer to an eq. profile
 *   Output   : Pointer to an eq. profile
 *   Function : Find the last eq. profile and return its pointer
 ******************************************************************************/
{
  ep_t *current_profile = profile;

  while( current_profile->next_profile )
  {
    current_profile = current_profile->next_profile;
  }

  return( current_profile );
}

band_t *band_last( band_t *band)
/*****************************************************************************
 *   Input    : Pointer to an eq. band
 *   Output   : Pointer to an eq. band
 *   Function : Find the last eq. band and return its pointer
 ******************************************************************************/
{
  band_t *current_band = band;

  while ( current_band->next_band )
  {
    current_band = current_band->next_band;
  }

  return( current_band );
}

void profile_add(ep_t *profile)
/*****************************************************************************
 *   Input    : Pointer to an eq. profile
 *   Output   : -
 *   Function : Add the profile to the list
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : A eq. profile and eq. band pointer
 *   Output   : -
 *   Function : Add the band to the profile
 ******************************************************************************/
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

void equalizer_create_spectrum(ep_t *profile )
/*****************************************************************************
 *   Input    : Pointer to an eq. profile
 *   Output   : -
 *   Function : Get the calculated frequency spectrum for an eq. profile
 ******************************************************************************/
{
  INT8U amp;

  for( INT8U i = 0 ; i < 16; i++)
  {
    amp = (INT8U)((dsp_filter_amplitude( eq_display_freq_log[ i ] / 2 )) + 8 );
    profile->spectrum[i] = amp > 15 ? 15 : amp;
  }
}

void equalizer_lcd_profile_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  for(INT8U i = 0; i < 16; i++)
  {
    lcd_set_cursor( i, 0);
    lcd_write_char( eq_profil[ 0 ][ active_profil->spectrum[i] ] );
    lcd_set_cursor( i, 1);
    lcd_write_char( eq_profil[ 1 ][ active_profil->spectrum[i] ] );
  }
}

void equalizer_lcd_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
/*****************************************************************************
 *   Header description
 ******************************************************************************/
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
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Main function called by the NVIC
 *            : This function handles the jobs related to audio in and out
 ******************************************************************************/
{
  // Set debug pins for scope debugging
  debug_pins_toggle(DEBUG_P1);
  debug_pins_high(DEBUG_P2);

  // Zero timer for cpu load calculations
  timer_set(0);

  // Clear the interrupt flags
  pwm_clear_interrupt();

  // Send last sample
  audio_out(sample);

  // Get new sample
  audio_in(&sample);

  // if eq. on then calculate filters on left and right channel
  if( equalizer_state == EQ_ON )
  {
    sample.left = dsp_iir_filter(sample.left);
    sample.right = dsp_iir_filter(sample.right);
  }

  // set audio time used
  audio_int_time = timer_get();

  // Set debug pins low for scope debugging
  debug_pins_low(DEBUG_P2);
}

void equalizer_onoff()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  equalizer_state = equalizer_state == EQ_ON ? EQ_OFF : EQ_ON;
}

void equalizer_on()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  equalizer_state = EQ_ON;
}

void equalizer_off()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  equalizer_state = EQ_OFF;
}

void equalizer_profiles_setup()
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : This makes the complete setup off all default eq. profiles in
 *              the equalizer module.
 ******************************************************************************/
{
  band_t *band;
  ep_t *profile;

  // Make p0 Profile
  profile = profile_create();
  strcpy(profile->name, "Rock");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 20;
  band->bandwidth = 10;
  band->gain = -5;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 60;
  band->bandwidth = 10;
  band->gain = 5;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 170;
  band->bandwidth = 100;
  band->gain = 4;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 310;
  band->bandwidth = 30;
  band->gain = -5;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 600;
  band->bandwidth = 10;
  band->gain = -6;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 6000;
  band->bandwidth = 7000;
  band->gain = -7;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 3000;
  band->bandwidth = 1000;
  band->gain = -4;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_hs;
  band->bandwidth = 600;
  band->frequency = 19000;
  band->gain = -5;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p0
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make p1 Profile
  profile = profile_create();
  strcpy(profile->name, "Classic");
  profile->gain = 5;

  band = band_create();
  band->type = iir_peak;
  band->bandwidth = 10;
  band->frequency = 20;
  band->gain = -9;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 6000;
  band->gain = -7;
  band->bandwidth = 200;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 14000;
  band->gain = -7;
  band->bandwidth = 400;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_hs;
  band->bandwidth = 500;
  band->frequency = 16000;
  band->gain = -9;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p1
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P2 Profile
  profile = profile_create();
  strcpy(profile->name, "Megafon");
  profile->gain = -10;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 600;
  band->gain = 25;
  band->bandwidth = 200;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p2
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P3 Profile
  profile = profile_create();
  strcpy(profile->name, "Test 1");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 19;
  band->gain = 16;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p3
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P4 Profile
  profile = profile_create();
  strcpy(profile->name, "Bass Boost");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 50;
  band->gain = 6;
  band->bandwidth = 60;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 170;
  band->gain = 4;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 310;
  band->gain = 4;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p4
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P5 Profile
  profile = profile_create();
  strcpy(profile->name, "High Boost");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 6000;
  band->gain = 6;
  band->bandwidth = 500;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 9000;
  band->gain = 4;
  band->bandwidth = 1000;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 16000;
  band->gain = 4;
  band->bandwidth = 2000;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p5
  profile_use( profile->id);
  equalizer_create_spectrum( profile );


  // Make P6 Profile
  profile = profile_create();
  strcpy(profile->name, "Bass/high Boost");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 50;
  band->gain = 6;
  band->bandwidth = 60;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 170;
  band->gain = 4;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 310;
  band->gain = 4;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 6000;
  band->gain = 6;
  band->bandwidth = 500;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 9000;
  band->gain = 4;
  band->bandwidth = 1000;
  band_get_coef(band);
  profile_add_band( profile, band );

  band = band_create();
  band->type = iir_peak;
  band->frequency = 16000;
  band->gain = 4;
  band->bandwidth = 2000;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p6
  profile_use( profile->id);
  equalizer_create_spectrum( profile );


  // Make P7 Profile
  profile = profile_create();
  strcpy(profile->name, "1k Notch ");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 1000;
  band->gain = -40;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p7
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P8 Profile
  profile = profile_create();
  strcpy(profile->name, "EMP komp");
  profile->gain = 0;

  band = band_create();
  band->type = iir_hs;
  band->frequency = 15915;
  band->gain = 4.653;
  band->bandwidth = 1;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for p8
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P9 Profile
  profile = profile_create();
  strcpy(profile->name, "1k, 5db, 100bw");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 1000;
  band->gain = 5;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for P9
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P10 Profile
  profile = profile_create();
  strcpy(profile->name, "1k, -5db, 100bw");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 1000;
  band->gain = -5;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for P10
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P11 Profile
  profile = profile_create();
  strcpy(profile->name, "1k, 7db, 100bw");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 1000;
  band->gain = 7;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for P11
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

  // Make P12 Profile
  profile = profile_create();
  strcpy(profile->name, "1k, -7db, 100bw");
  profile->gain = 0;

  band = band_create();
  band->type = iir_peak;
  band->frequency = 1000;
  band->gain = -7;
  band->bandwidth = 100;
  band_get_coef(band);
  profile_add_band( profile, band );

  profile_add( profile );

  // Init spectrum for P12
  profile_use( profile->id);
  equalizer_create_spectrum( profile );

}

void equalizer_init()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  // Initialize the frequency bin to be used on the profile display
  dsp_filter_log_freq( eq_display_freq_log, 16 );

  // Setup all default profiles
  equalizer_profiles_setup();

  // Set active profile with id = 0
  profile_use( 0);

  // Turn audio on
  line_in( ON );
  line_out( ON );
}

/****************************** End Of Module *******************************/
