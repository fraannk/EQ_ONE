/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: global.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Global definition file
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 3. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef GLOBAL_H_
  #define GLOBAL_H_

/***************************** Include files *******************************/

/*****************************    Defines    *******************************/
#define CPU_F                 80000000
#define MS_PER_TICK           1             // Millisec per Ticks
#define CPU_MULTIPLEX         80            // CPU cycles per microsec


#define SAMPLE_RATE           44100

#define UART_BAUDRATE         19200
#define UART_DATABITS         8
#define UART_STOPBITS         1
#define UART_PARITY           'n'

// Files
#define   COM1                0         // UART


#define   VERSION_MAJOR       0
#define   VERSION_MINOR       1


/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

/****************************** End Of Module *******************************/
#endif /* GLOBAL_H_ */
