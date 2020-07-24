//==============================================================================
//      Chris Hamby Presents...
//
//      shapes.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      if you want to move a motor, chances are your function is in here
//      how do we move motors?  by assigning values to the following registers
//              TB0CCR3         LEFT_REVERSE_SPEED
//              TB0CCR4         LEFT_FORWARD_SPEED
//              TB0CCR5         RIGHT_REVERSE_SPEED
//              TB0CCR6         RIGHT_FORWARD_SPEED
//
//      the values we assign to these registers are used in PWM
//      the speed, as a percentage of the maximum always-on speed is:
//
//              (SPEED %) = (REGISTER_VALUE) / (WHEEL_PERIOD)
//
//      using the global functions ensures that forward and reverse are not
//      simultaneously enabled.  if they were, something bad would happen.
//
//
//      global functions:
//              Forward_Move(void)              move forward until stopped
//              Reverse_Move(void)              move backwards until stopped
//              Motors_Off(void)                stop moving
//
//              Brake_Left(void)                brake the left wheel
//              Brake_Right(void)               brake the right wheel
//              Brake_All(void)                 brake both wheels
//
//              Forward_Timed(int num_ms)       move in increments of 100ms
//              Reverse_Timed(int num_ms)       macros defined in terms of TA0CCR0
//              Left_Timed(int num_ms)          ONE_SECOND = ONE_SECOND_TA0CCR0
//              Right_Timed(int num_ms)         TA0CCR0 is a 100ms timer
//
//              Turn_Right(void)                turn 90 degrees CW
//              Turn_Left(void)                 turn 90 degrees CCW
//              Turn_180(void)                  turn 180 degrees
//
//              MotorTest_Setup(void)    
//              MotorTest_Process(void)         test motor functionality
//              FindLine_Setup(void)     
//              FindLine_Process(void)          go forward until black line is found
//              FollowLine_Setup(void)   
//              FollowLine_Process(void)        track a black line
//
//
//      local functions
//              Left_Forward(void)
//              Left_Reverse(void)
//              Right_Forward(void)
//              Right_Reverse(void)
//              MotorTest1(void)
//              SpeedAdjust(void)
//              Detector_On_Line(int)
//==============================================================================

#include "macros.h"
#include "msp430.h"
#include <string.h>

//==================================================
//              Local Function List
//==================================================
void Left_Forward(void);        //control individual wheels
void Left_Reverse(void);   
void Right_Forward(void);       //ensure that the opposite direction is disabled   
void Right_Reverse(void);

void MotorTest1(void);                          //test all the wheel functionality
void SpeedAdjust(void);                         //line following controller
char Detector_On_Line(int detector_value);      //line detection function

// Variables-------------------------------------------------------------------
extern volatile unsigned char test_state = NO;
extern volatile char MotorTest_OneTime = NO;

char findLine_oneTime   = YES;          //these variables are used to 
char followLine_oneTime = YES;          //run the setup function one time
char foundLine          = NO;           //indicates completion of FindLine          
char findLine_State     = EMPTY;        //state machine for finding a line
char followLine_State   = EMPTY;        //state machine for following a line
char switched_turn_state = YES;         //indicates a fresh change of direction
char TURN_STATE         = NO_TURN;      //the current turning direction for line following


//==============================================================================
//                      Motor Control
//==============================================================================
//  <WHEEL>_FORWARD_SPEED and <wheel>_REVERSE_SPEED
//  one must be set to 0 before the other is enabled
//
//LEFT MOTOR CONTROL ===========================================================
void Left_Forward(void) {
  LEFT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = LEFT_DEFAULT_TRAVEL_SPEED;
}
void Left_Reverse(void) {
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = DEFAULT_SPEED_REVERSE;
}
void Left_Off(void) {
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

//RIGHT MOTOR CONTROL ==========================================================
void Right_Forward(void) {
  RIGHT_REVERSE_SPEED = WHEEL_OFF;      
  RIGHT_FORWARD_SPEED = RIGHT_DEFAULT_TRAVEL_SPEED;  
}
void Right_Reverse(void) {
  RIGHT_FORWARD_SPEED = WHEEL_OFF;      
  RIGHT_REVERSE_SPEED = DEFAULT_SPEED_REVERSE;
}
void Right_Off(void) {
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
}

//HIT THE BRAKES ===============================================================
void Brake_Left(void){
  Left_Reverse();
  Left_Off();
}
void Brake_Right(void){
  Right_Reverse();
  Right_Off();
}
void Brake_All(void){
  Motors_Off();       //ensure forward motion is off before going backwards
  Reverse_Move();     //"brake" by putting the wheels in reverse
  Motors_Off();       //turn the motors off for real
}

//==============================================================================
//                  Basic Movement  
//==============================================================================
void Forward_Move(void) {
  Left_Forward();
  Right_Forward();
}
void Reverse_Move(void) {
  Left_Reverse();
  Right_Reverse();
}
void Motors_Off(void) {
  Left_Off();
  Right_Off();
}


void Turn_Right(void){
  Right_Timed(TURN_RIGHT_TIME);
}
void Turn_Left(void){
  Left_Timed(TURN_LEFT_TIME);
}
void Turn_180(void){
  Left_Timed(TURN_180_TIME);
}



//==============================================================================
//                      Motor Test 
//==============================================================================
//individually cycle through the wheels by pressing button 1
//ensure that both wheels can go forward and reverse

void MotorTest_Setup(void){
  test_state = EMPTY;
  MotorTest_OneTime = NO;
  strcpy(display_line[DISPLAY_LINE_1], "Motor Test");
  strcpy(display_line[DISPLAY_LINE_2], "B2 to Quit");
}

//Menu macro: MOTORTEST
void MotorTest_Process(void) {
  showRTC200(DISPLAY_LINE_3);   //show the timer
  if(MotorTest_OneTime){        //only assign the motor speeds once
    MotorTest_OneTime = NO;
    switch (test_state){
      case EMPTY:
        MotorTest_Setup();
        break;
      case TEST_STATE1:
        Left_Forward();
        Right_Off();
        strcpy(display_line[DISPLAY_LINE_4], "^        -");
        break;
      case TEST_STATE2:
        Left_Off();
        Right_Forward();
        strcpy(display_line[DISPLAY_LINE_4], "-        ^");
        break;
      case TEST_STATE3:
        Right_Off();
        Left_Reverse();
        strcpy(display_line[DISPLAY_LINE_4], "v        -");
        break;
      case TEST_STATE4:
        Left_Off();
        Right_Reverse();
        strcpy(display_line[DISPLAY_LINE_4], "-        v");
        break;
      case TEST_STATE5:
        Motors_Off();
        strcpy(display_line[DISPLAY_LINE_4], "-        -");
        break;
    }
  }
  //press button 1 to advance the state
  if(Check_Button_1()) {
    MotorTest_OneTime = YES;
    test_state++;
    if(test_state >= MAX_TEST_STATE)
      test_state = EMPTY;
  }
  
  if(Check_Button_2()) {
    Motors_Off();
    endEvent();
  }
}


//==============================================================================
//                   timed movement
//==============================================================================
void Forward_Timed(int num_ms){
  Forward_Move();
  delay_100ms(num_ms);
  Brake_All();
}
void Left_Timed(int num_ms){
  Right_Forward();
  Left_Reverse();
  delay_100ms(num_ms);
  Motors_Off();
}
void Right_Timed(int num_ms){
  Left_Forward();
  Right_Reverse();
  delay_100ms(num_ms);
  Motors_Off();
}
void Reverse_Timed(int num_ms){
  Reverse_Move();
  delay_100ms(num_ms);
  Motors_Off();
}

//==============================================================================
//                   Find Line
//==============================================================================
// Go forward until a black line is detected
// Requires proper calibration
// Afterwards, turn right and begin following the line
void FindLine_Setup(void) {
    foundLine = NO;             //can't find the line before looking for it!
    clearDisplay();
    strcpy(display_line[DISPLAY_LINE_1], "Find Line ");
    strcpy(display_line[DISPLAY_LINE_4], "B2 to Menu");
    resetRTC200();              //reset the timer that is displayed
    Enable_Emitter();
    Forward_Move();
    findLine_State = FINDLINE_RUN;
}

//go forward until the detectors are both on a black line
void FindLine_Process(void){
  switch(findLine_State){
    case (FINDLINE_SETUP):      //beginning this process
      FindLine_Setup();         //run the setup function
    break;
    
    case (FINDLINE_RUN):        //running this process
      if(Detector_On_Line(ADC_Left_Detector) && Detector_On_Line(ADC_Right_Detector)){
        foundLine = YES;                        //might be used globally
        findLine_State = FINDLINE_FOUND;        //advance state machine
      }
    break;
    
    case (FINDLINE_FOUND):      //written with project 7 in mind
      Disable_Emitter();        //don't need this until we start following the line
      Brake_All();              //stop
      Turn_Right();              //turn left to align car with circle
      strcpy(display_line[DISPLAY_LINE_2], "Found Line");
      findLine_State = FINDLINE_SETUP;  //reset for next time
      event = FOLLOW_LINE;              //next thing to do
      delay_100ms(ONE_SECOND);
      break;
  }

  showRTC200(DISPLAY_LINE_3);   //show the millisecond clock
  if(Check_Button_2()) {
    endEvent();
    findLine_State = FINDLINE_SETUP;
  }
}

//returns YES if the detector value is at least grey
//but makes sure the emitter is actually on (if not, everything looks black)
char Detector_On_Line(int detector_value){
  if(detector_value > white_threshold && detector_value < on_threshold) 
    return YES;
  return NO;
}





//==============================================================================
//                   Follow Line 
//==============================================================================
//the car will follow a black line on a white background

void FollowLine_Setup(void){
  if(!foundLine)        //did we come from the FindLine_Process()?
    resetRTC200();      //if not, might as well start RTC200 over
  
  strcpy(display_line[DISPLAY_LINE_1], "Track Line");
  strcpy(display_line[DISPLAY_LINE_4], "B2 to Menu");
  Enable_Emitter();
  delay_100ms(VHUNDRED_MS);     //delay 500 ms
  Forward_Move();               //start moving forward
  followLine_State = FOLLOWLINE_RUN; 
}


//Follow the line around the circle twice
//Turn into the circle and stop
//Display the clock/how much time has passed
void FollowLine_Process(void){
  char tempChar = get_UCA3_RX();
  if(tempChar == 'L'){
    Turn_Left();
    Forward_Timed(ONE_SECOND);
    Brake_All();
    followLine_State = FOLLOWLINE_COMPLETE;
  }
  if(tempChar == 'R'){
    Turn_Right();
    Forward_Timed(ONE_SECOND);
    Brake_All();
    followLine_State = FOLLOWLINE_COMPLETE;

  }
  switch(followLine_State){
    case(FOLLOWLINE_SETUP):     //get everything ready to run
      FollowLine_Setup();
      break;
      
    case(FOLLOWLINE_RUN):               //follow the black circle
      SpeedAdjust();                    //adjust speeds to stay on the line
      if(RTC200 >= RTC200_CIRCLE){      //experimentally determine this time
        followLine_State = FOLLOWLINE_INTO_CIRCLE;
        Brake_All();
      }
      break;
      
    case(FOLLOWLINE_INTO_CIRCLE):       //go into the circle
      Turn_Left();                     //this should be the right direction
      Forward_Timed(ONE_SECOND);        //go into the circle, actually
      followLine_State = FOLLOWLINE_COMPLETE;   //indicate completion of the program
      runTimer = NO;
      break;
      
    case(FOLLOWLINE_COMPLETE):  //do nothing
      break;
      
  default:break;                //do nothing
  }

  if(Check_Button_2()) {
    endEvent();
    Brake_All();
    followLine_State = FOLLOWLINE_SETUP;
  }
}


//==============================================================================
//                   Speed Adjust Controller 
//==============================================================================
//first detector to see white determines the turn direction
// left detector sees white first -> turn right
// right detector sees white first -> turn left
//second detector to see white makes the turn sharper
void SpeedAdjust(void){
  switch(TURN_STATE){
    case NO_TURN:                                       //GOING STRAIGHT
      if(ADC_Left_Detector < white_threshold){          //left detector sees white
        TURN_STATE = TURN_RIGHT;                        //time to turn right
      }
      else if(ADC_Right_Detector < white_threshold){    //right detector sees white
        TURN_STATE = TURN_LEFT;                         //time to turn left
      }
      else if(switched_turn_state){                     //just finished turning, reset speed values
        LEFT_FORWARD_SPEED = LEFT_DEFAULT_FOLLOW_SPEED;        //go straight until the next turn
        RIGHT_FORWARD_SPEED = RIGHT_DEFAULT_FOLLOW_SPEED;
        switched_turn_state=NO;
      }
    break;
    //==========================================================================
    case TURN_LEFT:                              // RIGHT DETECTOR IS ON WHITE 
      RIGHT_FORWARD_SPEED += SPEED_INCREMENT;    //increase right wheel speed to turn left
      if(RIGHT_FORWARD_SPEED >= RIGHT_MAX_SPEED) //bound the speed
        RIGHT_FORWARD_SPEED = RIGHT_MAX_SPEED;
 
      if(ADC_Right_Detector > black_threshold){  //right detector is back on line
        TURN_STATE = NO_TURN;
        switched_turn_state=YES;
      }
      if(ADC_Left_Detector < white_threshold){   //left detector sees white also
        Brake_Left();
      }
      else{
        if(LEFT_FORWARD_SPEED > LEFT_MIN_SPEED)  //the slower wheel approaches LEFT_MIN_SPEED
          LEFT_FORWARD_SPEED -= SPEED_DECREMENT; //it slows down if previously full speed
        else
          LEFT_FORWARD_SPEED+= SPEED_INCREMENT;  //it speeds up if previously braked
      }
    break;
    //==========================================================================    
    case TURN_RIGHT:                            // LEFT DETECTOR IS ON WHITE 
      LEFT_FORWARD_SPEED += SPEED_INCREMENT;    //increase left wheel speed to turn right
      if(LEFT_FORWARD_SPEED >= LEFT_MAX_SPEED)  //bound the speed
        LEFT_FORWARD_SPEED = LEFT_MAX_SPEED;    //this is the fastest allowable speed
      if(ADC_Left_Detector > black_threshold){  // left detector is back on line
        TURN_STATE = NO_TURN;
        switched_turn_state=YES;
      }
      if(ADC_Right_Detector < white_threshold){ // right detector sees white also
        Brake_Right();                          // right wheel stops entirely in this case
      }
      else{
        if(RIGHT_FORWARD_SPEED > RIGHT_MIN_SPEED)  //the slower right wheel approaches RIGHT_MIN_SPEED
          RIGHT_FORWARD_SPEED -= SPEED_DECREMENT;  //it slows down if previously full speed
        else
          RIGHT_FORWARD_SPEED += SPEED_INCREMENT;  //it speeds up if previously braked
      }
    break;
  }
}
