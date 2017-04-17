/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: buffers.h
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
* 17. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef BUFFERS_H_
  #define BUFFERS_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define     BUFFERS_MAX         16
/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

INT8S buffer_create(INT16U size);
INT8U buffer_get(INT8U id, INT8U *data);
INT8U buffer_put(INT8U id, INT8U data);
INT8U buffer_is_empty(INT8U id);

/****************************** End Of Module *******************************/
#endif /* BUFFERS_H_ */
