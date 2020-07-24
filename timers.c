//==============================================================================
//      Chris Hamby Presents...
//
//      timers.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      time-related events are handled here
//      Timer_Process() runs in the main OS loop
//      it keeps track of all timer-related values
//
//      global functions
//              Timer_Process(void)
//              Init_Timers(void)
//              Init_Timer_A0(void)
//              Init_Timer_B0(void)
//              delay_100ms(int)
//              Show_RTC200_Process(void)
//              resetRTC200(void)

//==============================================================================

#include "msp430.h"
#include "macros.h"
#include  "functions.h"
#include  <string.h>

unsigned volatile int TA0_CCR0_COUNT = COUNT_RESET;        //increments every 100ms
extern volatile unsigned int my_lcd_count = COUNT_RESET;   //how often to update LCD

extern char runTimer = YES;             //set to NO to pause RTC200
extern unsigned int RTC200 = EMPTY;     //a timer that increments every 200 ms
unsigned int delay_timer = COUNT_RESET; //a timer used in the delay_100ms function

unsigned int hundred_ms_count  = HUNDRED_MS;
unsigned int thundred_ms_count = TWOHUNDRED_MS;



//====================================================
//                Timer (Main) Process 
//====================================================
//The forward-facing function seen/called by main every loop
//TA0_CCR0_COUNT increments every 100 ms
void Timer_Process() {
  //100 ms flag
  if(TA0_CCR0_COUNT == hundred_ms_count) {
    hundred_ms_count += HUNDRED_MS;
    delay_timer++;
  }
  
  //200 ms flag
  if(TA0_CCR0_COUNT == thundred_ms_count) {
    thundred_ms_count += TWOHUNDRED_MS;
    Display_Update(NO, NO, NO, NO);             //refresh the LCD Display
    update_display= YES;                        //
    display_changed = YES;                      //
    if(runTimer)                        //if RTC200 is enabled
      RTC200 += RTC200_INCREMENT;       //increment the display clock
  }
  
  //1 second flag
  if(TA0_CCR0_COUNT >= ONE_SECOND_TA0CCR0) {    
    TA0_CCR0_COUNT = COUNT_RESET;       //wait another second before toggling again
    hundred_ms_count  = HUNDRED_MS;
    thundred_ms_count = TWOHUNDRED_MS;
  }
}



//====================================================
//            Real-Time Counter - 200 ms 
//====================================================
//as the name suggests, RTC200 is a timer that increments every 200 ms
void resetRTC200(void){
  RTC200 = EMPTY;
}

extern void Show_RTC200_Process(void){
  clearDisplay();
  showRTC200(DISPLAY_LINE_1);
  strcpy(display_line[DISPLAY_LINE_4],shrug_guy);
  if(Check_Button_1())
    resetRTC200();
  if(Check_Button_2())
    endEvent(); 
}


//====================================================
//            real time delay function
//====================================================
void delay_100ms(int delay_amount) {
  delay_timer = NO;
  while(delay_timer!=delay_amount)Timer_Process();
}



//====================================================
//            Initialization Functions 
//====================================================
//TBxCLn is the TBxCCRn compare latch
//It holds the data for the comparison to timer value in compare mode
//
//TBxCLn is buffered by TBxCCRn
//The buffer gives the user control over when a compare period updates
//The timing of this transfer from TBxCCRn buffer to TBxCLn is selectable
//using the CCLD bits

//CCTLx = OUTMOD_7      sets all "non-OUTMOD" bits to 0
//Each TB0CCTLn register is in compare mode -- good for PWM
//enable the interrupts
//Reset/Set mode - set output low when TB0 > TB0CLn

//Call this to set up all of the timers
void Init_Timers(void) {
  Init_Timer_A0();
  Init_Timer_B0();
}

//Setup Timer A0 using knowledge from the user guide
void Init_Timer_A0(void) {
  TA0CTL = TASSEL__SMCLK;       //use SMCLK as the source
  TA0CTL |= TACLR;              //reset TA0R, clock divider, and MC
  TA0CTL |= MC__CONTINUOUS;     //continuous timer mode, 0 -> 0xFFFFh
//Divider==================================================
  TA0CTL |= ID__2;       //divide clock by 2
  TA0EX0 = TAIDEX__8;    //divide clock by an additional 8
  
//Capture/Control Registers================================
  //blinking LCD
  TA0CCR0 = TA0CCR0_INTERVAL;   //every 10 ms
  TA0CCTL0 |= CCIE;   //CCR0 enable interrupt
  //Button Debounce
  TA0CCR1 = TA0CCR1_INTERVAL;   //every 100 ms
  TA0CCTL1 &= ~CCIE;   //CCR1 enable interrupt
  
  TA0CTL &= ~TAIE;      //Disable overflow interrupt 
  TA0CTL &= ~TAIFG;     //Clear overflow interrupt flag
  TA0CTL |= TACLR;      //Clear existing clock divider logic
}


void Init_Timer_B0(void) {
// SMCLK source, up count mode, PWM Right Side    
// TB0.1 P3.6 R_REVERSE
// TB0.2 P3.7 R_FORWARD
// TB0.3 P3.4 L_REVERSE
// TB0.4 P3.5 L_FORWARD     

  TB0CTL = TBSSEL__SMCLK;    //SMCLK source
  TB0CTL |= MC__UP;          //up mode
  TB0CTL |= TBCLR;

  TB0CCR0 = WHEEL_PERIOD;    //the total period
  TB0CCTL3 = OUTMOD_7;  
  LEFT_REVERSE_SPEED = WHEEL_OFF;       //change these values to move the car
  TB0CCTL4 = OUTMOD_7;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  TB0CCTL5 = OUTMOD_7;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  TB0CCTL6 = OUTMOD_7;
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
}