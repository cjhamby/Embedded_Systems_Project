//==============================================================================
//      Chris Hamby Presents...
//
//      menu.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
// 
//      The car has two basic states : MENU/EVENT
//      MENU state -    scroll with the wheel to see the possible events
//                      press button 2 to begin the event selection
//                      press button 1 to toggle the backlight
//      EVENT state -   Some sort of process is occuring
//                      in general,
//                      button 1 and wheel interact with the event process
//                      button 2 returns to the menu
//
//      This file includes the functions for previewing the events in MENU state
//      and for executing the correct process in an EVENT state
//
//
//      global functions:
//              Event_Process()
//              Menu_Process()
//              endEvent()
//
//      local functions:
//              Wheel_To_Menu_Selection(void)
//              Preview_Event(void)
//
//==============================================================================

#include "macros.h"
#include  "msp430.h"
#include  "functions.h"
#include <string.h>
void Wheel_To_Menu_Selection(void);
void Preview_Event(void);
extern volatile char event = EMPTY;             //for running a process
extern char next_event     = EMPTY;             //for previewing a selection



void Event_Process(void){       //handles execution of an event
  switch (event) {
    case SHOW_ADC:              //show the ADC values on the screen
      Show_Adc_Process();
      break;
    case CALIBRATE:             //calibrate the IR detectors
      Calibrate_Process();
      break;
    case MOTORTEST:             //ensure both motors run F/R
      MotorTest_Process();
      break;
    case FIND_LINE:             //drive forward until a black line is found
      FindLine_Process();
      break;
    case FOLLOW_LINE:           //follow a black line on a white ground
      FollowLine_Process();
      break;
    case SHOW_RTC200_PROCESS:   //display the timer
      Show_RTC200_Process();
      break;
    case IOT_PROCESS:           //communicate with the IOT module
      IOT_Process();
      break;
    case IOT_ENABLE:            //open the IOT module port
      IOT_Enable_Process();
      break;
    default:                    //no event- MENU process
      Menu_Process();
      break;
  }
}


//in MENU state, this function shows the event selection
//pressing button 2 will begin the event
void Preview_Event(void) {
  char myNextEvent[NUM_DISPLAY_CHARS] = "No Select ";
  switch(next_event) {
    case SHOW_ADC:
      strcpy(myNextEvent, "Show ADCs ");
      break;
    case CALIBRATE:
      strcpy(myNextEvent, "Calibrate ");
      break;
    case MOTORTEST:
      strcpy(myNextEvent, "Motor Test");
      break;
    case FIND_LINE:
      strcpy(myNextEvent, "Find Line ");
      break;
    case FOLLOW_LINE:
      strcpy(myNextEvent, "Track Line");
      break;
    case SHOW_RTC200_PROCESS:
      strcpy(myNextEvent, "Show Timer");
      break;
    case IOT_PROCESS:
      strcpy(myNextEvent, "IOT Comms ");
      break;
    case IOT_ENABLE:
      strcpy(myNextEvent, "IOT Enable");
      break;
  }
  strcpy(display_line[DISPLAY_LINE_2], myNextEvent);
}


//use the top bits of the thumb wheel ADC value to select an event
//right shift the other bits out of the picture
void Wheel_To_Menu_Selection(void){
  next_event = ADC_Thumb>>REMOVE_LOWER_9BITS;
}

//MENU state
//Enable scrolling between events with the thumb wheel
//Preview the event on the screen
void Menu_Process(void) {
  strcpy(display_line[DISPLAY_LINE_1], "MAIN MENU ");
  strcpy(display_line[DISPLAY_LINE_3], "          ");
  strcpy(display_line[DISPLAY_LINE_4], "LIGHT   GO");
  
  Wheel_To_Menu_Selection();    //analog wheel scrolls through the menu
  Preview_Event();               //show the selection

  if(Check_Button_1())      //button 1 toggles the backlight
    P5OUT ^= LCD_BACKLITE;
  
  if(Check_Button_2()) {    //button 2 initiates the selection
      event = next_event;
  }
}

//end an EVENT state and return to the MENU state
void endEvent(void) {
  event      = EMPTY;
  next_event = EMPTY;
  Disable_Emitter();
  clearDisplay();
  Preview_Event();
}


