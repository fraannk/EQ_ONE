/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: main.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Default main module for EQ_ONE
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
#include "gfstring.h"
#include "shell.h"
#include "display.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
extern INT8U display_status_task_id;
extern INT8U display_profile_task_id;

/*****************************   Functions   *******************************/

void status( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
/*****************************************************************************
*   Input    : Task parameter block
*   Output   : -
*   Function : Status LED task. 250ms toggle os the status led.
******************************************************************************/
{
  emp_toggle_status_led();
  task_set_state( state ? 0 : 1 );
  task_wait( 250 );
}

int main( void )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Default main entry point
******************************************************************************/
{
  // Initialize hardware with SAMPLE_RATE defined in global.h
  hardware_init( SAMPLE_RATE );

  // Initialize LCD driver
  lcd_init();

  // Initialize UART driver
  uart0_init(UART_BAUDRATE, UART_DATABITS, UART_STOPBITS, UART_PARITY);

  // Default EMP setup - turn of leds
  emp_set_led(0);

  // Initialize file system
  files_init();

  // Initialize equalizer
  equalizer_init();

  // Initialize and run the scheduler with all the default tasks
  scheduler_init();
  task_start("Status", TP_LOW, status);
  task_start("UART_RX", TP_HIGH, uart0_rx_task);
  task_start("UART_TX", TP_HIGH, uart0_tx_task);
  task_start("Shell", TP_MEDIUM, shell);
  task_start("Display", TP_HIGH, display_task);

  // Display taskes used for view two changing LCD displays
  display_status_task_id = task_start("EQ-Status", TP_MEDIUM, equalizer_lcd_task);
  display_profile_task_id = task_start("EQ-profile", TP_MEDIUM, equalizer_lcd_profile_task );

  // Stop one of the two display tasks
  task_stop(display_profile_task_id);


  task_start("LCD", TP_HIGH, lcd_buffer_task);

  // start the scheduler
  scheduler();

  return(0);
}

/****************************** End Of Module *******************************/
