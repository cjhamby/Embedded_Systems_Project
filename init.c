//==============================================================================
//      Chris Hamby Presents...
//
//      init.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      This file holds the Init_Conditions() function, which initializes everything
//
//==============================================================================
#include  "msp430.h"
#include  "macros.h"
#include "functions.h"
#include <string.h>

void Init_Conditions(void){
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;
  clearDisplay();

  //don't know what this does, try commenting it out later
  display[DISPLAY_1] = &display_line[DISPLAY_LINE_1][FIRST_CHAR];
  display[DISPLAY_2] = &display_line[DISPLAY_LINE_2][FIRST_CHAR];
  display[DISPLAY_3] = &display_line[DISPLAY_LINE_3][FIRST_CHAR];
  display[DISPLAY_4] = &display_line[DISPLAY_LINE_4][FIRST_CHAR];
  update_display = DISPLAY_RESET;
  update_display_count = DISPLAY_RESET;
  
// Interrupts are disabled by default, enable them.
  enable_interrupts();
  
// The Init_ functions are written in their respective .c files
  Init_Ports();         // Initialize Ports
  Init_Clocks();        // Initialize Clock System
  Init_Timers();        // Initialize Timers
  Init_LCD();           // Initialize LCD
  Init_ADC();           // Initialize ADC - IR detectors and wheel
  Init_Serial();        // Initialize Serial Communications
  
  //Initial LCD Contents
  strcpy(display_line[DISPLAY_LINE_2], "ChrisHamby");
  enable_display_update();
  display_changed = IT_CHANGED;
  Display_Update(NO, NO, NO, NO);
  
  P3OUT &= ~IOT_RESET;            //reset the IOT module
  delay_100ms(TWOHUNDRED_MS);     
  P3OUT |= IOT_RESET;  
  //The character emoji on the LCD (just for fun)
  strcpy(shrug_guy, SHRUG_MAN);
  shrug_guy[SHRUG_LHAND]    = SHRUG_HAND;  //the word shrug looks pretty weird 
  shrug_guy[SHRUG_LARM]     = SHRUG_ARM;   //if you think too hard about it
  shrug_guy[SHRUG_FACE_POS] = SHRUG_FACE;
  shrug_guy[SHRUG_RHAND]    = SHRUG_HAND;
}