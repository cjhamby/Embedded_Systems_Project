//==============================================================================
//      Chris Hamby Presents...
//
//      interrupts_serial.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      UCA0 is the PC communication channel - use it to communicate via putty and USB
//      you may need to find the proper port to connect: if so, use device manager
//      putty needs to have the same configuration as the car (i.e. baud)
//
//      UCA3 is the IOT communication channel - the configuration is correct
//      If you reconfigure things in the future, refer to the course docs
//
//      What happens in an ISR here?
//      If the receive interrupt receives a char:
//      it is written to the appropriate ring buffer
//      An index counter ensures the char is written in the correct position
//      All RX chars are also echoed directly to the UCA0TXBUF
//      Meaning that, if putty is open, we can see the received chars
//      They skip the ring buffer, so we don't have to worry about indexing issues

//      If the transmit interrupt is enabled:
//      All chars in the TX ring buffer are transmitted
//      A counter ensures we don't transmit too many chars
//      Note that setting TXBUF to EMPTY still transmits a NULL char, this is bad
//      Afterwards, the transmit interrupt is disabled again
//==============================================================================
#include "macros.h"
#include  "msp430.h"
#include  "functions.h"
#include <string.h>

char ISR_tempChar;
//============================================================================
//      UCA0 Serial Interrupt Vector - PC
//============================================================================
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
  unsigned int temp=EMPTY;
  switch(__even_in_range(UCA0IV, USCI_MAX_FLAG)){
  case USCI_NO_FLAG: break;
  
  //============================================================================
  case USCI_RX_FLAG:         //Received a character from the PC
    if(!PC_TX_Enable)        //the PC doesn't like to receive before transmitting
      PC_TX_Enable = YES;    //now we can transmit to the PC

    temp = UCA0_rx_ring_wr;     //where to write the char in the ring buffer
    UCA0_rx_ring_wr++;          //only change this variable here
    if(UCA0_rx_ring_wr >= SMALL_RING_SIZE)
      UCA0_rx_ring_wr = COUNT_RESET;
    
    ISR_tempChar = UCA0RXBUF;               //what to write into the ring buffer
    UCA0_Char_Rx[temp] = ISR_tempChar;      //do it
      
    UCA0TXBUF = ISR_tempChar;   //echo back to the terminal
    break;
    
    
  //============================================================================
  case USCI_TX_FLAG:              //Ready to transmit to the terminal
    if(!PC_TX_Enable)   break;    //PC transmit hasn't been enabled
    //Read a char from the TX ring buffer, and then transmit this character
    temp = UCA0_tx_ring_rd;
    UCA0_tx_ring_rd++;                       //ONLY EDIT THIS VARIABLE HERE
    if(UCA0_tx_ring_rd >= SMALL_RING_SIZE)   //keep it in bounds
      UCA0_tx_ring_rd = COUNT_RESET;
    
    UCA0_numToTransmit--;               //how many chars we have yet to transmit
    if(UCA0_numToTransmit == EMPTY){    //if there are no more chars 
      UCA0IE &= ~UCTXIE;                //disable further transmits 
    }
    ISR_tempChar = UCA0_Char_Tx[temp];
    UCA0TXBUF = ISR_tempChar;           //transmit the character

    break;
  default: break;
  }
}





//============================================================================
//      UCA3 Serial Interrupt Vector - IOT
//============================================================================
#pragma vector = USCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void) {
  unsigned int temp=EMPTY;
  switch(__even_in_range(UCA3IV, USCI_MAX_FLAG)){
  case USCI_NO_FLAG: 
    break;
  //===========================================================================
  case USCI_RX_FLAG: //the receive flag indicates a new char came in
    temp = UCA3_rx_ring_wr;    //where to write the char into the ring buffer
    ISR_tempChar = UCA3RXBUF;  //safety for volatile char

    if(store_wireless_info){   //store the network info for parsing later
      wireless_info[network_parse_index] = ISR_tempChar;
      network_parse_index++;            
    }
    else{
      UCA3_Char_Rx[temp] = ISR_tempChar;
      UCA3_rx_ring_wr++;
      if(UCA3_rx_ring_wr>=SMALL_RING_SIZE)
        UCA3_rx_ring_wr = COUNT_RESET;
    }

    if(PC_TX_Enable)               //echo character to the terminal
      UCA0TXBUF = ISR_tempChar;    //skip the ISR, go straight to the buffer
    break;
  //===========================================================================  
  case USCI_TX_FLAG:  //the transmit buffer is ready for new char   
    temp = UCA3_tx_ring_rd;         //volatile safety
    UCA3_tx_ring_rd++;
    if(UCA3_tx_ring_rd>=SMALL_RING_SIZE)
      UCA3_tx_ring_rd = COUNT_RESET;
    
    ISR_tempChar = UCA3_Char_Tx[temp];
    UCA3_numToTransmit--;               //how many chars we have yet to transmit
    
    if(UCA3_numToTransmit == EMPTY){     //if there are no more chars 
      UCA3IE &= ~UCTXIE;                //disable further transmits 
    }
    
    UCA3TXBUF = ISR_tempChar;           //transmit the char
    break;
  //===========================================================================
  default:              
    break;
  }
}