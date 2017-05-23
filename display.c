/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: display.c
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Module for display driver
 *
 * Change Log:
 *****************************************************************************
 * Date    Id    Change
 * --------------------
 * 16. mar. 2017  jorn    Module created.
 *
 *****************************************************************************/

/***************************** Include files *******************************/
#include "display.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "hardware.h"
#include "emp_type.h"
#include "scheduler.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/
const INT8U bars[2][16] = { {32,32,32,32,32,32,32,32, 0, 1, 2, 3, 4, 5, 6, 7},
                            { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7} };

/*****************************   Variables   *******************************/
// Task id's used in 'diskplay_task'
INT8U   display_status_task_id = 0;
INT8U   display_profile_task_id = 0;

/*****************************   Functions   *******************************/
void eq_build_display(INT8U levels[], INT8U *buffer)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  INT8U level;

  for(INT8U i=0; i<16; i++)
  {
    level = levels[i]>>4;
    buffer[i]    = bars[0][level];
    buffer[16+i] = bars[1][level];
  }
}

void display_task ( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  switch ( state )
  {
    case 0:
      task_stop( display_status_task_id );
      task_resume( display_profile_task_id );

      task_set_state(1);
      task_wait(3000);
      break;
    case 1:
      task_stop( display_profile_task_id );
      task_resume( display_status_task_id );
      task_set_state(0);
      task_wait(3000);
      break;
  }
}
/****************************** End Of Module *******************************/
