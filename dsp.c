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
#define IIR_DENOM_LENGTH    2
#define IIR_NUM_LENGTH      3

/*********** math defines ***********/

#define PI      3.14159265f
#define SQRT_2  1.4142135623f


/************* filter types **************/
#define IIR_PEAK        1
#define IIR_NOTCH       2
#define IIR_LS          3
#define IIR_HS          4


#define MAX_BAND      10



/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

typedef struct params{
 FP32 bandwith;
 FP32 frequency;
 FP32 gain;
 INT8U filter_type;
}param_t;

FP32 A[MAX_BAND][3];
FP32 B[MAX_BAND][3];
FP32 W[MAX_BAND][3];
FP32 A_temp[MAX_BAND][3];
FP32 B_temp[MAX_BAND][3];
param_t parameters;
coef_t  temp_coef;

BOOLEAN filter_is_on;

INT8U active_band = 0;
INT8U active_band_temp=0;

/*****************************   Functions   *******************************/
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

FP32 iir_filter_cascade(FP32 in)            /* input sample */
{
    INT8U i;
    FP32 out;

    out = in;
    if(filter_on())
    {
      for(i=0; i< active_band;i++)
          out = iir_filter_sos(out,A[i],B[i], W[i]);
    }
    return out;
}

void iir_calc_coef_peak(FP32 *a,FP32 *b)
{

  FP32 G,w_0,beta,G_B,W;
  w_0 = 2.0* PI *  parameters.frequency * ( 1.0 / SAMPLE_RATE );
  W = parameters.bandwith*2.0*PI*( 1.0 / SAMPLE_RATE );
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
  w_c =2.0*PI * parameters.frequency * (1.0/SAMPLE_RATE);
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
   w_c = 2.0 * PI*parameters.frequency *(1.0/SAMPLE_RATE);
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


void iir_demo_eq()
{/*
  FP32 Q[] =      {   5,      5,          5,        5,        5       };
  FP32 f_0[] =    {   500,    1500,       2500,     5000,     8000    };
  FP32  G[] =     {   -3,     5,          -5,       3,        10      };
  INT8U types[] = {   IIR_HS, IIR_PEAK, IIR_NOTCH,  IIR_PEAK, IIR_LS  };
  bands = 5;
  iir_init_dsp_states();
 // fill parameters struct
  for(INT8U i=0; i < bands; i++)
  {
      iir_set_param(Q[i],G[i],f_0[i],types[i],parameters[i]);

  }

  // calc coeff A,B from param_arr
  for(INT8U i=0; i < bands; i++)
  {
      iir_calc_coef(A[i],B[i],parameters[i]);
  }

  for(INT8U i=0; i < BUFF_SIZE;i++)
    out[i] = iir_filter_cascade(bands, A, B, W, in[i]);*/

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

