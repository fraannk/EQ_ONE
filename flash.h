/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: flash.h
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 24. apr. 2017	jorn    Module created.
*
*****************************************************************************/

#ifndef FLASH_H_
  #define FLASH_H_

/***************************** Include files *******************************/
#include "emp_type.h"
/*****************************    Defines    *******************************/

/********************** External declaration of Variables ******************/

/*****************************   Constants   *******************************/

/*************************  Function interfaces ****************************/
BOOLEAN flash_write( INT32U value);
INT32U flash_read();
BOOLEAN flash_home();
BOOLEAN flash_seek( INT32U steps);


/****************************** End Of Module *******************************/
#endif /* FLASH_H_ */
