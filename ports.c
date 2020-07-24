//==============================================================================
//      Chris Hamby Presents...
//
//      ports.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      Contains all necessary functions to set up the ports 
//      If you need to change a port pin's function, do it here
//
//      At the beginning of each Init_Port_<n>() function, the control registers
//      are configured such that each pin is configured to be a GPIO-OUT pin
//      Pins are changed from this default state as needed.
//      
//      global functions:
//              Init_Ports();
//
//      local functions:
//              Init_Port_<n>();        n= 1..8, J
//==============================================================================
#include  "msp430.h"
#include  "macros.h"


// Declarations for all of the functions
void Init_Ports(void);
void Init_Port_1(void);
void Init_Port_2(void);
void Init_Port_3(char p4_select);
void Init_Port_4(void);
void Init_Port_5(void);
void Init_Port_6(void);
void Init_Port_7(void);
void Init_Port_8(void);
void Init_Port_J(void);

// Call this function to initialize all of the ports
void Init_Ports(void){
  Init_Port_1();
  Init_Port_2();
  Init_Port_3(USE_TB0);       //USE_SMCLK, USE_L_REVERSE, USE_TB0
  Init_Port_4();
  Init_Port_5();
  Init_Port_6();
  Init_Port_7();
  Init_Port_8();
  Init_Port_J();
}

// for GPIO Pins, set PxOUT to 0 for LOW
//                             1 for high
//                set PxDIR to 0 for input
//                             1 for output
// use    &= ~(NAME)    to set a 0
// use    |= (NAME)     to set a 1
//
// the first four lines in each Init_Port_x function
// initialize all pins to GPIO - OUTPUT mode
// and set the initial output value to low
void Init_Port_1(void){
/*RED_LED         (0x01)
  GRN_LED         (0x02)
  V_THUMB         (0x04)
  TESTPROBE       (0x08)
  V_DETECT_R      (0x10)
  V_DETECT_L      (0x20)
  SD_UCB0SIMO     (0x40)
  SD_UCB0SOMI     (0x80)
*/
  P1SEL0 = EMPTY;         // all pins set to GPIO
  P1SEL1 = EMPTY;
  P1OUT  = EMPTY;         // no pin has an initial value yet
  P1DIR  = DIR_ALL_OUT;   // all pins set to OUT
  // P1_2 - V_THUMB ADC
  P1SEL0 |= V_THUMB;      // Select = [1, 1];
  P1SEL1 |= V_THUMB;      // note that function pins dont have direction
  // P1_4 - V_DETECT_R ADC
  P1SEL0 |= V_DETECT_R;
  P1SEL1 |= V_DETECT_R;
  // P1_5 - V_DETECT_L ADC
  P1SEL0 |= V_DETECT_L;
  P1SEL1 |= V_DETECT_L;
  // P1_6 -  SD_UCB0SIMO 
  P1SEL0 &= ~SD_UCB0SIMO;
  P1SEL1 |=  SD_UCB0SIMO;
  // P1_7 -  SD_UCB0SOMI
  P1SEL0 &= ~SD_UCB0SOMI;
  P1SEL1 |=  SD_UCB0SOMI;
}

void Init_Port_2(void){
/*UCA0TXD         (0x01)
  UCA0RXD         (0x02)
  SD_SPICLK       (0x04)
  P2_3            (0x08)
  P2_4            (0x10)
  UCA1TXD         (0x20)
  UCA1RXD         (0x40)
  P2_7            (0x80)
*/
  P2SEL0 = EMPTY;
  P2SEL1 = EMPTY;
  P2OUT  = EMPTY;
  P2DIR  = DIR_ALL_OUT;
  
  //conigure for serial communication
  P2SEL1 |= UCA0TXD;
  P2SEL0 &= ~UCA0TXD;
  P2SEL1 |= UCA0RXD;
  P2SEL0 &= ~UCA0RXD;
  
  //P2_2
  P2SEL0 &= ~SD_SPICLK;
  P2SEL1 |=  SD_SPICLK;
}

void Init_Port_3(char p4_select){
  // IOT_RESET       (0x01)
  // IOT_PROG_MODE   (0x02)
  // IOT_LINK        (0x04)
  // IOT_PROG_SEL    (0x08)
  // L_REVERSE       (0x10)
  // L_FORWARD       (0x20)
  // R_REVERSE       (0x40)
  // R_FORWARD       (0x80)
  P3SEL0 = EMPTY;
  P3SEL1 = EMPTY;
  P3DIR  = DIR_ALL_OUT;
  P3OUT  = EMPTY;
  
  //P3.0 - IOT_RESET
  P3OUT &= ~IOT_RESET;   //drive low to reset
  //P3.1 -   IOT_PROG_MODE
  P3DIR  &= ~IOT_PROG_MODE;      //input, 0
  //P3.2 -   IOT_LINK
  P3DIR  &= ~IOT_LINK;           //input
  //P3.3 -   IOT_PROG_SEL
  P3DIR  &= ~IOT_PROG_SEL;       //input
  //P3.4 -   L_REVERSE or SMCLK
  //in L_REVERSE mode, P3.4 is a GPIO pin
  //no additional configuration is required
  //in SMCLK mode, P3.4 is a function pin
  if(p4_select == USE_SMCLK) {
    P3DIR |= L_REVERSE;
    P3SEL1 |= L_REVERSE;
  }
  
  //in TB0 mode, these pins are connected directly to TB0.n (n=3..6)
  //this is used for PWM control of the motors
  if(p4_select == USE_TB0) {
    P3DIR |= L_REVERSE;
    P3SEL1 &= ~L_REVERSE;
    P3SEL0 |= L_REVERSE;
    
    P3DIR |= L_FORWARD;
    P3SEL1 &= ~L_FORWARD;
    P3SEL0 |= L_FORWARD;
    
    P3DIR |= R_REVERSE;
    P3SEL1 &= ~R_REVERSE;
    P3SEL0 |= R_REVERSE;
    
    P3DIR |= R_FORWARD;
    P3SEL1 &= ~R_FORWARD;
    P3SEL0 |= R_FORWARD;
  }
}

void Init_Port_4(void){
  //fortunately all of these pins are GPIO-OUT
  //which makes initialization EZ
  P4SEL0 = EMPTY;
  P4SEL1 = EMPTY;
  P4OUT  = DIR_ALL_OUT;
  P4DIR  = EMPTY;
  P4DIR  |=  UCB1_CS_LCD;
  P4OUT  &= ~UCB1_CS_LCD;       //chip select for LCD, low according to lCD infosheet
}

void Init_Port_5(void){
  // UCB1SIMO           (0x01)
  // OCB1SOMI           (0x02)
  // UCB1CLK            (0x04)
  // RESET_LCD          (0x08)
  // BUTTON2            (0x20)
  // BUTTON1            (0x40)
  // LCD_BACKLITE       (0x80)
  P5SEL1 = EMPTY; 
  P5SEL0 = EMPTY;
  P5DIR  = DIR_ALL_OUT;
  P5OUT  = EMPTY;
  P5IN   = EMPTY;
  //P5.0     UCB1SIMO, SEL0 = 1
  P5SEL0 |=  UCB1SIMO;
  //P5.1     UCB1SOMI, SEL0 = 1
  P5SEL0 |=  UCB1SOMI;
  //P5.2     UCB1SCK,  SEL0 = 1
  P5SEL0 |=  UCB1SCK;
  //P5.3     RESET_lCD, initially high
  P5OUT  |=  RESET_LCD;          //drive low to reset LCD
  //P5.4     P5_4, input for some reason
  //P5DIR  &= ~P5_4;
  //P5.5     BUTTON2, input, pull-UP
  P5DIR  &= ~BUTTON2; //input
  P5OUT  |=  BUTTON2; //configure resistor as pull-up
  P5REN  |=  BUTTON2; //enable resistor
  //P5.6     BUTTON1, input, pull-UP
  P5DIR  &= ~BUTTON1; //input
  P5OUT  |=  BUTTON1; //configure resistor as pull-up
  P5REN  |=  BUTTON1; //enable resistor
  //P5IES  |=  BUTTON1;
  //P5IFG  &= ~BUTTON1;
  //P5IE   |=  BUTTON1;
  //P5.7     LCD_BACKLITE, initially high
  P5OUT  &=  ~LCD_BACKLITE;       //high == on
  
  //BUTTON INTERRUPTS====================================================
  //We need to enable interrupts on a couple of pins, i.e. the buttons
  P5IV =  EMPTY;        //clear any outstanding flags
  P5IFG = EMPTY;        
  P5IES |= BUTTON1;    //edge select, should be falling edge
  P5IES |= BUTTON2;    //ince the buttons go low when pressed
  P5IE |= BUTTON1;      //enable the interrupts
  P5IE |= BUTTON2;      
  button1_debouncing = NO;
  button2_debouncing = NO;
  button1_debounce_count = EMPTY;
  button2_debounce_count = EMPTY;

}

void Init_Port_6(void){
  // These are all unused
  // if they are used later, I will add individual pin configs
  // otherwise that just seems like a waste of time
  // UCA3TXD         (0x01)
  // UCA3RXD         (0x02)
  // J1_5            (0x04)
  // MAG_INT         (0x08)
  // P6_4            (0x10)
  // P6_5            (0x20)
  // P6_6            (0x40)
  // P6_7            (0x80)
  P6SEL1 = EMPTY;
  P6SEL0 = EMPTY;
  P6DIR  = DIR_ALL_OUT;
  P6OUT  = EMPTY;
  
  //configure for UCA3
  P6SEL0 |= UCA3TXD;
  P6SEL1 &= ~UCA3TXD;
  P6SEL0 |= UCA3RXD;
  P6SEL1 &= ~UCA3RXD;
}

void Init_Port_7(void){
  // I2CSDA          (0x01)
  // I2CSCL          (0x02)
  // SD_DETECT       (0x04)
  // J4_36           (0x08)
  // P7_4            (0x10)
  // P7_5            (0x20)
  // P7_6            (0x40)
  // P7_7            (0x80)
  P7SEL1 = EMPTY;               // no need to change SEL bits
  P7SEL0 = EMPTY;               // since all are GPIO
  P7DIR  = DIR_ALL_OUT;         // SD_DETECT is the only input
  P7OUT  = EMPTY;               // no outputs here 
  //P7.2   SD_DETECT, input
  P7DIR |= SD_DETECT;
}

void Init_Port_8(void){
  //currently unused
  P8SEL1 = EMPTY;
  P8SEL0 = EMPTY;
  P8DIR  = DIR_ALL_OUT;
  P8OUT  = EMPTY;
}

void Init_Port_J(void){
  PJSEL1 = EMPTY;
  PJSEL0 = EMPTY;
  PJDIR  = DIR_ALL_OUT;
  PJOUT  = EMPTY;
  //PJ.0 - PJ.3 are all unused
  //PJ.4 is LFXIN, SEL0 = 1
  //LFXTBYPASS = 0 - assume LFXIN crystal mode (not bypass)
  PJSEL0 |= LFXIN; 
  //PJ.5 is LFXOUT SEL0 = 1
  PJSEL0 |= LFXOUT;
  //PJ.6 is HFXIN  SEL0 = 1
  //HFXTBYPASS = 0 - assume LFXIN crystal mode (not bypass)
  PJSEL0 |= HFXIN;
  //PJ.7 is HFXOUT SEL0 = 1
  PJSEL0 |= HFXIN;  
}

