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
// *** Hardware ***
// Use this #define to enable the EMP board
// ------------------------------------------------------------
//#define EMP                 // EMP do use on EMP board
//#define PWM                 // Only use this on Project board!!

#define CPU_F                 80000000      // Current MCU clock speed
#define MS_PER_TICK           1             // Millisec per Ticks
#define CPU_MULTIPLEX         80            // CPU cycles per microsec

// Audio
#define SAMPLE_RATE           44100         // Samplerate Fs used

// UART
#define UART_BAUDRATE         19200         // UART baudrate
#define UART_DATABITS         8             // UART databits
#define UART_STOPBITS         1             // UART stopbits
#define UART_PARITY           'n'           // UART no-parity

// Files
#define   COM1                0             // UART file to use

// EQ-One
#define   VERSION_MAJOR       0             // EQ-ONE Major version
#define   VERSION_MINOR       9             // EQ-ONE Minor version

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/

/****************************** End Of Module *******************************/
#endif /* GLOBAL_H_ */
