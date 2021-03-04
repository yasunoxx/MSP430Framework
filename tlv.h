/*
 * tlv.h -- MSP430Framework for LaunchPad
 * by yasunoxx
 * ### Use mspgcc(4.6.3 or later) only !!! ###
 */

/* original source by Paul Haddad
   http://e2e.ti.com/support/microcontrollers/msp430/f/166/t/89118.aspx?pi24337=1
*/

#define TLV_START (0x10C0)
#define TLV_END (0x10FF)
#define TLV_SIZE (TLV_END - TLV_START + 1)

#ifndef TAG_EMPTY
#define TAG_EMPTY 0x0FE
#define TAG_DCO_30 0x01
// #define TAG_ADC12_1 0x10
#define TAG_ADC10_1 0x10
#endif

extern unsigned short Var_CAL_ADC_GAIN_FACTOR;
extern unsigned short Var_CAL_ADC_OFFSET;
extern unsigned short Var_CAL_ADC_15VREF_FACTOR;
extern unsigned short Var_CAL_ADC_15T30, Var_CAL_ADC_15T85;
extern unsigned short Var_CAL_ADC_25VREF_FACTOR;
extern unsigned short Var_CAL_ADC_25T30, Var_CAL_ADC_25T85;
extern unsigned char Var_CALDCO_16MHz, Var_CALBC1_16MHz;
extern unsigned char Var_CALDCO_12MHz, Var_CALBC1_12MHz;
extern unsigned char Var_CALDCO_8MHz, Var_CALBC1_8MHz;
extern unsigned char Var_CALDCO_1MHz, Var_CALBC1_1MHz;

extern void GetTLV( void );
