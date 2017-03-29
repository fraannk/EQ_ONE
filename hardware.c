/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: hardware.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 29. mar. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void hardware_init()
/*****************************************************************************
*   Header description
******************************************************************************/
{
  // Set GPIO'S on Run Mode Clock Gating Control Register 2 (RCGC2)
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF
                 |  SYSCTL_RCGC2_GPIOD;

  // 3 clockcycle wait after enabling the clock
  __asm__("nop");__asm__("nop");__asm__("nop");

  // GPIO Direction (GPIODR)
  GPIO_PORTF_DIR_R = 0x0E;    // 0b00001110
  // Set the direction as output (PD6).
  GPIO_PORTD_DIR_R = 0x40;    // 0b01000000

  // GPIO Digital Enable (GPIODEN)
  GPIO_PORTF_DEN_R = 0x1F;    // 0b00011111
  // Enable the GPIO pins for digital function (PD6).
  GPIO_PORTD_DEN_R = 0x40;    // 0b01000000

  // GPIO PullUp Resistor (GPIOPUR)
  GPIO_PORTF_PUR_R = 0x11;    // 0b00010001

}

/****************************** End Of Module *******************************/
