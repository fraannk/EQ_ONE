/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: shell.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 18. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "scheduler.h"
#include "file.h"
#include "uart.h"
#include "string.h"
#include "scheduler.h"
#include "global.h"
#include "equalizer.h"


/*****************************    Defines    *******************************/
typedef enum{
  STARTUP,
  IDLE,
  CMD,
  EXECUTE
}SHELL_STATE;



/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/


void shell( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
{
  static char cmd_line[127];
  static INT8U cmd_index = 0;

  if(event == TE_RESET)
  {
    task_set_state( STARTUP );
    task_clear_event();
    cmd_index = 0;
    cmd_line[0] = 0;
  }

  INT8U ch = 0;
  file_read(COM1, &ch );
  if(ch || state == EXECUTE)
  {
    switch (state)
    {
      case STARTUP:
        if(ch == 0x0D)
        {
          gfprintf(COM1, "\r\nWelcome to EQ-ONE.\r\n\r\n>>");
          task_set_state( IDLE );
        }
        break;
      case IDLE:
        if( (ch >= 0x20) && (ch <= 0x7E))     // Valid ASCII
        {
          cmd_line[cmd_index] = ch;
          cmd_index++;
          file_write(COM1, ch);
        }
        if( ch == 0x7F )                      // BackSpace
        {
          if(cmd_index > 0)
          {
            cmd_line[cmd_index] = 0;
            cmd_index--;
            file_write(COM1, ch);
          }
        }
        if( ch == 0X0D)                       // ENTER
        {
          task_set_state( EXECUTE );
        }
        break;
      case EXECUTE:
        if(cmd_line[0] == 'p' && cmd_line[1] == 's' )
          task_status(COM1);
        if(cmd_line[0] == 'e' )
          equalizer_onoff();
        cmd_line[0] = 0;
        cmd_index = 0;
        gfprintf(COM1, "\r\n>>");
        task_set_state(IDLE);
        break;
      default:
        break;
    }
  }
}


/****************************** End Of Module *******************************/
