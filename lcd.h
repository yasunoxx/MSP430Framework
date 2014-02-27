/*
 * lcd.h -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

/* lcd.c */
extern void InitLCD( void );
extern void SubLCD( void );
void ScreenScene( void );

#define MSB   0
#define LSB   1

#define LCD_RS    BIT4
#define	LCD_RW    BIT5
#define LCD_E     BIT3
#define LCD_CMD   0
#define LCD_DATA  1
#define	LCDC_DIR   P1DIR
#define	LCDC_OUT   P1OUT
#define	LCDD_DIR   P2DIR
#define	LCDD_OUT   P2OUT
