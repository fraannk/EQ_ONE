/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: flash.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 24. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"

/*****************************    Defines    *******************************/
#define FLASH_START     0x00030000

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT32U flash_begin = FLASH_START;
INT32U flash_offset = 0;


/*****************************   Functions   *******************************/

BOOLEAN flash_write( INT32U value)
{
  FLASH_FMA_R = ( flash_begin + flash_offset );
  FLASH_FMD_R = value;
  FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE;
  while( FLASH_FMC_R & FLASH_FMC_WRITE );         // Wait for WRITE bit to clear
  flash_offset++;
  return(1);
}

INT32U flash_read()
{
  return( (*((volatile INT32U *) flash_begin + flash_offset )) );
  flash_offset++;
}

BOOLEAN flash_home()
{
  flash_offset = 0;
  return(1);
}

BOOLEAN flash_seek( INT32U steps)
{
  flash_offset = flash_offset + steps;
  return(1);
}



/****************************** End Of Module *******************************/
