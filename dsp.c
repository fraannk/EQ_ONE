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
#ifndef PI
  #define PI 3.14159265f
#endif

#define SQRT_2 1.4142135623f


/************* filter types **************/
#define IIR_PEAK        1
#define IIR_NOTCH       2
#define IIR_LS          3
#define IIR_HS          4


#define N_SAMPLES 1000
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U   bands;



/*****************************   Functions   *******************************/
void iir_init_dsp_states()
{
  for(INT8U i=0; i < K_MAX; i++)
  {
    for(INT8U j=0; j < 3; j++)
    {
      W[i][j] = 0;
      A[i][j] = 0;
      B[i][j] = 0;
    }
  }

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

FP32 iir_filter_cascade(INT8U band,         /* number of bands*/
                        FP32 A[K_MAX][3],   /* A coefficient matrix */
                        FP32 B[K_MAX][3],   /* B coefficient matrix*/
                        FP32 W[K_MAX][3],   /* State matrix*/
                        FP32 in)            /* input sample */
{
    INT8U i;
    FP32 out;

    out = in;

    for(i = 0; i < band; i++)
        out = iir_filter_sos(out,A[i],B[i],W[i]);

    return out;
}

void iir_calc_coef_peak(FP32 *a,FP32 *b,FP32 *param)
{

  FP32 G,w_0,beta,W;

  G =  powf( 10.0, ( param[1]/20.0 ) );
  w_0 = 2.0* PI * param[2] * ( 1.0 / SAMPLE_RATE );
  W = tanf( w_0/param[0] );
  beta = sqrtf(G) + W;
  a[0] = 1.0;
  a[1] = - (2.0* sqrtf(G)*cosf( w_0 ) )/beta;
  a[2] = ( sqrtf(G) - W )/beta;
  b[0] = ( sqrtf(G) + G*W ) / beta;
  b[1] = - (2.0*sqrtf(G)*cosf( w_0) )/beta;
  b[2] = (sqrtf(G) - G*W)/beta;

}
void iir_calc_coef_hs(FP32 *a,FP32 *b,FP32 *param)
{
  FP32 G,w_0,beta,W;
  G =  powf( 10.0, ( param[1]/20.0 ) );
  w_0 = 2.0* PI * param[2] * ( 1.0 / SAMPLE_RATE );
  W = tanf( w_0/param[0] );
  beta = sqrtf(G)*W*W + SQRT_2*W*powf( G, 0.25) + 1.0;
  a[0] = 1.0;
  a[1] =  (2.0 *( sqrtf(G)*W*W - 1.0) )/beta;
  a[2] = ( sqrtf(G)*W*W - SQRT_2*W*powf(G,0.25) + 1.0 )/beta;
  b[0] = ( sqrtf(G)*( sqrtf(G) + SQRT_2*W*powf( G, 0.25) + W*W) ) / beta;
  b[1] = - (sqrtf(G)*2.0*(sqrtf(G) - W*W ))/beta;
  b[2] =( sqrtf(G)*(sqrtf(G) - SQRT_2*W*powf(G,0.25) + W*W))/beta;
}

void iir_calc_coef_ls(FP32 *a,FP32 *b,FP32 *param)
{

  FP32 G,w_0,beta,W;
  G =  powf( 10.0, ( param[1]/20.0 ) );
  w_0 = 2* PI * param[2] * ( 1 / SAMPLE_RATE );
  W = tanf( w_0/param[0] );
  beta = sqrtf(G) + SQRT_2*W*powf(G, 0.25) + W*W;
  a[0] = 1.0;
  a[1] = ( 2.0*(W*W - sqrtf(G) ) ) /beta;
  a[2] = ( sqrtf(G) -SQRT_2*W*powf(G,0.25) + W*W  ) /beta;
  b[0] = (sqrtf(G)*( sqrtf(G)*W*W +SQRT_2*W*powf(G,0.25) + 1.0  )) /beta;
  b[1] = (  sqrtf(G)*2.0*(sqrtf(G)*W*W - 1.0) ) /beta;
  b[2] = (sqrtf(G)*( sqrtf(G)*W*W - SQRT_2*W*powf(G,0.25) + 1.0  )) /beta;
}

void iir_set_param(FP32 Q,FP32 G,FP32 f_0,INT8U filter_type,param param)
{
  param.filter_type = filter_type;
  param.param_arr[0] = Q;
  param.param_arr[1] = G;
  param.param_arr[2] = f_0;
}

void iir_get_param(FP32 *Q,FP32 *G,FP32 *f_0,INT8U *filter_type, param param)
{
  *filter_type = param.filter_type;
  *Q = param.param_arr[0];
  *G = param.param_arr[1];
  *f_0 = param.param_arr[2];
}

void iir_calc_coef(FP32 *a,FP32 *b,param param)
{

    switch(param.filter_type)
    {
        case IIR_PEAK:
            iir_calc_coef_peak(a,b,param.param_arr);
        break;
        case IIR_NOTCH:
            iir_calc_coef_peak(a,b,param.param_arr);
        break;
        case IIR_HS:
            iir_calc_coef_hs(a,b,param.param_arr);
        break;
        case IIR_LS:
            iir_calc_coef_ls(a,b,param.param_arr);
        break;
    }
}


void iir_demo_eq()
{
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
    out[i] = iir_filter_cascade(bands, A, B, W, in[i]);

}
/****************************** End Of Module *******************************/

