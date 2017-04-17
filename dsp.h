
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

/*****************************    Defines    *******************************/
#define BUFF_SIZE 100



/********************** External declaration of Variables ******************/
FP32 a[BUFF_SIZE]; // denominator coefficients
FP32 b[BUFF_SIZE]; // numerator coefficients
FP32 in[BUFF_SIZE]; // input buffer
FP32 out[BUFF_SIZE]; // output buffer


/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
void iir_init_dsp_states(void);

void iir_set_coefficients(FP32 *a, FP32 *b);

FP32 iir_filter_sos(FP32 in,FP32 *a,FP32 *b,FP32 *states_sos);
/*****************************************************************************
*   Input    : Audio signal
*   Output   : filtered audio signal
*   Function : filters signal
******************************************************************************/

FP32 iir_filter_cascade(INT8U K_in, FP32** A, FP32** B, FP32** W, FP32 in);

#endif /* DSP_H_ */
/****************************** End Of Module *******************************/

