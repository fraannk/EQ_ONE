/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: emp_board.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 170210  JJA   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"


/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/
void emp_set_led(INT8U led)
/*****************************************************************************
*   Header description
******************************************************************************/
{
  // Set bit pattern bit2-bit0 on for led_color on PF1-PF3
  if( led &  0x01 )
    GPIO_PORTF_DATA_R &= ~0x08;
  else
    GPIO_PORTF_DATA_R |= 0x08;

  if( led &  0x02 )
    GPIO_PORTF_DATA_R &= ~0x04;
  else
    GPIO_PORTF_DATA_R |= 0x04;

  if( led &  0x04 )
    GPIO_PORTF_DATA_R &= ~0x02;
  else
    GPIO_PORTF_DATA_R |= 0x02;
}

void emp_toggle_led(INT8U led)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  // Set bit pattern bit2-bit0 on for led_color on PF1-PF3
  if( led &  0x01 )
    GPIO_PORTF_DATA_R ^= 0x08;

  if( led &  0x02 )
    GPIO_PORTF_DATA_R ^= 0x04;

  if( led &  0x04 )
    GPIO_PORTF_DATA_R ^= 0x02;
}

void emp_toggle_status_led()
/*****************************************************************************
*   Header description
******************************************************************************/
{
  GPIO_PORTD_DATA_R ^= 0x40;
}

void emp_clear_all_led(void)
/*****************************************************************************
*   Header description
******************************************************************************/
{
  emp_set_led(0);
}
/****************************** End Of Module *******************************/












