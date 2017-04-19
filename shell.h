/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: shell.h
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
* 18. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef SHELL_H_
  #define SHELL_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void shell( INT8U my_id, INT8U my_state, TASK_EVENT event, INT8U data );

/****************************** End Of Module *******************************/
#endif /* SHELL_H_ */
