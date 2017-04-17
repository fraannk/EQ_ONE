/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: systick.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Scheduler module for the EQ_ONE system
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 16. apr. 2017  jorn    Module adopted from MOH systick
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "global.h"

/*****************************    Defines    *******************************/

// Missing definitions in tm4c123gh6pm.h file
#define NVIC_INT_CTRL_PEND_SYST   0x04000000    // Pend a systick int
#define NVIC_INT_CTRL_UNPEND_SYST 0x02000000    // Unpend a systick int
#define SYSTICK_PRIORITY    0x7E

/*****************************   Variables   *******************************/
volatile INT16S ticks = 0;

/*****************************   Functions   *******************************/
void systick_handler()
{
  // Hardware clears systick int reguest
  ticks++;
}

void enable_global_int()
{
  // enable interrupts.
  __asm("cpsie i");
}

void disable_global_int()
{
  // disable interrupts.
  __asm("cpsid i");
}

void systick_init( )
{
  INT32U systick_reload_value = ( CPU_F / 1000 ) * MS_PER_TICK;

  // Disable systick timer
  NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE);

  // Set current systick counter to reload value
  NVIC_ST_CURRENT_R = systick_reload_value;
  // Set Reload value, Systick reload register
  NVIC_ST_RELOAD_R = systick_reload_value;

  // NVIC systick setup, vector number 15
  // Clear pending systick interrupt request
  NVIC_INT_CTRL_R |= NVIC_INT_CTRL_UNPEND_SYST;

  // Set systick priority to 0x10, first clear then set.
  NVIC_SYS_PRI3_R &= ~(NVIC_SYS_PRI3_TICK_M);
  NVIC_SYS_PRI3_R |= (NVIC_SYS_PRI3_TICK_M & (SYSTICK_PRIORITY<<NVIC_SYS_PRI3_TICK_S));

  // Select systick clock source, Use core clock
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC;

  // Enable systick interrupt
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN;

  // Enable and start timer
  NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}

/****************************** End Of Module *******************************/
