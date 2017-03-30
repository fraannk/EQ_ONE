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

void set_80Mhz_clock()
{
  // Enable the use of RCC2
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;     // Set USERCC2

  // Bypass PLL ans system clock divider
  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;        // Set Bypass on RCC
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;     // Set Bypass on RCC2
  SYSCTL_RCC_R &= ~SYSCTL_RCC_USESYSDIV;    // Clear USESYS

  // Set XTAL to 16Mhz
  SYSCTL_RCC_R |= SYSCTL_RCC_XTAL_16MHZ;

  // Set Oscillator Source OSCSRC
  SYSCTL_RCC2_R |= SYSCTL_RCC2_OSCSRC2_MO;  // Set MOSC

  // Clear the PWRDN
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;

  // Set system divider SYSDIV
  SYSCTL_RCC2_R |= (0x2 << 22);

  // Enable the new divider by setting USESYS
  SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;    // Set USESYS

  // Wait for PLL lock, value=1 when PLL is locked
  while( !(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) );

  // Enable PLL
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;  // Clear Bypass2

}

void init_portb()
{
  // Set GPIO'S on Run Mode Clock Gating Control Register 2 (RCGC2)
      SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

      // 3 clockcycle wait after enabling the clock
      __asm__("nop");__asm__("nop");__asm__("nop");

      // GPIO Direction (GPIOB)
      GPIO_PORTB_DIR_R = 0x0F;

      // GPIO Digital Enable (GPIODEN)
      GPIO_PORTB_DEN_R = 0x0F;
}

void init_tiva_board()
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


void hardware_init()
/*****************************************************************************
*   Header description
******************************************************************************/
{
  //set_80Mhz_clock();
  init_tiva_board();
  init_portb();
}

/****************************** End Of Module *******************************/
