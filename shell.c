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
#include "shell.h"
#include <stdint.h>
#include <stdlib.h>
#include "scheduler.h"
#include "file.h"
#include "uart.h"
#include "scheduler.h"
#include "global.h"
#include "equalizer.h"
#include "gfstring.h"
#include <string.h>
#include "display.h"


/*****************************    Defines    *******************************/
typedef enum{
  STARTUP,
  IDLE,
  CMD,
  EXECUTE
}SHELL_STATE;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern INT8U display_status_task_id;
extern INT8U display_profile_task_id;

/*****************************   Functions   *******************************/
char *get_cmd(char *cmd_line)
{
  INT8U index = 0;
  const char *seperator = "\x20";

  char *cmd = NULL;

  index = strcspn( cmd_line, seperator);        // Search for first space
  if(index)
  {
    cmd = malloc(index+1);
    strncpy( cmd, cmd_line, index);
    cmd[index] = 0x00;
  }

  return(cmd);
}

BOOLEAN cmd_compare(char *cmd, char *str)
{
  return( strcmp( cmd, str) == 0 ? 1 : 0 );
}


void shell( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
{
  static char cmd_line[127];
  static INT8U cmd_index = 0;

  char *cmd;

  if(event == TE_RESET)
  {
    task_set_state( STARTUP );
    task_clear_event();
    cmd_index = 0;
    cmd_line[0] = 0;
  }
  else
  {
    INT8U ch = 0;
    file_read(COM1, &ch );
    if(ch || state == EXECUTE)
    {
      switch (state)
      {
        case STARTUP:
          if(ch == 0x0D)
          {
            gfprintf(COM1, "\x1B[H\x1B[2J\r\nWelcome to EQ-ONE - version %0d.%0d\r\n",
                     VERSION_MAJOR, VERSION_MINOR);
            gfprintf(COM1, "---------------------------------------------\r\n");
            gfprintf(COM1, "Embedded digital graphics equalizer module\r\n\r\n");
            gfprintf(COM1, "Use \x1B[1mhelp\x1B[0m for list of commands\r\n\r\n");
            gfprintf(COM1, ">>");
            task_set_state( IDLE );
          }
          break;
        case IDLE:
          if( (ch >= 0x20) && (ch <= 0x7E))     // Valid ASCII
          {
            cmd_line[cmd_index] = ch;
            cmd_line[cmd_index+1] = 0;
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
          // cmd line syntax : cmd -arg:val -arg:val
          cmd = get_cmd(cmd_line);

          if( cmd_compare(cmd, "exit") )
          {
            task_event(TE_RESET);
            gfprintf(COM1, "\r\n   ... Bye\r\n");
          }
          else
          {
            if(cmd_index != 0)
            {
              if( cmd_compare(cmd, "ps")  )
                task_status(COM1);
              else if( cmd_compare( cmd, "t0") )
              {
                task_stop( display_status_task_id );
                task_resume( display_profile_task_id );
              }
              else if( cmd_compare( cmd, "t1") )
              {
                task_stop( display_profile_task_id );
                task_resume( display_status_task_id );
              }
              else if( cmd_compare(cmd, "eq")  )
                equalizer_onoff();
              else if( cmd_compare(cmd, "p0") )
                profile_use(0);
              else if( cmd_compare(cmd, "p1") )
                profile_use(1);
              else if( cmd_compare(cmd, "p2") )
                profile_use(2);
              else if( cmd_compare(cmd, "p3") )
                profile_use(3);
              else if( cmd_compare(cmd, "p4") )
                profile_use(4);
              else if( cmd_compare(cmd, "p5") )
                profile_use(5);
              else if( cmd_compare(cmd, "p6") )
                profile_use(6);
              else if( cmd_compare(cmd, "p7") )
                profile_use(7);
              else if( cmd_compare(cmd, "p8") )
                profile_use(8);
              else if( cmd_compare(cmd, "p9") )
                profile_use(9);
              else if( cmd_compare(cmd, "p10") )
                profile_use(10);
              else if( cmd_compare(cmd, "help") )
              {
                gfprintf(COM1, "\r\nCommands : exit, eq, ps");
              }
              else
                gfprintf(COM1, "\r\nUnknown command : %s\r\n", cmd_line);
            }
              task_set_state(IDLE);
              cmd_line[0] = 0;
              cmd_index = 0;
              gfprintf(COM1, "\r\n>>");
          }
          break;
        default:
          break;
      }
    }
  }
}


/****************************** End Of Module *******************************/
