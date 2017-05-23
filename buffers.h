/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: buffers.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Module to handle dynamic FIFO buffers
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 17. apr. 2017	jorn    Module created.
 *
 *****************************************************************************/

#ifndef BUFFERS_H_
#define BUFFERS_H_

/***************************** Include files *******************************/
#include "emp_type.h"

/*****************************    Defines    *******************************/
#define     BUFFERS_MAX         16
/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

INT8S buffer_create(INT16U size);
/*****************************************************************************
 *   Input    : size of INT8U
 *   Output   : Handle is to buffer
 *   Function : Creates a buffer with size
 ******************************************************************************/

INT8U buffer_get(INT8U id, INT8U *data);
/*****************************************************************************
 *   Input    : Buffer id and pointer to data
 *   Output   : True if success
 *   Function : Get value into data from buffer with id
 ******************************************************************************/

INT8U buffer_put(INT8U id, INT8U data);
/*****************************************************************************
 *   Input    : Buffer id and data
 *   Output   : True if sucess
 *   Function : Put value data in buffer
 ******************************************************************************/

INT8U buffer_is_empty(INT8U id);
/*****************************************************************************
 *   Input    : Valid buffer id
 *   Output   : Returns 1 if buffer is empty and 0 if not
 *   Function : Check if a buffer with id is empty
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* BUFFERS_H_ */
