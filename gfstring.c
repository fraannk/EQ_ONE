/*
 * string.c
 *
 *  Created on: 26/12/2011
 *      Author: Morten
 */

#include <stdarg.h>
#include "emp_type.h"
#include "global.h"
#include "file.h"

#define NEGATIVE 0
#define POSITIVE 1
#define LEFT    0
#define RIGHT   1



void putc1( file, ch )
int file;
unsigned char ch;
{
  file_write( file, ch );
  //put_queue( file, ch, WAIT_FOREVER );
}


void putStr( fp, str )
FILE fp;
char *str;
{
  while( *str )
  {
        putc1( fp, *str++ );
  }
}

void putChars( fp, Str, Len )
FILE fp;
char *Str;
int Len;
{
  while( Len-- )
    putc1( fp, *Str++ );
}

void putDec( fp, Val, Sign, Size, Filler )
FILE fp;
long Val;
int  Sign;
int  Size;
char Filler;
{
  long Weight = 1;
  long Digit;
  int  i;

  if( Sign == NEGATIVE )
  {
    Size--;
    if( Filler == '0' )
    {
      putc1( fp, '-' );
      Sign = POSITIVE;
    }
  }

  // Weight = 10**(Size-1)
  if( Size == 0 )
  {
    while( Weight < Val )
    {
      Weight *= 10;
      Size++;
    }
    if( Size == 0)
      Size = 1;
    else
      Weight /= 10;
  }
  else
  {
    i = Size - 1;
    while( i-- > 0 )
      Weight *= 10;
  }

  while( Size > 0 )
  {
    Digit = Val / Weight;
    if( Digit == 0 )
      putc1( fp, Filler );
    else
    {
      if( Sign == NEGATIVE )
      {
        putc1( fp, '-' );
        Sign = POSITIVE;
      }
      putc1( fp, Digit + '0' );
      Filler = '0';
    }
    Val %= Weight;
    Weight /= 10;
    Size--;
  }
}

void putHex( fp, Val, Size )
FILE fp;
long Val;
int  Size;
{
  unsigned long Weight = 1;
  long Digit;
  int  i;

  if( Size == 0 )
    Size = sizeof( long ) * 2;

  i = Size - 1;
  while( i-- > 0 )
    Weight *= 16;

  //putStr( fp, "0x");
  while( Size > 0 )
  {
    Digit = Val / Weight;
    if( Digit < 10 )
      putc1( fp, Digit + '0' );
    else
      putc1( fp, Digit + '7' );
    Val %= Weight;
    Weight /= 16;
    Size--;
  }
}

void gfprintf( FILE fp, const char *Str, ...)
{
  unsigned long Val;
  int i, Done, Size, Len, Sign, Adjust;
  char *subStr, preChar;
  va_list vaArgP;

  va_start(vaArgP, Str);

  while( *Str )
  {
    // Find the first non-% character, or the end of the string.
    i = 0;
    while(( Str[i] != '%' ) && ( Str[i] != '\0'))
      i++;

    putChars( fp, Str, i );

    // Skip the portion of the string that was written.
    Str += i;

    // See if the next character is a %.
    if( *Str == '%')
    {
      Size   = 0;
      Done   = FALSE;
      preChar = ' ';
      i = 0;

      while( !Done )
      {
        Str++;
        switch( *Str )
        {
          case '%':
            putc1( fp, '%' );
            Done = TRUE;
            break;

          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            if(( *Str == '0' ) && ( i == 0 ))
              preChar = '0';

            Size *= 10;
            Size += *Str - '0';
            i++;
            break;

          case '-':
            if( i == 0 )
              Adjust = LEFT;
            i++;
            break;

          case 'c':
            Val = va_arg(vaArgP, unsigned long);
            putc1( fp, (char)Val );
            Done = TRUE;
            break;

          case 'd':
            Val  = va_arg(vaArgP, unsigned long);
            if( (long)Val < 0 )
            {
              Val = -(long)Val;
              Sign = NEGATIVE;
            }
            else
              Sign = POSITIVE;
            putDec( fp, (long)Val, Sign, Size, preChar );
            Done = TRUE;
            break;

          case 'u':
            Val  = va_arg(vaArgP, unsigned long);
            putDec( fp, Val, POSITIVE, Size, preChar );
            Done = TRUE;
            break;

          case 'x':
          case 'X':
          case 'p':
            Val = va_arg(vaArgP, unsigned long);
            putHex( fp, Val, Size );
            Done = TRUE;
            break;

          case 's':
            subStr = va_arg(vaArgP, char *);
            Len = 0;
            while( subStr[Len] )
              Len++;
            if( Adjust == RIGHT )
              while( Size-- > Len )
                putc1( fp, ' ' );
            putStr( fp, subStr );
            if( Adjust == RIGHT )
              while( Size-- > Len )
                putc1( fp, ' ' );
            Done = TRUE;
            break;
          default:
            putStr( fp, "???" );
            Done = TRUE;
        }
      }
      Str++;
    }
  }
  // End the varargs processing.
  va_end(vaArgP);
}

void gprintf( const char *Str, ...)
{
  va_list vaArgP;

  va_start(vaArgP, Str);
  gfprintf( COM1, Str, vaArgP );     // stdout = COM1
  va_end(vaArgP);
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
