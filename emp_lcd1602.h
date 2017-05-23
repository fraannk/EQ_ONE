/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: emp_lcd1602.c
 *
 * PROJECT....: EMP
 *
 * DESCRIPTION: LCD driver module
 *
 * Change Log:
 ******************************************************************************
 * Date    Id    Change
 * YYMMDD
 * --------------------
 * 170210  JJA   Module created.
 *
 *****************************************************************************/

#ifndef EMP_LCD1602_
#define EMP_LCD1602_

/***************************** Include files *******************************/
#include "emp_type.h"
#include "scheduler.h"

/*****************************    Defines    *******************************/
#define     LCD_CHARS               16
#define     LCD_LINES               2
#define     LCD_BUF_SIZE            LCD_CHARS*LCD_LINES

/*****************************   Constants   *******************************/

/*****************************   Functions   *******************************/
void lcd_init();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Initializes the LCD module
 ******************************************************************************/

void lcd_set_cursor(INT8U x, INT8U y);
/*****************************************************************************
 *   Input    : X, Y coordinates
 *   Output   : -
 *   Function : Set cursor on LCD (Buffered)
 ******************************************************************************/

void lcd_clear();
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Clears the LCD (Buffered)
 ******************************************************************************/

void lcd_write(char *str);
/*****************************************************************************
 *   Input    : line containing pointer to char
 *   Output   : -
 *   Function : Write a line (string) directly to LCD
 ******************************************************************************/

void lcd_write_char(char ch);
/*****************************************************************************
 *   Input    : Char to be written
 *   Output   : -
 *   Function : Write char to LCD (buffered)
 ******************************************************************************/

void lcd_buffer_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
/*****************************************************************************
 *   Input    : Task parameter block
 *   Output   : -
 *   Function : LCD taskto update the LCD buffer to display, 1 char per tick
 ******************************************************************************/


/****************************** End Of Module *******************************/
#endif
