
/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: dsp.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION:
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
*30. mar. 2017 Dennis    Module created.
*
*****************************************************************************/

#ifndef DSP_H_
#define DSP_H_

/***************************** Include files *******************************/
#include "emp_type.h"

/*****************************    Defines    *******************************/
#define BUFF_SIZE     100
#define K_MAX         8
#define BAND_MAX      10

/********************** External declaration of Variables ******************/

FP32 in[BUFF_SIZE]; // input buffer
FP32 out[BUFF_SIZE]; // output buffer
FP32  A[K_MAX][3],
      B[K_MAX][3],
      W[K_MAX][3];


typedef struct {
FP32 param_arr[3];
INT8U filter_type;
}param;

param parameters[K_MAX];

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void iir_init_dsp_states(void);



FP32 iir_filter_sos(FP32 in,FP32 *a,FP32 *b,FP32 *states_sos);
/*****************************************************************************
*   Input    : Audio signal
*   Output   : filtered audio signal
*   Function : filters signal
******************************************************************************/

FP32 iir_filter_cascade(INT8U K_in, FP32 A[K_MAX][3], FP32 B[K_MAX][3], FP32 W[K_MAX][3], FP32 in);


void iir_calc_coef(FP32 *a,FP32 *b,param param);
void iir_calc_coef_peak(FP32 *a,FP32 *b,FP32 *param);
void iir_calc_coef_hs(FP32 *a,FP32 *b,FP32 *param);
void iir_calc_coef_ls(FP32 *a,FP32 *b,FP32 *param);


void iir_set_param(FP32 Q,FP32 G,FP32 f_0,INT8U filter_type,param param);
void iir_get_param(FP32 *Q,FP32 *G,FP32 *f_0,INT8U *filter_type,param param);




void iir_demo_eq();

#endif /* DSP_H_ */
/****************************** End Of Module *******************************/




