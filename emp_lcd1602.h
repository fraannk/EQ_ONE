/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: emp_lcd1602.h
*
* PROJECT....: EMP
*
* DESCRIPTION: EMP Lcd library
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
void lcd_direct_write_line(char *line);
void lcd_direct_write_data(INT8U byte);
void lcd_direct_set_cursor(INT8U x, INT8U y);
void lcd_direct_write_buffer(INT8U *buffer);
void lcd_buffer_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data );
void lcd_set_cursor(INT8U x, INT8U y);
void lcd_clear();
void lcd_write(char *str);
void lcd_write_char(char ch);
void lcd_set_custom_font();
void lcd_set_custom_font_eq();


/****************************** End Of Module *******************************/
#endif
