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
#include "scheduler.h"
#include <stdint.h>
#include "emp_type.h"
#include "debug.h"
#include "hardware.h"
#include "file.h"
#include "gfstring.h"
#include "global.h"
#include "systick.h"

/*****************************    Defines    *******************************/
// Task Control Block Data structure (TCB)
typedef struct
{
  TASK_CONDITION  condition;
  INT8U           id;
  const char      *name;
  INT8U           state;
  TASK_EVENT      event;
  INT8U           sem;
  TASK_PRIORITY   priority;
  INT16U          timer;
  INT8U           ticks;
  void            (*task)(INT8U, INT8U, TASK_EVENT, INT8U);
  INT32U          runtime_min;
  INT32U          runtime_max;
} tcb;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern INT16S ticks;              // sysTick

tcb *current_task;                // Pointer to current task
tcb task_pool[TASK_POOL_MAX];     // Task pool

/*****************************   Functions   *******************************/
INT8U task_new_id(void)
/*****************************************************************************
 *   Input    : -
 *   Output   : Task id
 *   Function : Get the next free task id
 ******************************************************************************/
{
  static INT8U next_id = 0;
  return( next_id++ );
}

INT8U task_get_priority_ticks(TASK_PRIORITY priority )
/*****************************************************************************
 *   Input    : Priority enum
 *   Output   : sysTick factor
 *   Function : Get the priority sysTick factor
 ******************************************************************************/
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

INT8U task_start(char *name,
                 TASK_PRIORITY priority ,
                 void (*task)(INT8U, INT8U, TASK_EVENT, INT8U))
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  INT8U id = task_new_id();
  task_pool[id].condition = TC_IDLE;
  task_pool[id].priority = priority;
  task_pool[id].name = name;
  task_pool[id].task = task;
  task_pool[id].ticks = task_get_priority_ticks(priority);
  task_pool[id].sem = 0;
  task_pool[id].state = 0;
  task_pool[id].timer = 0;
  task_pool[id].event = TE_RESET;
  task_pool[id].runtime_min = ~0;
  task_pool[id].runtime_max = 0;

  return( id );
}

void task_stop(INT8U task_id)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  if( task_id < TASK_POOL_MAX )
    task_pool[task_id].condition = TC_STOPPED;
}

void task_resume(INT8U task_id)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  if( task_id < TASK_POOL_MAX )
    task_pool[task_id].condition = TC_IDLE;
}

void scheduler_init()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  for( INT8U i = 0; i < TASK_POOL_MAX; i++ )
  {
    task_pool[i].condition = TC_EMPTY;
    task_pool[i].event = TE_NOEVENT;
    task_pool[i].name = "\n";
    task_pool[i].id = i;
    task_pool[i].sem = 0;
    task_pool[i].state = 0;
    task_pool[i].timer = 0;
    task_pool[i].ticks = 0;
    task_pool[i].priority = TP_LOW;
    task_pool[i].runtime_min = ~0;
    task_pool[i].runtime_max = 0;
  }
}

void task_wait(INT16U millisec)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  current_task->timer = millisec;
  current_task->condition = TC_WAIT;
}

void task_set_state(INT8U state)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  current_task->state = state;
}

void task_event( INT8U event )
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  current_task->event = event;
}

void task_clear_event()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  current_task->event = TE_NOEVENT;
}


void task_status( FILE file_handler )
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  gfprintf(file_handler, "\r\nProcess status\r\n\r\n");

  gfprintf(file_handler, "\x1B[1m");
  gfprintf(file_handler, "ID Max     Min     Condition Priority State Name\r\n");
  gfprintf(file_handler, "\x1B[0m");

  for(INT8U i = 0 ; i < TASK_POOL_MAX; i++)
  {
    if(task_pool[i].condition != TC_EMPTY)
    {

      gfprintf(file_handler, "%02d %7d %7d ",
               task_pool[i].id,
               task_pool[i].runtime_max,
               task_pool[i].runtime_min);
      switch (task_pool[i].condition)
      {
        case TC_IDLE:
          gfprintf(file_handler, "IDLE      ");
          break;
        case TC_READY:
          gfprintf(file_handler, "READY     ");
          break;
        case TC_RUNNING:
          gfprintf(file_handler, "RUNNING   ");
          break;
        case TC_STOPPED:
          gfprintf(file_handler, "STOPPED   ");
          break;
        case TC_WAIT:
          gfprintf(file_handler, "WAIT      ");
          break;
        default:
          break;
      }
      switch (task_pool[i].priority)
      {
        case TP_LOW:
          gfprintf(file_handler, "\x1B[32mLOW      \x1B[0m");
          break;
        case TP_MEDIUM:
          gfprintf(file_handler, "\x1B[33mMEDIUM   \x1B[0m");
          break;
        case TP_HIGH:
          gfprintf(file_handler, "\x1B[31mHIGH     \x1B[0m");
          break;
      }
      gfprintf(file_handler, "%5d ", task_pool[i].state);
      gfprintf(file_handler, "%s", task_pool[i].name);
      gfprintf(file_handler, "\r\n");
    }
  }
  gfprintf(file_handler, "\r\n");
}

void scheduler()
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  while(1)
  {
    while(ticks)
    {
      debug_pins_high(DEBUG_P3);
      ticks--;
      current_task = task_pool ;
      while( current_task->condition != TC_EMPTY )
      {
        if( current_task->condition != TC_STOPPED)
        {
          // Handle TC_WAIT condition
          if( current_task->condition == TC_WAIT )
          {
            if( current_task->timer )
              current_task->timer--;
            else
            {
              current_task->event = TE_TIMEOUT ;
              current_task->condition = TC_READY;
              current_task->ticks =
                  task_get_priority_ticks((*current_task).priority);
            }
          }
          else
          {
            if( current_task->ticks )
              current_task->ticks--;
            else
            {
              current_task->condition = TC_READY;
              current_task->ticks =
                  task_get_priority_ticks(current_task->priority);
            }
          }

          if( current_task->condition == TC_READY )
          {
            current_task->condition = TC_RUNNING;
            systick_touch();
            current_task->task( current_task->id,
                                current_task->state,
                                current_task->event,
                                0);
            INT32U runtime =  systick_touch();
            if ( current_task->runtime_min > runtime )
              current_task->runtime_min = runtime;
            if ( current_task->runtime_max < runtime )
              current_task->runtime_max = runtime;
            if( current_task->condition == TC_RUNNING )
              current_task->condition = TC_IDLE;
          }
        }
        current_task++;
      }
      debug_pins_low(DEBUG_P3);
    }
  }
}

/****************************** End Of Module *******************************/
