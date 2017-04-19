/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: file.c
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
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "file.h"
#include "uart.h"
#include "global.h"

/*****************************    Defines    *******************************/
#define FILES_MAX  16

typedef struct
{
  BOOLEAN (*write)(INT8U);   // Pointer to device put function
  BOOLEAN (*read)(INT8U*);  // Pointer to device get function
} file_t;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
file_t files[FILES_MAX];

/*****************************   Functions   *******************************/

BOOLEAN file_write( FILE file_handel, INT8U ch )
{
  if( files[file_handel].write )
    return (files[file_handel].write( ch ));
  else
    return (0);
}

BOOLEAN file_read( FILE file_handel, INT8U *pch )
{
  if( files[file_handel].read )
    return (files[file_handel].read( pch ));
  else
    return (0);
}

INT8U files_init()
{
  INT8U i;

  for( i = 0; i < FILES_MAX; i++ )
  {
    files[i].write = NULL;
    files[i].read = NULL;
  }

  files[COM1].write = uart_write;
  files[COM1].read = uart_read;

  return( 1 );
}

/****************************** End Of Module *******************************/












