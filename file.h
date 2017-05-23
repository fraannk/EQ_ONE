/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: file.h
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: File module, adopted from MOH file.c
 *
 * Change Log:
 *****************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 170418  Jorn Jacobi  Module created/adopted.
 *
 *****************************************************************************/

#ifndef FILE_H_
#define FILE_H_

/***************************** Include files *******************************/
#include "emp_type.h"

/*****************************    Defines    *******************************/
typedef int FILE;

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
BOOLEAN file_write( FILE, INT8U );
/*****************************************************************************
 *   Input    : File id and data to write
 *   Output   : Return value from file implementation
 *   Function : Write data to implemented write function pointer
 ******************************************************************************/

INT8U files_init();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Initialize the file module
 ******************************************************************************/

BOOLEAN file_read( FILE file, INT8U *pch );
/*****************************************************************************
 *   Input    : File id and pointer to read data
 *   Output   : Return data from the implemented read function pointer
 ******************************************************************************/

/****************************** End Of Module *******************************/
#endif /* FILE_H_ */
