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
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "dsp.h"

/*****************************    Defines    *******************************/
#define IIR_DENOM_LENGTH    2
#define IIR_NUM_LENGTH      3


// test values
#define A1  -1.9028f
#define A2  0.982f
#define B0  1.0067f
#define B1  -1.9028f
#define B2  0.9761f

#define K 8         // Max Band
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
FP32  A[K][3], B[K][3], W[K][3];



/*****************************   Functions   *******************************/
void iir_init_dsp_states()
{

    INT8U i,j;
    for(i = 0; i <K; i++)
    {
        iir_set_coefficients(A[i],B[i]);
        for(j = 0; j < 3; j++)
            W[i][j]=0;
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

FP32 iir_filter_cascade(INT8U K_in,     /* number of bands*/
                        FP32** A,       /* A coefficient matrix */
                        FP32** B,       /* B coefficient matrix*/
                        FP32** W,       /* State matrix*/
                        FP32 in)        /* input sample */
{
    INT8U i;
    FP32 out;

    out = in;

    for(i = 0; i < K_in; i++)
        out = iir_filter_sos(out,A[i],B[i],W[i]);

    return out;
}


void iir_set_coefficients(FP32 *a, FP32 *b)
{
    a[0] = 1;
    a[1] = A1;
    a[2] = A2;
    b[0] = B0;
    b[1] = B1;
    b[2] = B2;
}
/****************************** End Of Module *******************************/

