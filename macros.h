/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: macros.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Macro definitions
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 15. mar. 2017	jorn    Module created.
 *
 *****************************************************************************/

#ifndef MACROS_H_
#define MACROS_H_

/***************************** Include files *******************************/
#include <stdint.h>

/*****************************    Defines    *******************************/
#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

/****************************** End Of Module *******************************/
#endif /* MACROS_H_ */
