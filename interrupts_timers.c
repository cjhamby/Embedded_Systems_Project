//==============================================================================
//      Chris Hamby Presents...
//
//      interrupts_timers.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      This file contains the ISRs for Timer A0
//
//      There are two CCR registers utilized, CCR0 and CCR1
//      CCR0 is always running
//      CCR1 is the button debounce timer, it only runs when a button is debouncing
//
//      They are currently both configured to flag an interrupt every 100ms
//==============================================================================
#include "msp430.h"
#include "macros.h"
#include "functions.h"
//INTERRUPTS====================================================================
//The #pragma vector indicates to the compiler
//that the immediately following code is the ISR
//#pragma vector = [assigned vector],
//where [assigned vector] is defined in the msp430.h header


//This interrupt is sourced from TA0CCR0.CCIFG
//which is the highest priority interrupt from Timer A0
//and occurs when TIMERA0 reaches the TA0CCR0 value
#pragma vector = TIMER0_A0_VECTOR               //the interrupt vector
__interrupt void Timer0_A0_ISR(void){           //the ISR can be named anything
  TA0CCR0 += TA0CCR0_INTERVAL;                  //add offset to TA0CCR0
  TA0_CCR0_COUNT++;                             //counts how many intervals have passed        
}

//This interrupt handles flags from TA0IV
//for when TA0 reaches values in C/C Registers CCRx != CCR0
//The 100ms interrupt is enabled only when a button is debouncing
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void){
  switch(__even_in_range(TA0IV, TIMER_IV_MAX)) {   //when flagged, TA0IV value is an even number <=14
  case CCR_NOFLAG: break;       //no interrupt
  case CCR1_FLAG:               //100ms Interrupt
    //BUTTON 1 Interrupt ===============================================
    if(button1_debouncing) {
      if(button1_debounce_count++>=BUTTON1_DEBOUNCE_MAX_COUNT) {
        P5IV = EMPTY;             //stops duplicate reads/pressing
        P5IFG = EMPTY;
        P5IE |= BUTTON1;          //re-enable the button 1 interrupt
        button1_pressed = NO;
        button1_debouncing = NO;  //the button can now be pressed again
      }                           //note that button_pressed isn't set to NO here
    }                             //but it is set to NO by the code which uses it
    //BUTTON 2 Interrupt ===============================================
    if(button2_debouncing) {
      if(button2_debounce_count++>=BUTTON2_DEBOUNCE_MAX_COUNT){
        P5IV = EMPTY;
        P5IFG = EMPTY;
        P5IE |= BUTTON2;          //re-enable the button 2 interrupt
        button2_pressed = NO;
        button2_debouncing = NO;  //the button can now be pressed again
      }
    }    
    if(!button1_debouncing && !button2_debouncing)
      TA0CCTL1 &= ~CCIE;        //disable this interrupt until another button is pressed
    break;
  case TIMER_IV_MAX:  break;    //timer overflow; timer restarts at 0 automatically
  default: break;
  }
}
