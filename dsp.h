
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


/********************** External declaration of Variables ******************/


typedef struct coef{
  FP32 a[3];
  FP32 b[3];
}coef_t;




/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void iir_init_dsp_states(void);



FP32 iir_filter_sos(FP32 in,FP32 *a,FP32 *b,FP32 *states_sos);
/*****************************************************************************
*   Input    : Audio signal
*   Output   : filtered audio signal
*   Function : filters signal
******************************************************************************/

FP32 iir_filter_cascade(FP32 in);


void iir_calc_coef(FP32 *a,FP32 *b);
void iir_calc_coef_peak(FP32 *a,FP32 *b);
void iir_calc_coef_hs(FP32 *a,FP32 *b);
void iir_calc_coef_ls(FP32 *a,FP32 *b);



BOOLEAN iir_filter_clear(void);
BOOLEAN iir_filter_add(coef_t coef);
void iir_filter_use();

coef_t* iir_coef(INT8U type,FP32 frequency,FP32 gain,FP32 bandwidth);
void iir_set_param(FP32 Q,FP32 G,FP32 f_0,INT8U filter_type);
void iir_get_param(FP32 *Q,FP32 *G,FP32 *f_0,INT8U *filter_type);
void iir_filter_enable();
void iir_filter_disable();
BOOLEAN filter_on();



void iir_demo_eq();

#endif /* DSP_H_ */
/****************************** End Of Module *******************************/




