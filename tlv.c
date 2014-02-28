/*
 * tlv.c -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

/* original source by Paul Haddad
   http://e2e.ti.com/support/microcontrollers/msp430/f/166/t/89118.aspx?pi24337=1
*/

#include <legacymsp430.h>
#include <string.h>
#include "tlv.h"


unsigned short Var_CAL_ADC_GAIN_FACTOR;
unsigned short Var_CAL_ADC_OFFSET;
unsigned short Var_CAL_ADC_15VREF_FACTOR;
unsigned short Var_CAL_ADC_15T30, Var_CAL_ADC_15T85;
unsigned short Var_CAL_ADC_25VREF_FACTOR;
unsigned short Var_CAL_ADC_25T30, Var_CAL_ADC_25T85;
unsigned char Var_CALDCO_16MHz, Var_CALBC1_16MHz;
unsigned char Var_CALDCO_12MHz, Var_CALBC1_12MHz;
unsigned char Var_CALDCO_8MHz, Var_CALBC1_8MHz;
unsigned char Var_CALDCO_1MHz, Var_CALBC1_1MHz;


void GetTLV( void )
{
  unsigned char data[ TLV_SIZE ];
  unsigned char count = 0;
  unsigned short checksum;
  
  memcpy( data, (void *)TLV_START, TLV_SIZE );  // Read TLV

  // FIXME: add here CHECKSUM VALIDATION
  checksum = data[ count++ ];
  checksum += data[ count ++ ] << 8;

  while( 1 )
  {
    if( data[ count ] == TAG_EMPTY )
    {
      count++;
      count += data[ count ];
      count++;
    }
    else if( data[ count ] == TAG_ADC10_1 || data[ count ] == TAG_ADC12_1 )
    {
      count++;
      count++; // size
      Var_CAL_ADC_GAIN_FACTOR = data[ count++ ];
      Var_CAL_ADC_GAIN_FACTOR += data[ count++ ] << 8;
      Var_CAL_ADC_OFFSET = data[ count++ ];
      Var_CAL_ADC_OFFSET += data[ count++ ] << 8;
      Var_CAL_ADC_15VREF_FACTOR = data[ count++ ];
      Var_CAL_ADC_15VREF_FACTOR += data[ count++ ] << 8;
      Var_CAL_ADC_15T30 = data[ count++ ];
      Var_CAL_ADC_15T30 += data[ count++ ] << 8;
      Var_CAL_ADC_15T85 = data[ count++ ];
      Var_CAL_ADC_15T85 += data[ count++ ] << 8;
      Var_CAL_ADC_25VREF_FACTOR = data[ count++ ];
      Var_CAL_ADC_25VREF_FACTOR += data[ count++ ] << 8;
      Var_CAL_ADC_25T30 = data[ count++ ];
      Var_CAL_ADC_25T30 += data[ count++ ] << 8;
      Var_CAL_ADC_25T85 = data[ count++ ];
      Var_CAL_ADC_25T85 += data[ count++ ] << 8;
    }
    else if( data[ count ] == TAG_DCO_30 )
    {
      count++;
      count++; // size
      Var_CALDCO_16MHz = data[ count++ ];
      Var_CALBC1_16MHz = data[ count++ ];
      Var_CALDCO_12MHz = data[ count++ ];
      Var_CALBC1_12MHz = data[ count++ ];
      Var_CALDCO_8MHz = data[ count++ ];
      Var_CALBC1_8MHz = data[ count++ ];
      Var_CALDCO_1MHz = data[ count++ ];
      Var_CALBC1_1MHz = data[ count++ ];
    }
    if( count >= TLV_SIZE ) break;
  } // endwhile
}

