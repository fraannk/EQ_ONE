/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: emp_lcd1602.c
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
#include "emp_lcd1602.h"
#include <stdint.h>
#include <stdlib.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "hardware.h"
#include "macros.h"
#include "string.h"
#include "scheduler.h"



/*****************************    Defines    *******************************/
#define     LCD_DATA_PORT           GPIO_PORTC_DATA_R
#define     LCD_DATA_DDR            GPIO_PORTC_DIR_R
#define     LCD_DATA_EN             GPIO_PORTC_DEN_R
#define     LCD_D4_PIN              4
#define     LCD_D5_PIN              5
#define     LCD_D6_PIN              6
#define     LCD_D7_PIN              7

#define     LCD_CTRL_PORT           GPIO_PORTD_DATA_R
#define     LCD_CTRL_DDR            GPIO_PORTD_DIR_R
#define     LCD_CTRL_EN             GPIO_PORTD_DEN_R
#define     LCD_RS_PIN              2
#define     LCD_E_PIN               3

/*****************************   Constants   *******************************/
typedef enum {HIGH, LOW} pin_state;

/*****************************   Variables   *******************************/
INT8U lcd_display_buffer[LCD_BUF_SIZE];
INT8U buffer_x = 0;
INT8U buffer_y = 0;

/*****************************   Functions   *******************************/

void lcd_RS(pin_state pin)
{
  if(pin == LOW)
    bit_clear(LCD_CTRL_PORT, 1<<LCD_RS_PIN);
  else
    bit_set(LCD_CTRL_PORT, 1<<LCD_RS_PIN);
}

void lcd_E(pin_state pin)
{
  if(pin == LOW)
    bit_clear(LCD_CTRL_PORT, 1<<LCD_E_PIN);
  else
    bit_set(LCD_CTRL_PORT, 1<<LCD_E_PIN);
}

void lcd_write_nibble(INT8U byte)
{
  lcd_E(LOW);

  // clear all data bits
  LCD_DATA_PORT &= ~( 1<<LCD_D7_PIN );
  LCD_DATA_PORT &= ~( 1<<LCD_D6_PIN );
  LCD_DATA_PORT &= ~( 1<<LCD_D5_PIN );
  LCD_DATA_PORT &= ~( 1<<LCD_D4_PIN );

  // put Upper Nipple of byte on the data port
  LCD_DATA_PORT |= (byte & 1<<7 ) ? (1<<LCD_D7_PIN) : 0;
  LCD_DATA_PORT |= (byte & 1<<6 ) ? (1<<LCD_D6_PIN) : 0;
  LCD_DATA_PORT |= (byte & 1<<5 ) ? (1<<LCD_D5_PIN) : 0;
  LCD_DATA_PORT |= (byte & 1<<4 ) ? (1<<LCD_D4_PIN) : 0;
  delay_us(1);

  // Set E low
  lcd_E(HIGH);
  delay_us(1);
  // Set E high
  lcd_E(LOW);
  delay_us(1);
}

void lcd_write_instruction(INT8U byte)
{
  lcd_RS(LOW);
  lcd_E(LOW);
  lcd_write_nibble(byte);
  lcd_write_nibble(byte<<4);
  delay_us(41);
}

void lcd_write_data(INT8U byte)
{
  lcd_RS(HIGH);
  lcd_E(LOW);
  lcd_write_nibble(byte);
  lcd_write_nibble(byte<<4);
  delay_us(41);
}

void lcd_write_data_nodelay(INT8U byte)
{
  lcd_RS(HIGH);
  lcd_E(LOW);
  lcd_write_nibble(byte);
  lcd_write_nibble(byte<<4);
}

void lcd_set_cursor(INT8U x, INT8U y)
{
  INT8U ddram_adress = 0x80;
  ddram_adress |= (y*0x40 + x);
  lcd_write_instruction(ddram_adress);
}

void lcd_write_line(char *line)
{
  INT8U i = 0;
  while( line[i] != 0)
  {
    lcd_write_data(line[i++]);
  }
}

void lcd_clear(void)
{
  // Clear entire display
  lcd_write_instruction(0x01);
  delay_us(1520);
}

void lcd_set_custom_font()
{
  lcd_write_instruction(0x40);
  lcd_write_data(0b10000);
  lcd_write_data(0b11000);
  lcd_write_data(0b11100);
  lcd_write_data(0b11110);
  lcd_write_data(0b11100);
  lcd_write_data(0b11000);
  lcd_write_data(0b10000);
  lcd_write_data(0b00000);


  //lcd_write_instruction(0x41);
  lcd_write_data(0b00001);
  lcd_write_data(0b00011);
  lcd_write_data(0b00111);
  lcd_write_data(0b01111);
  lcd_write_data(0b00111);
  lcd_write_data(0b00011);
  lcd_write_data(0b00001);
  lcd_write_data(0b00000);
}

void lcd_set_custom_font_eq()
{
  // set start font at CGRAM addr. 0x0
  lcd_write_instruction(0x40);

  // font 0x0
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  // font 0x1
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x2
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x3
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x4
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x5
  lcd_write_data(0b00000);
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x6
  lcd_write_data(0b00000);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  // font 0x7
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
  lcd_write_data(0b11111);
}

void lcd_buffer_set_cursor(INT8U x, INT8U y)
{
  if(  y < LCD_LINES && x <= LCD_CHARS )
  {
    buffer_x = x;
    buffer_y = y;
  }
}

void lcd_buffer_clear()
{
  for(INT8U i = 0; i < LCD_BUF_SIZE; i++ )
    lcd_display_buffer[i] = 0x20;         // Clear with space
}

void lcd_buffer_write(char *str)
{
  INT8U offset = ( buffer_y << 4 ) + buffer_x;
  INT8U (* buffer_ptr)[LCD_BUF_SIZE] = NULL;
  buffer_ptr = (INT8U (*)[LCD_BUF_SIZE])lcd_display_buffer;

  if( sizeof(*str) < (LCD_BUF_SIZE - offset ) )
    memcpy( &lcd_display_buffer[offset] , str, strlen(str));
}

void lcd_write_buffer(INT8U *buffer)
{
  for(INT8U y=0; y < LCD_LINES; y++)
  {
    lcd_set_cursor(0, y);
    for(INT8U x=0 ; x < LCD_CHARS ; x++)
    {
      lcd_write_data( buffer[x+(16*y)] );
    }
  }
}

void lcd_buffer_task( INT8U id, INT8U state, TASK_EVENT event, INT8U data )
{
  if(state < LCD_BUF_SIZE)
  {
    if(state == 0)
      lcd_set_cursor(0,0);
    if(state == 16)
      lcd_set_cursor(0,1);

    lcd_write_data_nodelay( lcd_display_buffer[state] );
    task_set_state(++state);
  }
  else
    task_set_state(0);
}

void lcd_init()
{
  // Set GPIOF on Run Mode Clock Gating Control Register PORTC 6 PORTD
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3;

  // Init GPIO pins for LCD display
  LCD_DATA_DDR |= (1<<LCD_D4_PIN) | (1<<LCD_D5_PIN) | (1<<LCD_D6_PIN) |
      (1<<LCD_D7_PIN);
  LCD_DATA_EN  |= (1<<LCD_D4_PIN) | (1<<LCD_D5_PIN) | (1<<LCD_D6_PIN) |
      (1<<LCD_D7_PIN);

  LCD_CTRL_DDR |= (1<<LCD_E_PIN) | (1<<LCD_RS_PIN);
  LCD_CTRL_EN  |= (1<<LCD_E_PIN) | (1<<LCD_RS_PIN);

  // Begin LCD INIT
  // wait 40ms
  delay_us(40000);
  lcd_RS(LOW);
  lcd_E(LOW);

  // Write function set
  lcd_write_nibble(0x30);

  // Wait 4.1ms
  delay_us(4100);

  // Write function set
  lcd_write_nibble(0x30);

  // Wait 100us
  delay_us(100);

  // Write function set
  lcd_write_nibble(0x30);

  // Wait 100us
  delay_us(100);

  // Write 4bit function set
  lcd_write_nibble(0x20);
  delay_us(37);

  // Functions set 0 0 1 DL N F - -
  // DL : 0: 4 bit     1: 8 bit       Datalines
  // N  : 0: 1 line    1: 2 lines     Display lines
  // F  : 0: 5x8       1: 5x10        Fontsize
  lcd_write_instruction(0b00101000);

  // Display  0 0 0 0 1 D C B
  // D  :                             Display on/off
  // C  :                             Cursor on/off
  // B  :                             Blinking on/off
  lcd_write_instruction(0b00001000);


  // Clear entire display
  lcd_write_instruction(0x01);
  delay_us(1520);

  // Entry mode set 0 0 0 0 0 1 I/D S
  // I/D  0: Dec          1: Inc
  // S    0: Noshift
  lcd_write_instruction(0b00000110);

  // LCD ON
  lcd_write_instruction(0b00001100);

  // Load custom font to LCD CGRAM
  lcd_set_custom_font_eq();

  // set cursor to 0,0
  lcd_set_cursor(0,0);


}
/****************************** End Of Module *******************************/












