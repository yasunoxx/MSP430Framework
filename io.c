/*
 * io.c -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

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

#include <legacymsp430.h>
#include "io.h"

volatile unsigned short TempReg;

void ConfigureAdcTempSensor( void )
{
  /* Configure ADC Temp Sensor Channel */
  ADC10CTL1 = INCH_10 + ADC10DIV_3 + SHS_1;
                        // Temp Sensor ADC10CLK/4, Timer_A.OUT1 Trigger
  ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + REF2_5V + ADC10ON;  // + ADC10IE;
  ADC10CTL0 &= ~ADC10IFG;
  ADC10CTL0 |= ENC + ADC10SC;  // Sampling and conversion start
}

unsigned short SampleAndConversionAdcTemp( void )
{
  unsigned short tempreg;

  tempreg = ADC10MEM;
  ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion start
  return tempreg;
}

void InitializeButton( void )   // Configure Push Button 
{
  BUTTON_DIR &= ~BUTTON;
  BUTTON_OUT |= BUTTON;
  BUTTON_REN |= BUTTON;
  //  BUTTON_IES |= BUTTON;
  //  BUTTON_IFG &= ~BUTTON;
}



interrupt ( PORT1_VECTOR ) PORT1_ISR( void )
{
  /* FIXME: Can't debug this !! */
  //  BUTTON_IFG = 0;
  //  BUTTON_IE &= ~BUTTON;  /* Debounce */
  //  WDTCTL = WDT_ADLY_250;
  //  IFG1 &= ~WDTIFG;  /* clear interrupt flag */
  //  IE1 |= WDTIE;

  // __bic_SR_register_on_exit( LPM3_bits );
}

// ADC10 interrupt service routine
interrupt ( ADC10_VECTOR ) ADC10_ISR( void )
{
  TempReg = ADC10MEM;
  ADC10CTL0 &= ~ADC10IFG;
  ADC10CTL0 |= ENC + ADC10SC;
}
