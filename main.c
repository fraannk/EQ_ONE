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

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void i_am_alive( INT8U my_id, INT8U my_state, TASK_EVENT event, INT8U data )
{
  emp_toggle_status_led();
  task_set_state( my_state ? 0 : 1 );
  task_wait( 250 );
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

  // Initialize equalizer
  equalizer_init();

  // Initialize and run the scheduler
  scheduler_init();
  task_start(TP_LOW, i_am_alive);
  scheduler();

  return(0);
}

/****************************** End Of Module *******************************/
