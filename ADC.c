//==============================================================================
//      Chris Hamby Presents...
//
//      ADC.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      This file handles the thumb wheel and detectors
//      Including calibration, emitter control, and displaying values
//
//      global functions
//              ADC_Process(void)
//              Show_Adc_Process(void)
//              Calibrate_Process(void)
//              Init_ADC(void)
//              Enable_Emitter(void)
//              Disable_Emitter(void)
//
//      local functions
//              Average_Detectors(void)
//              turn_Emitter_On(void)
//              turn_Emitter_Off(void)
//              toggle_Emitter(void)
//              Show_Adc_Setup(void)
//==============================================================================
#include "macros.h"
#include  "msp430.h"
#include  "functions.h"
#include <string.h>

int Average_Detectors(void);
void turn_Emitter_On(void);
void turn_Emitter_Off(void);
void toggle_Emitter(void);
void Show_Adc_Setup(void);
  
char ADC_oneTime = YES;                                 //run setup functions one time only
extern volatile int ADC_Thumb           = EMPTY;        //ADC values
extern volatile int ADC_Right_Detector  = EMPTY;        //directly from interrupt
extern volatile int ADC_Left_Detector   = EMPTY;

extern float black_threshold = DEFAULT_BLACK_THRESHOLD;   //value between black/grey
extern float white_threshold = DEFAULT_WHITE_THRESHOLD;   //value between grey/white
extern float on_threshold = DEFAULT_ON_THRESHOLD;         //was the emitter on for the reading?

char enabled_emitter = NO;       //is the emitter enabled?
char emitter_on = NO;            //is the emitter on?
char emitter_switched = NO;      //did the emitter just turn on/off?

//Calibration Measurements
char calibrate_color = CAL_OFF;     //the color we're reading in the calibration
unsigned int off_reading   = EMPTY;
unsigned int black_reading = EMPTY; 
unsigned int white_reading = EMPTY; 
unsigned int grey_reading  = EMPTY;

//global access functions
//these functions are remnants of the attempt at single-conversion mode
//there also used to be Enable/Disable Detector functions
void Enable_Emitter(void) {
  enabled_emitter = YES;
  emitter_switched = YES;
}
void Disable_Emitter(void) {
  enabled_emitter = NO;
  emitter_switched = YES;
}


//This function controls the emitter
//If another function wants to turn on the emitter, it should
//call the function Enable_Emitter()
void ADC_Process(void) {
  if(emitter_switched) {        //to avoid repeatedly assigning pin output
    emitter_switched = NO;
    if(enabled_emitter)
      turn_Emitter_On();
    else
      turn_Emitter_Off();
  }
}

//local access functions
void turn_Emitter_On(void){
  P8OUT |= IR_LED;
  emitter_on= YES;
}
void turn_Emitter_Off(void){
  P8OUT &= ~IR_LED;
  emitter_on = NO;
}
void toggle_Emitter(void){
  enabled_emitter = YES-enabled_emitter;
}


//clear the screen and label the ADC values
void Show_Adc_Setup(void) {
  clearDisplay();
  strcpy(display_line[DISPLAY_LINE_1], "     THUMB");
  strcpy(display_line[DISPLAY_LINE_2], "      LEFT");
  strcpy(display_line[DISPLAY_LINE_3], "     RIGHT");
  strcpy(display_line[DISPLAY_LINE_4], "b2 to Menu");
  delay_100ms(VHUNDRED_MS);
}

//Show the three ADCs on the display
//Press button 1 to turn on the emitter
void Show_Adc_Process(void) {
  if(ADC_oneTime) {     //run a setup function
    Show_Adc_Setup();   //that labels the values
    ADC_oneTime = NO;
  }
  showADC(ADC_Thumb,            DISPLAY_LINE_1);
  showADC(ADC_Left_Detector,    DISPLAY_LINE_2);
  showADC(ADC_Right_Detector,   DISPLAY_LINE_3);
  
  if(Check_Button_1())
    Enable_Emitter();
  if(Check_Button_2()){
    endEvent();
    ADC_oneTime = YES;
  }
  
}


//this function returns the average value between the two detectors
int Average_Detectors(void) {
  int result = ADC_Left_Detector;       //the ADC values are volatile
  result += ADC_Right_Detector;         //so only one is used per line
  result /= NUM_DETECTORS;
  return result;
}

//this function assigns calibration values so we can accurately read colors
void Calibrate_Process(void) {
  //live update the detector values onto the screen
  switch(calibrate_color) {
    case CAL_OFF:
      strcpy(display_line[DISPLAY_LINE_1], "Emit Off  ");
      strcpy(display_line[DISPLAY_LINE_3], "         W");
      strcpy(display_line[DISPLAY_LINE_4], "         B");
      showADC(ADC_Right_Detector, DISPLAY_LINE_2);
      break;
    case CAL_WHITE:
      Enable_Emitter();
      showADC(ADC_Right_Detector, DISPLAY_LINE_3);
      break;
    case CAL_BLACK:
      //Enable_Emitter();
      showADC(ADC_Right_Detector, DISPLAY_LINE_4);
      break;
    case CAL_LINE:
      //Enable_Emitter();
      showADC(ADC_Right_Detector, DISPLAY_LINE_2);
      break;
    case DONE_CALIBRATING:
      Disable_Emitter();
      showADC((int)white_threshold, DISPLAY_LINE_2);
      showADC((int)black_threshold, DISPLAY_LINE_3);
      showADC((int)on_threshold,   DISPLAY_LINE_4);
      break;
    default:break;
  }
      
  //BUTTON 2 RECORDS THE CALIBRATION VALUE =====================================
  if(Check_Button_2()) {
    switch(calibrate_color) {                   //what color are we recording?
      case CAL_OFF:                             //EMITTER OFF=================== 
        off_reading = Average_Detectors();                   //record the value
        calibrate_color = CAL_WHITE;                         //next color to calibrate
        strcpy(display_line[DISPLAY_LINE_1], "White Next");
        showADC(off_reading, DISPLAY_LINE_2);                //show reading on LCD
        break;
      case CAL_WHITE:                           //EMITTER ON WHITE==============
        white_reading = Average_Detectors();                 //record the value
        calibrate_color = CAL_BLACK;                         //next color to calibrate
        strcpy(display_line[DISPLAY_LINE_1], "Black Next");
        showADC(white_reading, DISPLAY_LINE_3);              //show reading on LCD
        break;
      case CAL_BLACK:                           //EMITTER ON BLACK==============
        black_reading = Average_Detectors();                 //record the value
        calibrate_color = CAL_LINE;                          //next color to calibrate
        strcpy(display_line[DISPLAY_LINE_1], "Line Next");
        break;
      case CAL_LINE:                            //EMITTER ON LINE===============
        grey_reading = ADC_Right_Detector;      //don't average the values for this reading
        //finished calibrating - time to calculate the color thresholds
        black_threshold = grey_reading + BLACK_MARGIN;          //black - grey transition
        white_threshold = grey_reading - WHITE_MARGIN;          //grey - white transition
        on_threshold = (black_reading + off_reading)/AVERAGE_2; //for knowing if the emitter is actually on
        
        Disable_Emitter();      //no need for the emitter any more
        clearDisplay();         
        strcpy(display_line[DISPLAY_LINE_1], "B2 to Menu");
        calibrate_color = DONE_CALIBRATING;
        break;
      case DONE_CALIBRATING:                    //FINISHED CALIBRATING==========
        calibrate_color = CAL_OFF;
        endEvent();
        break;
    }  
  }
}

//this function prepares the ADCs for use
void Init_ADC(void){
// Configure ADC12 - Copied from Wolfware
// ADC10CTL0 Register Description
  ADC12CTL0 = RESET_STATE;
  ADC12CTL0 |= ADC12SHT0_2;     // Sample and Hold time, ADC12MEM8-MEM23,               16 ADC clocks
  ADC12CTL0 |= ADC12SHT1_2;     // Sample and Hold time, ADC12MEM0-MEM7, MEM24-MEM31,   16 ADC clocks
  ADC12CTL0 |= ADC12MSC;        // First rising edge SHI signal triggers sampling timer
  ADC12CTL0 |= ADC12ON;         // ADC12 on

// ADC10CTL1 Register Description
  ADC12CTL1 = RESET_STATE;
  ADC12CTL1 |= ADC12PDIV_0;     // Predivide ADC12_B clock source by 1
  ADC12CTL1 |= ADC12SHS_0;      // sample-and-hold source ADC12SC
  ADC12CTL1 |= ADC12SHP;        // SAMPCON signal is sourced from the sampling timer.
  ADC12CTL1 |= ADC12ISSH_0;     // sample-input signal is not inverted
  ADC12CTL1 |= ADC12DIV_0;      // clock divider, /1
  ADC12CTL1 |= ADC12SSEL0;      // clock source select : ADC12OSC (MODOSC)
  //ADC12CTL1 |= ADC12CONSEQ_1;   // ******conversion sequence mode select: Single-sequence-of-channels*******
  ADC12CTL1 |= ADC12CONSEQ_3;   // conversion sequence mode select: Repeat-sequence-of-channels

// ADC12CTL2 Register Description
  ADC12CTL2 = RESET_STATE;
  //ADC12CTL2 |= ADC12RES_0;      //8 Bit resolution results, 10 clock cycle conversion
  ADC12CTL2 |= ADC12RES_2;      // 12-bit conversion results / 14 clock cycle conversion
  ADC12CTL2 |= ADC12DF_0;       // Binary unsigned
  ADC12CTL2 |= ADC12PWRMD_0;    // Regular power mode where sample rate is not restricted

// ADC12CTL3 Register Description
  ADC12CTL3 = RESET_STATE;
  ADC12CTL3 |= ADC12ICH3MAP_0;  // external pin is selected for ADC input channel A26
  ADC12CTL3 |= ADC12ICH2MAP_0;  // external pin is selected for ADC input channel A27
  ADC12CTL3 |= ADC12ICH1MAP_0;  // external pin is selected for ADC input channel A28
  ADC12CTL3 |= ADC12ICH0MAP_0;  // external pin is selected for ADC input channel A29
  ADC12CTL3 |= ADC12TCMAP_1;    // ADC internal temperature sensor ADC input channel A30
  ADC12CTL3 |= ADC12BATMAP_1;   // ADC internal 1/2 x AVCC is ADC input channel A31
  ADC12CTL3 |= ADC12CSTARTADD_0; // conversion start address: 0h to 1Fh, corresponding to ADC12MEM0 to ADC12MEM31

// ADC10MCTL0 Register Description
  ADC12MCTL0 = RESET_STATE;
  ADC12MCTL0 |= ADC12WINC_0;    // Comparator window disabled
  ADC12MCTL0 |= ADC12DIF_0;     // Single-ended mode enabled
  ADC12MCTL0 |= ADC12VRSEL_0;   // VR+ = AVCC, VR- = AVSS
  ADC12MCTL0 |= ADC12INCH_2;    // channel = A2 Thumb Wheel

  ADC12MCTL1 = RESET_STATE;
  ADC12MCTL1 |= ADC12WINC_0;  // Comparator window disabled
  ADC12MCTL1 |= ADC12DIF_0;   // Single-ended mode enabled
  ADC12MCTL1 |= ADC12VRSEL_0; // VR+ = AVCC, VR- = AVSS
  ADC12MCTL1 |= ADC12INCH_5;  // channel = A5 Left

  ADC12MCTL2 = RESET_STATE;
  ADC12MCTL2 |= ADC12WINC_0;  // Comparator window disabled
  ADC12MCTL2 |= ADC12DIF_0;   // Single-ended mode enabled
  ADC12MCTL2 |= ADC12VRSEL_0; // VR+ = AVCC, VR- = AVSS
  ADC12MCTL2 |= ADC12INCH_4;  // channel = A4 Right
  ADC12MCTL2 |= ADC12EOS;               //from the slides

  ADC12MCTL3 = RESET_STATE;
  ADC12MCTL3 |= ADC12WINC_0;  // Comparator window disabled
  ADC12MCTL3 |= ADC12DIF_0;   // Single-ended mode enabled
  ADC12MCTL3 |= ADC12VRSEL_0; // VR+ = AVCC, VR- = AVSS
  ADC12MCTL3 |= ADC12INCH_30; // Temp sensor

  ADC12MCTL4 = RESET_STATE;
  ADC12MCTL4 |= ADC12WINC_0;  // Comparator window disabled
  ADC12MCTL4 |= ADC12DIF_0;   // Single-ended mode enabled
  ADC12MCTL4 |= ADC12VRSEL_0; // VR+ = AVCC, VR- = AVSS
  ADC12MCTL4 |= ADC12INCH_31; // Battery voltage monitor
  ADC12MCTL4 |= ADC12EOS;     // End of Sequence

// ADC12IER0-2 Register Descriptions
  ADC12IER0 = RESET_STATE;    // Interrupts for channels  0 - 15
  ADC12IER1 = RESET_STATE;    // Interrupts for channels 16 - 31
  ADC12IER2 = RESET_STATE;    // Interrupts for ADC12RDYIE ADC12TOVIE ADC12OVIE
                              // ADC12HIIE ADC12LOIE ADC12INIE
							  
//  ADC12IER0 |= ADC12IE4;      // Generate Interrupt for MEM2 ADC Data load//
  ADC12IER0 |= ADC12IE2;    // Generate Interrupt for MEM2 ADC Data load
//  ADC12IER0 |= ADC12IE0;    // Enable ADC conv complete interrupt

  ADC12CTL0 |= ADC12ENC;     // Start conversion
  ADC12CTL0 |= ADC12SC;      // Start sampling
//------------------------------------------------------------------------------
}

