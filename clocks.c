//------------------------------------------------------------------------------
//
//  Description: This file contains the Clock Initialization
//
//  Jim Carlson
//  Jan 2016
//  Built with IAR Embedded Workbench Version: V7.3.1.3987 (6.40.1)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include  "msp430.h"
#include  "macros.h"


void Init_Clocks(void);

void Init_Clocks(void){
//------------------------------------------------------------------------------
// Clock Configurtaions
// This is the clock initialization for the program.
//
// Initial clock configuration, runs immediately after boot.
// Disables 1ms watchdog timer,
//      configure MCLK to 8MHz
//      configure SMCLK to 8MHz.
// Since the X1 oscillator is not currently connected,
// X1CLK is an unknown speed (probably ~10kHz).
//------------------------------------------------------------------------------
  WDTCTL = WDTPW | WDTHOLD;  // Disable watchdog
//  PJSEL0 |= XINR;
//  PJSEL0 |= XOUTR;

// Clocks:
// Clock System Control 0 Register
  CSCTL0 = CSKEY;            // Unlock register

  
  
// -----------------------------------------------------------------------------
// Clock System Control 1 Register - DCOCLK frequency selection
  CSCTL1 = CLEAR_REGISTER;   // Clear register
// Some of the values in CSCTL1 are Read Only, and always read as 0
// DCOFSEL_x, where x=0..6, frequencies between 1-8MHz
//  CSCTL1 = DCOFSEL_0;        // Set DCO setting for 1MHz
  CSCTL1 = DCOFSEL_6;        // Set DCO setting for 8MHz
//  CSCTL1 &= ~DCORSEL;        // DCO range select. For high-speed devices,
                             // this bit can be written by the user.
                             // For low-speed devices, it is always reset.

  
  
// -----------------------------------------------------------------------------
// Clock System Control 2 Register - sourcing options
  CSCTL2 = CLEAR_REGISTER;   // Clear register
  CSCTL2 |= SELA__LFXTCLK;   // Select (source for ACLK) = LFXTCLK
  CSCTL2 |= SELS__DCOCLK;    // Select (source for SMCLK) = DCOCLK
  CSCTL2 |= SELM__DCOCLK;    // Select (source for MCLK) = DCOCLK

  
  
  
// -----------------------------------------------------------------------------  
// Clock System Control 3 Register - division options
  CSCTL3 = CLEAR_REGISTER;   // Clear register
  CSCTL3 |= DIVA__1;         // set ACLK  divider /1
  CSCTL3 |= DIVS__1;         // set SMCLK divider /1
//  CSCTL3 |= DIVS__16;        // set SMCLK divider -> 8MHz/16 = 500 kHz
  CSCTL3 |= DIVM__1;         // set MCLK divider /1

  
  
// -----------------------------------------------------------------------------  
// Clock System Control 4 Register - HFXTCLK and LFXTCLK options, VLOCLK Enable
//  CSCTL4 = CLEAR_REGISTER;   // Clear register
  CSCTL4 &= ~LFXTOFF;        // Enable LFXT1

  do
  {
    CSCTL5 &= ~LFXTOFFG;     // Clear XT1 fault flag
    SFRIFG1 &= ~OFIFG;
  } while (SFRIFG1 & OFIFG); // Test oscillator fault flag

  
//  CSCTL0 = CSLOCK;           // Lock registers
  CSCTL0_H = CS_REG_LOCK;              // Lock CS registers



//------------------------------------------------------------------------------
}


