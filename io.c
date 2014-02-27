/*
 * io.c -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
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
