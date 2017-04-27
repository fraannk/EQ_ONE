
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
/*****************************************************************************
*   Input    : none
*   Output   : none
*   Function : initialize dsp module
******************************************************************************/


FP32 iir_filter_sos(FP32 in,FP32 *a,FP32 *b,FP32 *states_sos);
/*****************************************************************************
*   Input    : Input sample
*   Output   : filtered sample
*   Function : filters input sample with a biquad filter
******************************************************************************/

FP32 iir_filter_cascade(FP32 in);
/*****************************************************************************
*   Input    : Input sample
*   Output   : filtered sample
*   Function : Cascade biquads
******************************************************************************/


void iir_calc_coef(FP32 *a,FP32 *b);
/*****************************************************************************
*   Input    : a coefficient, b coefficient (call by reference)
*   Output   : none
*   Function : calculate biquad coefficients for parameter filter_type
******************************************************************************/

void iir_calc_coef_peak(FP32 *a,FP32 *b);
/*****************************************************************************
*   Input    : a coefficient, b coefficient (call by reference)
*   Output   : none
*   Function : calculate peak/notch coefficients
******************************************************************************/

void iir_calc_coef_hs(FP32 *a,FP32 *b);
/*****************************************************************************
*   Input    : a coefficient, b coefficient (call by reference)
*   Output   : none
*   Function : calculate high shelf coefficients
******************************************************************************/

void iir_calc_coef_ls(FP32 *a,FP32 *b);
/*****************************************************************************
*   Input    : a coefficient, b coefficient (call by reference)
*   Output   : none
*   Function : calculate low shelf coefficients
******************************************************************************/



BOOLEAN iir_filter_clear(void);
/*****************************************************************************
*   Input    : none
*   Output   : true if clear succeeded
*   Function : clears temp coefficient matrix,
******************************************************************************/

BOOLEAN iir_filter_add(coef_t coef);
/*****************************************************************************
*   Input    : coefficients
*   Output   : true if add succeeded
*   Function : add coefficients to temp coefficient matrix
******************************************************************************/

void iir_filter_use();
/*****************************************************************************
*   Input    : none
*   Output   : none
*   Function : transfers coefficient matrix to active coefficients
******************************************************************************/

coef_t* iir_coef(INT8U type,FP32 frequency,FP32 gain,FP32 bandwidth);
/*****************************************************************************
*   Input    : type (filter type), frequency, gain, bandwidth
*   Output   : coefficients struct with a-,b-coefficients
*   Function : calculates coefficients from the input arguments
******************************************************************************/

void iir_set_param(FP32 BW,FP32 G,FP32 f_0,INT8U filter_type);
/*****************************************************************************
*   Input    : BW (bandwidth), G (gain), f_0 (center/corner frequency), filter_type
*   Output   : none
*   Function : set parameters
******************************************************************************/

void iir_get_param(FP32 *BW,FP32 *G,FP32 *f_0,INT8U *filter_type);
/*****************************************************************************
*   Input    : BW (bandwidth), G (gain), f_0 (center/corner frequency), filter_type
*   Output   : none
*   Function : returns input arguments as call by reference
******************************************************************************/

void iir_filter_enable();
/*****************************************************************************
*   Input    : none
*   Output   : none
*   Function : enable the filter in iir_cascade()
******************************************************************************/

void iir_filter_disable();
/*****************************************************************************
*   Input    : none
*   Output   : none
*   Function : disnable the filter in iir_cascade()
******************************************************************************/

BOOLEAN filter_on();
/*****************************************************************************
*   Input    : none
*   Output   : returns true if filter is on
*   Function : checks if filter (iir_cascade() ) is on
******************************************************************************/

#endif /* DSP_H_ */
/****************************** End Of Module *******************************/




