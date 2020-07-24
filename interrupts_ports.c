//==============================================================================
//      Chris Hamby Presents...
//
//      interrupts_ports.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      if you press a button, it triggers this interrupt
//      
//      there's also some information regarding interrupts
//      from back when I didn't understand them like the back of my hand :-)
//==============================================================================
#include "msp430.h"
#include "macros.h"
#include "functions.h"

//INTERRUPT VECTORS FOR PORTS=================
//PxIFG and PxIV, x=1..8
//recall that each port has 8 pins
//the interrupt vector corresponds to these
//02h -> Port x.0
//04h -> Port x.1
//...and so on
//PxIFG contains every flag
//The highest priority flag is selected
//PxIV contains the highest priority flag

//INTERRUPT ENABLE REGISTER===================
//PxIE controls which interrupts are enabled
//0   -> disabled
//1   -> enabled

//only transitions cause interrupts
//PxIES controls which edge triggers the interrupt
//0   -> rising edge
//1   -> falling edge
//accessing the lower byte of the PxIV register resets highest flag


//Button Ports:
//Button 1:Port 5.6
//Button 2:Port 5.5
//buttons disable themselves for the debounce period
//buttons are enabled by the TIMER0_A1_ISR interrupt
volatile char button1_debouncing = NO;
volatile char button2_debouncing = NO;
unsigned volatile int button1_debounce_count = COUNT_RESET;
unsigned volatile int button2_debounce_count = COUNT_RESET;

#pragma vector = PORT5_VECTOR
__interrupt void PORT5_ISR(void) {
  //P5IV contains the highest priority flag
  //switch(__even_in_range(P5IV, PORT_IV_MAX)){ 
  switch(P5IV) {
    case (BUTTON1_FLAG):        //BUTTON 1 PRESSED==================
      button1_pressed = YES;
      P5IE &= ~BUTTON1;         //disable this interrupt until debounced
      button1_debouncing = YES; //begin the debounce period
      button1_debounce_count = COUNT_RESET;
      TA0CCTL1 |= CCIE;         //enable the interrupt timer
      break;
    case (BUTTON2_FLAG):        //BUTTON 2 PRESSED=================
      button2_pressed = YES;
      P5IE &= ~BUTTON2;         //disable this interrupt until debounced
      button2_debouncing = YES; //begin the debounce period
      button2_debounce_count = COUNT_RESET;
      TA0CCTL1 |= CCIE;         //enable the interrupt timer
      break;
    default: break;
  }
}
