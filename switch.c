//==============================================================================
//      Chris Hamby Presents...
//
//      switch.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      The two functions here ensure that button presses are only read once
//
//      global functions
//              Check_Button_1(void)
//              Check_Button_2(void)
//
//==============================================================================

#include "macros.h"
#include "msp430.h"
#include "functions.h"

char Check_Button_1(void);
char Check_Button_2(void);
volatile unsigned char button1_pressed = NO;
volatile unsigned char button2_pressed = NO;
volatile unsigned long cooldown = COOLDOWN_TIME;  //debounce time

//each button press will do exactly one thing
//these functions ensure that
char Check_Button_1(void){
  if(button1_pressed){
    button1_pressed = NO;
    return YES;
  }
  return NO;
}

char Check_Button_2(void){
  if(button2_pressed){
    button2_pressed = NO;
    return YES;
  }
  return NO;
}