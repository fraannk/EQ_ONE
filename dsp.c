/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: dsp.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION:
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 30. mar. 2017  Dennis    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "dsp.h"
#include <stdint.h>
#include <math.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "global.h"


/*****************************    Defines    *******************************/
// Math constants
#define PI      3.14159265f
#define SQRT_2  1.4142135623f

// Filter
#define MAX_BAND      10

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

typedef struct params{
 FP32 bandwith;
 FP32 frequency;
 FP32 gain;
 INT8U filter_type;
}param_t;

typedef enum{
  dm_float,
  dm_integer,
  dm_fixed
}dsp_mode;

dsp_mode current_dsp_mode = dm_float;

FP32 A[MAX_BAND][3];      // coefficient buffer for iir_cascade()
FP32 B[MAX_BAND][3];      // coefficient buffer for iir_cascade()
FP32 W[MAX_BAND][3];      // state variable buffer for iir_cascade()
FP32 A_temp[MAX_BAND][3]; // coefficient buffer for temp values
FP32 B_temp[MAX_BAND][3]; // coefficient buffer for temp values
FP32 master_gain;

param_t parameters;
coef_t  temp_coef;
BOOLEAN filter_is_on;
INT8U active_band = 0;
INT8U active_band_temp=0;



/*****************************   Functions   *******************************/

FP32 dsp_filter_amplitude(INT16U frequency)
{
  FP32  num_real,
        num_imag,
        denom_real,
        denom_imag,
        num,
        denom,
        result = 1,
        Omega;

  Omega = (2.0*PI*((FP32)frequency))*(1.0/SAMPLE_RATE);
  for(INT8U i=0; i < active_band ;i++)
  {
    num_real = B[i][0] + (B[i][1]*cos(Omega)) + (B[i][2]*cos(2.0*Omega));
    num_imag = (B[i][1]*sin(Omega)) + (B[i][2]*sin(2.0*Omega));
    denom_real = A[i][0] + (A[i][1]*cos(Omega)) + (A[i][2]*cos(2.0*Omega));
    denom_imag = (A[i][1]*sin(Omega)) + (A[i][2]*sin(2.0*Omega)) ;
    num =sqrt( (num_real*num_real) + (num_imag*num_imag) ) ;
    denom =sqrt( (denom_real*denom_real) + (denom_imag*denom_imag) ) ;
    result *= (num/denom);
  }
  return (20.0*log10(result*master_gain));
}

void dsp_filter_log_freq(INT16U* frequency_arr,INT8U size)
{
  FP32 log_steps,f;
  log_steps = (log10(20000.0) - log10(20.0))/((FP32)2.0*size);
  INT8U i,j =0;
  for(i=0 ; i < 2*size; i++)
  {
    if( i % 2 != 0) // odd number½
    {
      f = log10(20.0) + i*log_steps;
      frequency_arr[j] = pow(10.0,f);
      j++;
    }
  }
}

void dsp_mode_float()
{
  current_dsp_mode = dm_float;
}

void dsp_mode_integer()
{
  current_dsp_mode = dm_integer;
}

void dsp_mode_fixed()
{
  current_dsp_mode = dm_fixed;
}

void iir_init_dsp_states()
{
  for(INT8U i=0; i < MAX_BAND; i++)
  {
    for(INT8U j=0; j < 3; j++)
    {
      W[i][j] = 0;
    }
  }
  iir_filter_disable();
}

FP32 iir_filter_sos(FP32 in,            /* input sample */
                    FP32 *a,            /* a coefficients */
                    FP32 *b,            /* b coefficients */
                    FP32 *states_sos)   /* sos states */
{
  FP32 out = 0;
  // difference equation
  states_sos[0] = in - a[1]*states_sos[1] - a[2]*states_sos[2];
  out = b[0]*states_sos[0] + b[1]*states_sos[1] + b[2]*states_sos[2];

 // set next states
  states_sos[2] = states_sos[1];
  states_sos[1] = states_sos[0];

  return out;
}

INT16U dsp_iir_filter( INT16U sample )            /* input sample */
{
  // Set returning value and initialize it with the input sample value
  INT16U sample_out = sample;

  // Switchcase to handle the different modes of the DSP module
  switch(current_dsp_mode)
  {
    case dm_float:
      ; // empty line to fix variable define right after a case.. go figure..
      FP32 in = (FP32)(sample-2048);
      FP32 out = in;

      out = in;
      if(filter_on())
      {
        for(INT8U i=0; i< active_band;i++)
          out = iir_filter_sos( out, A[i] , B[i] , W[i] );
      }
      out = master_gain*out;
      if( out > 2048 )
        sample_out = 2048;
      if( out < -2048)
        sample_out = -2048;

      sample_out = (INT16U)(out+2048);
      break;
    case dm_integer:
      // do integer stuff here and call a new variation of the iir_filter_sos
      break;
    case dm_fixed:
      // do fixed stuff here and call yet another variation of the iir_filter_sos
      break;
  }
  return sample_out;
}

void iir_calc_coef_peak(FP32 *a,FP32 *b)
{

  FP32 G,w_0,beta,G_B,W;
  w_0 = PI *  parameters.frequency * ( 1.0 / SAMPLE_RATE );
  W = parameters.bandwith*PI*( 1.0 / SAMPLE_RATE );
  G =  powf( 10.0, ( parameters.gain /20.0 ) );
  G_B = sqrtf((1.0 + G*G)/2.0);
  beta = sqrtf(((G_B*G_B) - 1.0)/((G*G) - (G_B*G_B)))*tanf(W/2.0);
  a[0] = 1.0;
  a[1] = -2.0*((1.0*cos(w_0))/(1.0 + beta));
  a[2] = (1.0 - beta)/(1.0 + beta);
  b[0] = ((1.0 + G*beta)/(1.0 + beta));
  b[1] = -2.0*((cos(w_0))/(1.0+beta));
  b[2] =((1.0 - G*beta)/(1.0 + beta));

}

void iir_calc_coef_hs(FP32 *a,FP32 *b)
{
  FP32 G,w_0,beta,G_B,w_c;
  G =  powf( 10.0, ( parameters.gain /20.0 ) );
  G_B = sqrtf((1.0 + G*G)/2.0);
  w_0 = PI;
  w_c =PI * parameters.frequency * (1.0/SAMPLE_RATE);
  beta = sqrtf((G_B*G_B - 1.0)/(G*G - G_B*G_B))*tanf((PI- w_c)/2.0);
  a[0] = 1.0;
  a[1] = -2.0*((1.0*cos(w_0))/(1.0 + beta));
  a[2] = (1.0 - beta)/(1.0 + beta);
  b[0] = ((1.0 + G*beta)/(1.0 + beta));
  b[1] = -2.0*((cos(w_0))/(1.0+beta));
  b[2] =((1.0 - G*beta)/(1.0 + beta));
}

void iir_calc_coef_ls(FP32 *a,FP32 *b)
{

  FP32 G,w_0,beta,G_B,w_c;
  G =  powf( 10.0, ( parameters.gain /20.0 ) );
  G_B = sqrtf((1.0 + G*G)/2.0);
  w_c = PI*parameters.frequency *(1.0/SAMPLE_RATE);
  beta = sqrtf((G_B*G_B - 1)/(G*G - G_B*G_B))*tanf((w_c )/2.0);
  w_0 = 0;
  a[0] = 1.0;
  a[1] = -2.0*((1.0*cos(w_0))/(1.0 + beta));
  a[2] = (1.0 - beta)/(1.0 + beta);
  b[0] = ((1.0 + G*beta)/(1.0 + beta));
  b[1] = -2.0*((cos(w_0))/(1.0+beta));
  b[2] =((1.0 - G*beta)/(1.0 + beta));
}

void iir_set_param(FP32 BW,FP32 G,FP32 f_0,INT8U filter_type)
{
  parameters.filter_type = filter_type;
  parameters.bandwith = BW;
  parameters.gain= G;
  parameters.frequency = f_0;
}

void iir_get_param(FP32 *BW,FP32 *G,FP32 *f_0,INT8U *filter_type)
{
  *filter_type = parameters.filter_type;
  *BW = parameters.bandwith;
  *G = parameters.gain;
  *f_0 = parameters.frequency;
}

void iir_calc_coef(FP32 a[],FP32 b[])
{

  switch(parameters.filter_type)
  {
    case IIR_PEAK:
        iir_calc_coef_peak(a,b);
    break;
    case IIR_NOTCH:
        iir_calc_coef_peak(a,b);
    break;
    case IIR_HS:
        iir_calc_coef_hs(a,b);
    break;
    case IIR_LS:
        iir_calc_coef_ls(a,b);
    break;
  }
}



BOOLEAN iir_filter_clear()
{
  active_band_temp = 0;
  for(INT8U i=0; i < MAX_BAND; i++)
  {
    for(INT8U j=0; j < 3; j++)
    {
        A_temp[i][j]=0;
    }
  }
  return TRUE;
}

BOOLEAN iir_filter_master_gain( FP32 gain )
{
  //TODO: add master gain to current dsp profile
  master_gain = pow(10.0, (gain/20.0));
  return TRUE;
}

BOOLEAN iir_filter_add(coef_t coef)
{
  INT8U i;

  if(active_band_temp > MAX_BAND  )
  {
   return FALSE;
  }
  for(i = 0; i < 3;i++)
  {
    A_temp[active_band_temp][i] = coef.a[i];
    B_temp[active_band_temp][i] = coef.b[i];
  }
  active_band_temp++;
  return TRUE;
}
void iir_filter_use()
{

  INT8U i,j;
  iir_filter_disable();
  for(i=0; i< active_band_temp;i++)
  {
    for(j=0; j < 3; j++)
    {
      A[i][j] =  A_temp[i][j];
      B[i][j] =  B_temp[i][j];
    }

  }
  active_band = active_band_temp;
  iir_filter_enable();
}

coef_t* iir_coef(INT8U type,FP32 frequency,FP32 gain,FP32 bandwidth)
{
  iir_set_param(bandwidth, gain, frequency, type);
  iir_calc_coef(temp_coef.a,temp_coef.b);
  return &temp_coef;
}

void iir_filter_enable()
{
  filter_is_on = TRUE ;
}
void iir_filter_disable()
{
  filter_is_on = FALSE;
}
BOOLEAN filter_on()
{
  return filter_is_on ? TRUE : FALSE;
}
/****************************** End Of Module *******************************/

