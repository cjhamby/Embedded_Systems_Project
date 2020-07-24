//==============================================================================
//      Chris Hamby Presents...
//
//      lcd.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      some various functions for interacting with the lcd screen
//      and formatting data to be shown on the screen
//
//      global functions
//              HEXtoBCD(int)
//              showADC(int, int)
//              showRTC200(int)
//              clearDisplay()
//
//==============================================================================
#include "macros.h"
#include <string.h>

char adc_char[ADC_STRING_SIZE];
char formatRTC200[NUM_DISPLAY_CHARS];
extern char shrug_guy[NUM_DISPLAY_CHARS] = "";
//extern char display_line[NUM_DISPLAY_LINES][NUM_DISPLAY_CHARS];
//****************************************************************************
// Hex to BCD Conversion
// Convert a Hex number to a BCD for display on an LCD or monitor
void HEXtoBCD(int hex_value){
  int value=EMPTY;
  strcpy(adc_char, "");
  adc_char [CONVERSION_CHAR_1] = '0';           //start empty
  while (hex_value> THOUSANDS_CHECK){           //find the thousands value
    hex_value= hex_value-DECREASE_THOUSAND;
    value = value + ADJUST_1;
  }
  adc_char[CONVERSION_CHAR_1] = MAKE_A_CHAR + value;  //assign the thousands value
  value = EMPTY;
  while (hex_value> HUNDREDS_CHECK){            //find the hundreds value
    hex_value= hex_value-DECREASE_HUNDRED;
    value = value + ADJUST_1;
  }
  adc_char[CONVERSION_CHAR_2] = MAKE_A_CHAR + value;  //assign the hundreds value
  value = EMPTY;
  while (hex_value> TENS_CHECK){                //find the tens value
    hex_value= hex_value-DECREASE_TEN;
    value = value + ADJUST_1;
  }
  adc_char[CONVERSION_CHAR_3] = MAKE_A_CHAR + value;  //assign the tens value
  adc_char[CONVERSION_CHAR_4] = MAKE_A_CHAR + hex_value;        //assign the (leftover) ones value
}

//******************************************************************************

void showADC(int ADC_value, int line) {
   HEXtoBCD(ADC_value);
   strcpy(display_line[line], adc_char);
}

void showRTC200(int line){
  int temp;
  strcpy(formatRTC200, "          ");
  formatRTC200[MS_PLACE] = (RTC200 % DECREASE_TEN)+MAKE_A_CHAR;
  temp = RTC200/DECREASE_TEN;
  formatRTC200[DEC_PLACE] = '.';
  formatRTC200[SEC_PLACE_L] = (temp % DECREASE_TEN)+MAKE_A_CHAR;
  temp /= DECREASE_TEN;
  formatRTC200[SEC_PLACE_M] = (temp % DECREASE_TEN)+MAKE_A_CHAR;
  temp /= DECREASE_TEN;
  formatRTC200[SEC_PLACE_H] = (temp % DECREASE_TEN)+MAKE_A_CHAR;
  strcpy(display_line[line], formatRTC200);
  strcpy((char*)UCA0_Char_Tx, formatRTC200);
}

void clearDisplay(void) {
  strcpy(display_line[DISPLAY_LINE_1], "          ");
  strcpy(display_line[DISPLAY_LINE_2], "          ");
  strcpy(display_line[DISPLAY_LINE_3], "          ");
  strcpy(display_line[DISPLAY_LINE_4], "          ");
}
