/*
 * main.c -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

/* target type are define in Makefile */

// Original Copyright:
/*
 * main.c
 *
 * MSP-EXP430G2-LaunchPad User Experience Application
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Framework strategy
 *   1. System timer 10[msec], not WDT
 *   2. peripheral initialize
 *   3. main loop
 *   4. signal handling
 */

#include <legacymsp430.h>
#include "io.h"
#include "lcd.h"
#include "tlv.h"

const char *FW_Version = "0X01";

#ifdef TARGET_XT1
#define TIMER_PWM_PERIOD 22
// 21.97 = 60 * 12000[Hz] / 32768[Hz]
#define TIMER_PWM_CENTER 11
// 10.98 = 30 * 12000[Hz] / 32768[Hz]
#else
#define	TIMER_PWM_PERIOD 60
#define TIMER_PWM_CENTER 30
#endif
#define TIMER_PWM_PERIOD_DCO 60000
#define TIMER_PWM_CENTER_DCO 30000

void ConfigureTimerPwm( void );
void PreApplicationMode( void );
void InitializeClocks( void );
void initializeDCO( void );
void InitializeLeds( void );

#define SYSTIMER_COUNTUP	200
volatile unsigned int SysTimer_Counter;
volatile unsigned short LcdWait;
#define Low 0
#define High 1
volatile unsigned char ClockMode;

#define	SYSTIMER_FLIP_OFF	0
#define	SYSTIMER_FLIP_ON	1
unsigned char F_SysTimer_Flipper;
unsigned short ScreenWait;
unsigned char ScreenScenario;

int main( void )
{
  WDTCTL = WDTPW + WDTHOLD;  // Stop WDT

  InitializeClocks();
  // InitializeButton();
  InitializeLeds();
  PreApplicationMode();
  ConfigureTimerPwm();

  __enable_interrupt();
  ConfigureAdcTempSensor();
  InitLCD();
  GetTLV();
  __disable_interrupt();
  initializeDCO();
  ConfigureTimerPwm();
  __enable_interrupt();
  LED_OUT |= LED1;  // Runup

  /* Main Application Loop */
  while(1)
  {
    // State Machines
    SubLCD();
    ScreenScene();
  }
}

void PreApplicationMode( void )
{    
  SysTimer_Counter = 0;
  F_SysTimer_Flipper = SYSTIMER_FLIP_OFF;
  ClockMode = Low;

  ScreenWait = 0;
  ScreenScenario = 0;
//  __bis_SR_register( LPM3_bits + GIE );  // LPM0 with 
//  __bis_SR_register( LPM3_bits );
}

void ConfigureTimerPwm( void )
{
  if( ClockMode == Low )
  {
    TACCR0 = TIMER_PWM_PERIOD;	// Compare Maxim value
#ifdef TARGET_XT1
    TACTL = TASSEL_2 | MC_1;	// TACLK = SMCLK, Up mode.
#else
    TACTL = TASSEL_1 | MC_1;	// TACLK = ACLK, Up mode.
#endif
    TACCTL0 = CCIE;		// TACCTL0 output OUT bit(not used)
    TACCTL1 = CCIE + OUTMOD_3;	// TACCTL1 Capture Compare, Set/reset
    TACCR1 = TIMER_PWM_CENTER;	// Compare Center value
  }
  else // ClockMode Low to High
  {
    TACCR0 = TIMER_PWM_PERIOD_DCO;
    TACCR1 = TIMER_PWM_CENTER_DCO;
  }
}

void InitializeClocks( void )
{
  /* FIXME: look LFXT1OF, and choose one */
#ifdef TARGET_XT1
  {
    // see F2xx manual section 5.2.7.1
    unsigned char loop;
    
    __bic_SR_register( OSCOFF_bits ); // Oscillator ON
    BCSCTL1 &= ~( XTS ); // Set low oscillator
    BCSCTL3 = LFXT1S_0; // Use LFXT1
    while( 1 )
    {
      IFG1 &= ~( OFIFG );
      for( loop = 0; loop < 254; loop++ );
      if( ( IFG1 & OFIFG ) == 0 ) break;
    }
    BCSCTL2 |= SELM_3;
  }
#else
  DCOCTL = 0;
  BCSCTL1 = 0;
  BCSCTL1 = XT2OFF | DIVA_0 | RSEL0;
                          // Set ACLK / 1
  BCSCTL2 = 0;
  BCSCTL2 = SELM_3 | DIVM_0 | SELS | DIVS_0;
                          // SMCLK = MCLK = ACLK = 12kHz
  BCSCTL3 = 0;
  BCSCTL3 = LFXT1S_2; // use VLO 
#endif
}

void initializeDCO( void )
{
  DCOCTL = Var_CALDCO_12MHz;
  DCOCTL |= DCO2;
  BCSCTL1 = Var_CALBC1_12MHz;
  BCSCTL1 |= DIVA_0;
  BCSCTL2 &= ~( DIVS_3 ); 
  BCSCTL2 = SELM_0 | DIVM_0; // DCO = MCLK = SMCLK = ACLK(12MHZ, VLO * 1000)
  TACTL = TASSEL_2 | MC_2 | TACLR;
      // Select SMCKL as source, no divider, Continuous mode and reset timer
  BCSCTL1 |= XT2OFF; // XT2 off, and Set ACLK to /1 divider

  ClockMode = High;
}

void InitializeLeds( void )
{
  LED_DIR |= LED1 + LED2;                          
  LED_OUT &= ~( LED1 + LED2 );
}



interrupt ( TIMER0_A0_VECTOR ) TimerA0_ISR( void )
{	
  TACCTL0 &= ~CCIFG;
}

interrupt ( TIMER0_A1_VECTOR ) TimerA1_ISR( void )
{
  SysTimer_Counter++;
  if( SysTimer_Counter >= SYSTIMER_COUNTUP )
  {
    if( F_SysTimer_Flipper == SYSTIMER_FLIP_OFF )
    {
      F_SysTimer_Flipper = SYSTIMER_FLIP_ON;
      LED_OUT |= LED2;
    }
    else
    {
      F_SysTimer_Flipper = SYSTIMER_FLIP_OFF;
      LED_OUT &= ~LED2;
    }
    SysTimer_Counter = 0;
  }

  LcdWait++;
  ScreenWait++;

  TACCTL1 &= ~CCIFG;
}

// WDT Interrupt Service Routine
interrupt ( WDT_VECTOR ) WDT_ISR( void )
{
  IE1 &= ~WDTIE;  /* disable interrupt */
  IFG1 &= ~WDTIFG;  /* clear interrupt flag */
  WDTCTL = WDTPW + WDTHOLD;  /* put WDT back in hold state */
}

