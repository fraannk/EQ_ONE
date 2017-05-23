
/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: dsp.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: DSP calculation API module
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
typedef enum {
  iir_none    = 0,
  iir_peak    = 1,
  iir_notch   = 2,
  iir_ls      = 3,
  iir_hs      = 4
}iir_filter_type;       // Filter types defined as Enum

// Filter types
#define IIR_NONE        0
#define IIR_PEAK        1
#define IIR_NOTCH       2
#define IIR_LS          3
#define IIR_HS          4

/********************** External declaration of Variables ******************/
typedef struct coef{
  FP32 a[3];          // a coefficients of a biquad
  FP32 b[3];          // b coefficients of a biquad
}coef_t;


/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

INT16U dsp_iir_filter( INT16U sample);
/*****************************************************************************
*   Input    : Input sample
*   Output   : filtered sample
*   Function : Cascade biquads
******************************************************************************/

coef_t* iir_coef(INT8U type,FP32 frequency,FP32 gain,FP32 bandwidth);
/*****************************************************************************
*   Input    : type (filter type), frequency, gain, bandwidth
*   Output   : coefficients struct with a-,b-coefficients
*   Function : calculates coefficients from the input arguments
******************************************************************************/

BOOLEAN iir_filter_clear(void);
/*****************************************************************************
*   Input    : -
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
*   Input    : -
*   Output   : -
*   Function : transfers coefficient matrix to active coefficients
******************************************************************************/

BOOLEAN iir_filter_master_gain( FP32 gain );
/*****************************************************************************
*   Input    : gain
*   Output   : bool
*   Function : sets master gain for equalizer
******************************************************************************/

FP32 dsp_filter_amplitude(INT16U frequency);
/*****************************************************************************
*   Input    : frequency
*   Output   : amplitude
*   Function : returns the amplitude for a frequency for the active equalizer
******************************************************************************/

void dsp_filter_log_freq(INT16U* frequency_arr,INT8U size);
/*****************************************************************************
*   Input    : frequency array, number of elements in array
*   Output   : -
*   Function : calculates logarithmic divided frequency array from 20Hz to 20kHz
******************************************************************************/

#endif /* DSP_H_ */
/****************************** End Of Module *******************************/




