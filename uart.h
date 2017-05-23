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
#include <stdint.h>
#include "emp_type.h"
#include "tm4c123gh6pm.h"
#include "scheduler.h"
#include "global.h"
#include "buffers.h"

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void uart0_init( INT32U baud_rate,
                 INT8U databits,
                 INT8U stopbits,
                 INT8U parity );
/*****************************************************************************
 *   Input    : Baudrate, databits, stopbits and parity parameters
 *   Output   : -
 *   Function : Initialize UART module
 ******************************************************************************/

void uart0_rx_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : UART receive task
 ******************************************************************************/

void uart0_tx_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : UART transmit task
 ******************************************************************************/

BOOLEAN uart_write( INT8U ch );
/*****************************************************************************
 *   Input    : Write data
 *   Output   : -
 *   Function : Write data to UART
 ******************************************************************************/

BOOLEAN uart_read( INT8U *pch );
/*****************************************************************************
 *   Input    : Pointer to data
 *   Output   : -
 *   Function : Read data from UART
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* UART_H_ */
