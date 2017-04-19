/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: uart.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Module for the UART driver
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 17. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef UART_H_
  #define UART_H_

/***************************** Include files *******************************/
#include "scheduler.h"

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void uart0_init( INT32U baud_rate,
                 INT8U databits,
                 INT8U stopbits,
                 INT8U parity );

void uart0_rx_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );

void uart0_tx_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );

BOOLEAN uart_write( INT8U ch );
BOOLEAN uart_read( INT8U *pch );

/****************************** End Of Module *******************************/
#endif /* UART_H_ */
