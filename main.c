/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: main.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 29. mar. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "hardware.h"
#include "emp_type.h"
#include "emp_board.h"
#include "scheduler.h"
#include "debug.h"
#include "emp_lcd1602.h"
#include "global.h"
#include "uart.h"
#include "equalizer.h"
#include "file.h"
#include "string.h"
#include "shell.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void status( INT8U my_id, INT8U my_state, TASK_EVENT event, INT8U data )
{
  emp_toggle_status_led();
  task_set_state( my_state ? 0 : 1 );
  task_wait( 250 );
}

void echo( INT8U my_id, INT8U my_state, TASK_EVENT event, INT8U data )
{
  INT8U ch=0;
  if(file_read(COM1, &ch))
  {
    task_status(COM1);
  }
}

int main( void )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Default main entry point
******************************************************************************/
{
  // Initialize hardware
  hardware_init( SAMPLE_RATE );

  // Initialize drivers
  lcd_init();
  uart0_init(UART_BAUDRATE, UART_DATABITS, UART_STOPBITS, UART_PARITY);

  // Default EMP setup
  emp_set_led(0);

  // Initialize filesystem
  files_init();

  // Initialize equalizer
  equalizer_init();

  // Initialize and run the scheduler
  scheduler_init();
  task_start("Status", TP_LOW, status);
  task_start("Echo", TP_HIGH, echo);
  task_start("UART_RX", TP_HIGH, uart0_rx_task);
  task_start("UART_TX", TP_HIGH, uart0_tx_task);
  task_start("Shell", TP_MEDIUM, shell);
  scheduler();

  return(0);
}

/****************************** End Of Module *******************************/
