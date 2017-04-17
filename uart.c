/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: uart.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Module for the UART driver
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 17. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "emp_type.h"
#include "tm4c123gh6pm.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

INT32U lcrh_databits( INT8U antal_databits )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 5 and 6 according to the wanted number of data bits.
*           5: bit5 = 0, bit6 = 0.
*           6: bit5 = 1, bit6 = 0.
*           7: bit5 = 0, bit6 = 1.
*           8: bit5 = 1, bit6 = 1  (default).
*          all other bits are returned = 0
******************************************************************************/
{
  if(( antal_databits < 5 ) || ( antal_databits > 8 ))
  antal_databits = 8;
  return(( (INT32U)antal_databits - 5 ) << 5 );  // Control bit 5-6, WLEN
}

INT32U lcrh_stopbits( INT8U antal_stopbits )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 3 according to the wanted number of stop bits.
*           1 stpobit:  bit3 = 0 (default).
*           2 stopbits: bit3 = 1.
*          all other bits are returned = 0
******************************************************************************/
{
  if( antal_stopbits == 2 )
    return( 0x00000008 );     // return bit 3 = 1
  else
  return( 0x00000000 );   // return all zeros
}

INT32U lcrh_parity( INT8U parity )
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : sets bit 1, 2 and 7 to the wanted parity.
*           'e':  00000110b.
*           'o':  00000010b.
*           '0':  10000110b.
*           '1':  10000010b.
*           'n':  00000000b.
*          all other bits are returned = 0
******************************************************************************/
{
  INT32U result;

  switch( parity )
  {
    case 'e':
      result = 0x00000006;
      break;
    case 'o':
      result = 0x00000002;
      break;
    case '0':
      result = 0x00000086;
      break;
    case '1':
      result = 0x00000082;
      break;
    case 'n':
    default:
      result = 0x00000000;
  }
  return( result );
}

void uart0_fifos_enable()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Enable the tx and rx fifos
******************************************************************************/
{
  UART0_LCRH_R  |= 0x00000010;
}

void uart0_fifos_disable()
/*****************************************************************************
*   Input    :
*   Output   :
*   Function : Enable the tx and rx fifos
******************************************************************************/
{
  UART0_LCRH_R  &= 0xFFFFFFEF;
}

void uart0_init( INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity )
{
  INT32U BRD;

  // Initialize UART0

  // Enable clock for port A
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;

  // Enable clock for UART0
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;

  // Setup Port A pins for UART
  // TODO: Fixs this
  GPIO_PORTA_AFSEL_R |= 0x00000003;
  GPIO_PORTA_DIR_R   |= 0x00000002;
  GPIO_PORTA_DEN_R   |= 0x00000003;
  GPIO_PORTA_PUR_R   |= 0x00000002;

  // Calculate UART baudrate
  // TODO: Documents this
  BRD = 64000000 / baud_rate;
  UART0_IBRD_R = BRD / 64;
  UART0_FBRD_R = BRD & 0x0000003F;

  // Setup UART
  // TODO: Fix this
  UART0_LCRH_R  = lcrh_databits( databits );
  UART0_LCRH_R += lcrh_stopbits( stopbits );
  UART0_LCRH_R += lcrh_parity( parity );

  uart0_fifos_disable();

  UART0_CTL_R  |= (UART_CTL_UARTEN | UART_CTL_TXE );  // Enable UART
}

/****************************** End Of Module *******************************/