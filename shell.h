/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: shell.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Shell module
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
#include "emp_type.h"
#include "scheduler.h"

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void shell( INT8U my_id, INT8U my_state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : Task that runs the shell
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* SHELL_H_ */
