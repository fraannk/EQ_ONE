/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: scheduler.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Scheduler module for the EQ_ONE system
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 16. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef SCHEDULER_H_
  #define SCHEDULER_H_

/***************************** Include files *******************************/
#include <stdint.h>
#include "emp_type.h"
#include "file.h"

/*****************************    Defines    *******************************/
#define         TASK_POOL_MAX         128
#define         TASK_LOW_PRIO_TICKS   10
#define         TASK_MED_PRIO_TICKS   2
#define         TASK_HIGH_PRIO_TICKS  1

typedef enum{
  TP_LOW      = 0x1,
  TP_MEDIUM   = 0x2,
  TP_HIGH     = 0x4
} TASK_PRIORITY;

typedef enum{
  TC_EMPTY    = 0x0,
  TC_STOPPED  = 0x1,
  TC_IDLE     = 0x2,
  TC_READY    = 0x4,
  TC_RUNNING  = 0x8,
  TC_WAIT     = 0x16
} TASK_CONDITION;

typedef enum{
  TE_NOEVENT  = 0x0,
  TE_TIMEOUT  = 0x1,
  TE_RESET    = 0x2
} TASK_EVENT;

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

INT8U task_start(char *name,
                 TASK_PRIORITY priority ,
                 void (*task)(INT8U, INT8U, TASK_EVENT, INT8U));

void task_wait(INT16U millisec);
void task_stop(INT8U task_id);
void task_resume(INT8U task_id);

void task_set_state(INT8U state);
void task_clear_event();

void scheduler_init();

void task_status( FILE file_handler );

void task_event( INT8U event );

void scheduler();
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Function to run the scheduler
******************************************************************************/


/****************************** End Of Module *******************************/
#endif /* SCHEDULER_H_ */
