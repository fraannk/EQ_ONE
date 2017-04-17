/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: scheduler.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Scheduler module for the EQ_ONE system
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 16. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "scheduler.h"
#include "emp_type.h"
#include "debug.h"

/*****************************    Defines    *******************************/
typedef struct
{
  TASK_CONDITION  condition;
  INT8U           id;
  INT8U           name[16];
  INT8U           state;
  TASK_EVENT      event;
  INT8U           sem;
  TASK_PRIORITY   priority;
  INT16U          timer;
  INT8U           ticks;
  void            (*task)(INT8U, INT8U, TASK_EVENT, INT8U);
} tcb;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern INT16S ticks;
tcb *current_task;
tcb task_pool[TASK_POOL_MAX];

/*****************************   Functions   *******************************/
INT8U task_new_id(void)
{
  static INT8U next_id = 0;
  return( next_id++ );
}

INT8U task_get_priority_ticks(TASK_PRIORITY priority )
{
  INT8U ticks;
  switch( priority )
  {
    case TP_LOW:    ticks = TASK_LOW_PRIO_TICKS; break;
    case TP_MEDIUM: ticks = TASK_MED_PRIO_TICKS; break;
    case TP_HIGH:   ticks = TASK_HIGH_PRIO_TICKS; break;
    default:   ticks = TASK_LOW_PRIO_TICKS;
  }

  return( ticks );
}

void task_start(TASK_PRIORITY priority ,
                void (*task)(INT8U, INT8U, TASK_EVENT, INT8U))
{
  INT8U id = task_new_id();
  task_pool[id].condition = TC_IDLE;
  task_pool[id].priority = priority;
  task_pool[id].task = task;
  task_pool[id].ticks = task_get_priority_ticks(priority);
  task_pool[id].sem = 0;
  task_pool[id].state = 0;
  task_pool[id].timer = 0;
}

void scheduler_init()
{
  for( INT8U i = 0; i < TASK_POOL_MAX; i++ )
  {
    task_pool[i].condition = TC_STOPPED;
    task_pool[i].event = TE_NOEVENT;
    task_pool[i].id = i;
    task_pool[i].sem = 0;
    task_pool[i].state = 0;
    task_pool[i].timer = 0;
    task_pool[i].ticks = 0;
    task_pool[i].priority = TP_LOW;
  }
}

void task_wait(INT16U millisec)
{
  (*current_task).timer = millisec;
  (*current_task).condition = TC_WAIT;
}

void task_set_state(INT8U state)
{
  (*current_task).state = state;
}

void scheduler()
{
  while(1)
  {
    while(ticks)
    {
      debug_pins_high(DEBUG_P3);
      ticks=0;
      current_task = task_pool ;
      while( (*current_task).condition != TC_STOPPED )
      {
        // Handle TC_WAIT condition
        if( (*current_task).condition == TC_WAIT )
        {
          if( (*current_task).timer )
            (*current_task).timer--;
          else
          {
            (*current_task).event = TE_TIMEOUT ;
            (*current_task).condition = TC_READY;
            (*current_task).ticks =
                task_get_priority_ticks((*current_task).priority);
          }
        }
        else
        {
          if( (*current_task).ticks )
            (*current_task).ticks--;
          else
          {
            (*current_task).condition = TC_READY;
            (*current_task).ticks =
                task_get_priority_ticks((*current_task).priority);
          }
        }

        if( (*current_task).condition == TC_READY )
        {
          (*current_task).condition = TC_RUNNING;
          (*current_task).task( (*current_task).id,
                                (*current_task).state,
                                (*current_task).event,
                                0);
          if( (*current_task).condition == TC_RUNNING )
            (*current_task).condition = TC_IDLE;
        }
        current_task++;
      }
      debug_pins_low(DEBUG_P3);
    }
  }
}

/****************************** End Of Module *******************************/
