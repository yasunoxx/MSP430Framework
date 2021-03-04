/*
 * lcd.c -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

#ifdef GCC_VERSION_463
#include <legacymsp430.h>
#else
#include <msp430g2553.h>
#endif
#include <string.h>
#include <stdio.h>
#include "io.h"
#include "lcd.h"
#include "tlv.h"

#define TRUE  1
#define FALSE 0

struct _b2n
{
  unsigned char data;
  unsigned char nibbles[ 2 ];
} b2n;

char BufLCD[ 2 ][ 16 ];
const char mesLCD[ 2 ][ 16 ] = {
// 0123456789ABCDEF
  "ProtoR02        ",
  "                "
};
const char *stringTemp = "Temp:   ";

extern const char *FW_Version;	// in main.c
extern volatile unsigned short LcdWait;
extern unsigned short ScreenWait;
extern unsigned char ScreenScenario;

volatile unsigned char subState_LCD;
int tempSMA[ 32 ];
unsigned char tempSMApos, tempSMAcount;


void InitLCD( void );
void SubLCD( void );
void lcd_send4( unsigned char command, unsigned char mode );
void lcd_cmd4( unsigned char command );
void lcd_cmd8( unsigned char command );
void lcd_data( unsigned char data );
void byte2nibbles( void );


/*-----------------------------------------------------------------------------------*/
/*!
@brief	initialize periferal for drive LCD
@param	void
@retval	void
@author	yasunoxx
@date	2014.02.20
*/
/*-----------------------------------------------------------------------------------*/
void InitLCD( void )
{
  unsigned char loop;

  LCDC_DIR |= LCD_RS + LCD_RW + LCD_E;
  LCDD_DIR |= 0x0F;

  for( loop = 0; loop < 16; loop++ )
  {
    BufLCD[ 0 ][ loop ] = mesLCD[ 0 ][ loop ];
    BufLCD[ 1 ][ loop ] = mesLCD[ 1 ][ loop ];
  }
  BufLCD[ 1 ][ 4 ] = FW_Version[ 0 ];
  BufLCD[ 1 ][ 5 ] = FW_Version[ 1 ];
  BufLCD[ 1 ][ 6 ] = FW_Version[ 2 ];
  BufLCD[ 1 ][ 7 ] = FW_Version[ 3 ];

  subState_LCD = 0;

  for( loop = 0; loop <= 31; loop++ )
  {
    tempSMA[ loop ] = 0;
  }
  tempSMApos = 0;
  tempSMAcount = 0;
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	LCD drive sub state machine
@param	void
@retval	void
@author	yasunoxx
@date	2009.10.9
*/
/*-----------------------------------------------------------------------------------*/
void SubLCD( void )
{
  unsigned char loop;

  switch( subState_LCD )
  {
    case 0:
      // init state 1
      lcd_cmd8( 0x30 );   // set 8bit mode
      subState_LCD = 1;
      break;
    case 1:
      // init state 2
      lcd_cmd8( 0x30 );   // repeat
      subState_LCD = 2;
      break;
    case 2:
      // init state 3
      lcd_cmd8( 0x30 );
      subState_LCD = 3;
      break;
    case 3:
      // init state 4
      lcd_cmd8( 0x20 );   // set 4bit mode
      subState_LCD = 4;
      break;
    case 4:
      // init state 5
      lcd_cmd4( 0x28 );   // set 4bit mode, 2 rows, 5*7 dots
      lcd_cmd4( 0x08 );   // hidden display
      lcd_cmd4( 0x0C );   // display, no cursor
      lcd_cmd4( 0x06 );   // write increment

      lcd_cmd4( 0x02 );   // cursor home
      subState_LCD = 5;
      break;
    case 5:
      // regular state 1
      for( loop = 0; loop <= 4; loop++ ) lcd_data( BufLCD[ 0 ][ loop ] );
      subState_LCD = 6;
      break;
    case 6:
      // regular state 2
      for( loop = 5; loop <= 10; loop++ ) lcd_data( BufLCD[ 0 ][ loop ] );
      subState_LCD = 7;
      break;
    case 7:
      // regular state 3
      for( loop = 11; loop <= 15; loop++ ) lcd_data( BufLCD[ 0 ][ loop ] );
      subState_LCD = 8;
      break;
    case 8:
      // regular state 4
      lcd_cmd4( 0x0C0 );   // cursor second row home
      subState_LCD = 9;
      break;
    case 9:
      // regular state 5
      for( loop = 0; loop <= 4; loop++ ) lcd_data( BufLCD[ 1 ][ loop ] );
      subState_LCD = 10;
      break;
    case 10:
      // regular state 6
      for( loop = 5; loop <= 10; loop++ ) lcd_data( BufLCD[ 1 ][ loop ] );
      subState_LCD = 11;
      break;
    case 11:
      // regular state 7
      for( loop = 11; loop <= 15; loop++ ) lcd_data( BufLCD[ 1 ][ loop ] );
      subState_LCD = 12;
      break;
    case 12:
      // regular state 8
      lcd_cmd4( 0x80 );   // cursor second row home
      subState_LCD = 5;
      break;
    default:
      // to init state 1
      subState_LCD = 0;
      break;
  }
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	Send data to LCD(4bit×2) for MSP430
@param[in] unsigned char command：LCD command value
@param[in] unsigned char mode：command value(LCD_CMD), or char.data(LCD_DATA)
@retval	void
@author	yasunoxx
@date	2009.10.9/2014.02.20
*/
/*-----------------------------------------------------------------------------------*/
void lcd_send4( unsigned char command, unsigned char mode )
{
  unsigned char temp;

  LCDC_OUT &= ~LCD_RW; // H->L

  // 4bit command send
  temp = LCDD_OUT & 0x0F0;
  LCDD_OUT = temp | ( command >> 4 );
  if( mode == LCD_DATA ) LCDC_OUT |= LCD_RS;  // L->H
  LCDC_OUT |= LCD_E; // L->H
  LCDC_OUT &= ~LCD_E; // H->L

  temp = LCDD_OUT & 0x0F0;
  LCDD_OUT = temp | ( command & 0x0F );
  if( mode == LCD_DATA ) LCDC_OUT |= LCD_RS;  // L->H
  LCDC_OUT |= LCD_E; // L->H
  LCDC_OUT &= ~LCD_E; // H->L
  if( mode == LCD_DATA ) LCDC_OUT &= ~LCD_RS;  // H->L

  LCDC_OUT |= LCD_RW; // L->H

  __disable_interrupt();
  LcdWait = 0;
  __enable_interrupt();
  while( 1 )
  {
    if( LcdWait >= 2 ) break;
  }
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	Send command to LCD(4bit) for MSP430
@param[in] unsigned char command：LCD command value
@retval	void
@author	yasunoxx
@date	2009.10.9
*/
/*-----------------------------------------------------------------------------------*/
void lcd_cmd4( unsigned char command )
{
  lcd_send4( command, LCD_CMD );
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	Send char.dara to LCD(4bit) for MSP430
@param[in] unsigned char command：LCD char.data value
@retval	void
@author	yasunoxx
@date	2009.10.9
*/
/*-----------------------------------------------------------------------------------*/
void lcd_data( unsigned char data )
{
  lcd_send4( data, LCD_DATA );
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	Send command to LCD(8bit, but 4bit MSB) for MSP430
@param[in] unsigned char command：LCD command value
@retval	void
@author	yasunoxx
@date	2009.10.9/2014.02.20
*/
/*-----------------------------------------------------------------------------------*/
void lcd_cmd8( unsigned char command )
{
  unsigned char temp;

  LCDC_OUT &= ~LCD_RW; // H->L

  // 8bit command send
  // (send MSB nibble)
  temp = LCDD_OUT & 0x0F0;
  LCDD_OUT = temp | ( command >> 4 );
  LCDC_OUT &= ~LCD_RS;  // H->L
  LCDC_OUT |= LCD_E; // L->H
  LCDC_OUT &= ~LCD_E; // H->L

  LCDC_OUT |= LCD_RW; // L->H

  __disable_interrupt();
  LcdWait = 0;
  __enable_interrupt();
  while( 1 )
  {
    if( LcdWait >= 2 ) break;
  }
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	8bit split to 4bit×2(like atoi())
@param	void
@retval	void
@author	yasunoxx
@date	2009.10.9
*/
/*-----------------------------------------------------------------------------------*/
void byte2nibbles( void )
{
  b2n.nibbles[ MSB ] = b2n.data >> 4;
  b2n.nibbles[ MSB ] &= 0x0F;
  if( b2n.nibbles[ MSB ] < 10 )
  {
    b2n.nibbles[ MSB ] += '0';
  }
  else
  {
    b2n.nibbles[ MSB ] += 'A' - 10;
  }

  b2n.nibbles[ LSB ] = b2n.data & 0x0F;
  if( b2n.nibbles[ LSB ] < 10 )
  {
    b2n.nibbles[ LSB ] += '0';
  }
  else
  {
    b2n.nibbles[ LSB ] += 'A' - 10;
  }
}

/*-----------------------------------------------------------------------------------*/
/*!
@brief	Temp. Meter Application(LCD automated state machine sample)
@param	void
@retval	void
@author	yasunoxx
@date	2014.02.20
*/
/*-----------------------------------------------------------------------------------*/
void ScreenScene( void )
{
  unsigned char loop; // , work3, work4;
  unsigned short tempreg;
  long int work;

  switch( ScreenScenario )
  {
    case 0:
      if( ScreenWait >= 200 )
      {
        ScreenWait = 0;
        ScreenScenario = 1;
      }
      break;
    case 1:
      // "Temp:XXX"
      strncpy( BufLCD[ 0 ], stringTemp, 8 );
      for( loop = 0; loop <= 7; loop++ )
      {
        BufLCD[ 1 ][ loop ] = ' ';
      }
      tempreg = SampleAndConversionAdcTemp();
      b2n.data = tempreg >> 8;
      byte2nibbles();
      BufLCD[ 0 ][ 5 ] = b2n.nibbles[ LSB ];
      b2n.data = tempreg & 0x0FF;
      byte2nibbles();
      BufLCD[ 0 ][ 6 ] = b2n.nibbles[ MSB ];
      BufLCD[ 0 ][ 7 ] = b2n.nibbles[ LSB ];

      work = ( ( ( 85 - 30 ) * 100 ) / ( Var_CAL_ADC_25T85 - Var_CAL_ADC_25T30 ) );
      tempSMA[ ( tempSMApos & 0x1F ) ] = ( ( tempreg - Var_CAL_ADC_25T30 ) * work + 3000 );
      tempSMApos++;
      tempSMAcount++;
      if( tempSMAcount > 31 ) tempSMAcount = 31;
      work = 0;
      for( loop = 0; loop <= tempSMAcount; loop++ )
      {
        work += tempSMA[ loop ];
      }
      work = work / ( tempSMAcount + 1 );

      sprintf( BufLCD[ 1 ], "%6d", ( int )work ); // Care NULL termination
      BufLCD[ 1 ][ 1 ] = BufLCD[ 1 ][ 2 ];
      BufLCD[ 1 ][ 2 ] = BufLCD[ 1 ][ 3 ];
      BufLCD[ 1 ][ 3 ] = '.';
      BufLCD[ 1 ][ 6 ] = 0x0DF;
      BufLCD[ 1 ][ 7 ] = 'C';
      ScreenWait = 0;
      ScreenScenario = 0;
      break;
    case 2:
      // SUSPEND: conversion too fast?
      if( ( ADC10CTL1 & ( 0x0FFFF - ADC10BUSY ) ) == 0 )
      {
        ScreenScenario = 1;
      }
    default:
      ScreenScenario = 0;
 }
}
