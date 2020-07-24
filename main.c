//==============================================================================
//      Chris Hamby Presents...
//
//      main.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      contains the operating system
//==============================================================================
#include "macros.h"
#include  "functions.h"
//#include  "msp430.h"
//#include <string.h>


void main(void){  
  Init_Conditions();  //All initialization functions are included in this (init.c)           
//========================================================
  while(ALWAYS) {       //the Operating System (OS)
    Display_Process();  //handles the LCD screen
    Timer_Process();    //handles time flags 
    Event_Process();    //handles the menu event
    ADC_Process();      //handles the emitter/detector
  }
}