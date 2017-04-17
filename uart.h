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

/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

void uart0_init( INT32U baud_rate,
                 INT8U databits,
                 INT8U stopbits,
                 INT8U parity );
/****************************** End Of Module *******************************/
#endif /* UART_H_ */
