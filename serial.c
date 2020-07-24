//==============================================================================
//      Chris Hamby Presents...
//
//      serial.c
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      handles all background serial communication stuff
//      all RX and TX chars are stored in ring buffers
//      functions exist to put chars on/get chars from these buffers
//      
//      To limit ambiguity, never interact with the ring buffers or ISRs outside
//      of these functions.  Important index variables are in place and rely on them.
//      
//------------------------------------------------------------
//COMMAND CHARS           COMMAND
//------------------------------------------------------------
//      ^               test response
//      K               personalized greeting
//      C               clear LCD display
//      L               toggle LCD backlight
//
//      S               slow baud - 9600
//      F               fast baud - 115200
//
//      H               use home wifi
//      U               use UNCA wifi
//      I               reset/enable IOT
//      R               reset IOT ring buffers
//      T               test IOT module (USB only)
//      W               show wireless info
//
//      f<n>            forward for   <n>*100 ms
//      b<n>            backwards for <n>*100 ms
//      l<n>            left for      <n>*100 ms
//      r<n>            right for     <n>*100 ms
//      Z               intercept and follow line
//      B               turn motors off
//
//
//      global functions:
//              Init_Serial(void)
//              IOT_Enable_Process(void)
//              UART0_Setup(void)
//              UART3_Setup(void)
//              get_UCA3_RX(void)
//              IOT_Process(void)
//
//      local functions:
//              Project8_Process(void)
//              IOT_Communication(void) 
//
//--------------Interact with Baud Rate-----------------------------------------
//              setBaud_UCA0(int)
//              setBaud_UCA3(int)
//              toggle_Baud_Rate(void)
//
//--------------Interact with Command-------------------------------------------
//              Execute_Command(void)             
//              Execute_Command_FRAM(void)  
//              get_Pin_From_Command_Char(void)
//              get_Time_From_Command_Char(void)  
//              getWirelessInfo(void)
//              showWirelessInfo(void)
//
//--------------Interact with Ring Buffers--------------------------------------
//              get_UCA0_RX(void)                 
//              get_UCA3_RX(void)  
//              StartTransmit_UCA0(void)          
//              StartTransmit_UCA3(void)          
//              queue_TX_Char_UCA0(char c)        
//              queue_TX_Char_UCA3(char c)        
//              transmitString_UCA0(char* str)    
//              transmitString_UCA3(char* str) 
//              clear_UCA0_Ring_Buffers(void)
//              clear_UCA3_Ring_Buffers(void)
//
//==============================================================================

#include "macros.h"
#include  "msp430.h"
#include  "functions.h"
#include <string.h>

volatile char UCA0_Char_Rx[SMALL_RING_SIZE];    //the ring buffers
volatile char UCA0_Char_Tx[SMALL_RING_SIZE];    
volatile char UCA3_Char_Rx[SMALL_RING_SIZE];
volatile char UCA3_Char_Tx[SMALL_RING_SIZE];
                                                //Indexes (Indeces?)
volatile unsigned int UCA0_rx_ring_wr = NO;     //where the newest received char goes
volatile unsigned int UCA0_rx_ring_rd = NO;     //where the char we read is at
volatile unsigned int UCA0_tx_ring_wr = NO;     //where the char we submit to tx goes
volatile unsigned int UCA0_tx_ring_rd = NO;     //the char that is currently being tx'ed

volatile unsigned int UCA3_rx_ring_wr = NO;     //UCA3 indexes
volatile unsigned int UCA3_rx_ring_rd = NO;     
volatile unsigned int UCA3_tx_ring_wr = NO;
volatile unsigned int UCA3_tx_ring_rd = NO;

// Interact with the ring buffers
void clear_UCA0_Ring_Buffers(void);     // empty the UCA0 ring buffers
void clear_UCA3_Ring_Buffers(void);     // empty the UCA3 ring buffers
char get_UCA0_RX(void);                 // get a char from the ring buffer
char get_UCA3_RX(void);                 // 
void StartTransmit_UCA0(void);          // transmit a char/command to the PC
void StartTransmit_UCA3(void);          // transmit a char/command to the IOT module
void queue_TX_Char_UCA0(char c);        // queue characters to be transmitted
void queue_TX_Char_UCA3(char c);        // a counter increments to know how many chars to transmit
void transmitString_UCA0(char* str);    // queue and transmit a string
void transmitString_UCA3(char* str);    //
volatile int UCA0_numToTransmit = COUNT_RESET;  //increment when char added to tx queue
volatile int UCA3_numToTransmit = COUNT_RESET;  //decrement in ISR

volatile char test_command[NUM_DISPLAY_CHARS] = "NCSU  #1  ";
volatile char PC_TX_Enable = NO;       //Don't transmit to PC until a char is received from PC



// Baud Rate and Baud Rate Accessories =========================================
void toggle_Baud_Rate(void);            //scroll through possible baud rates
void setBaud_UCA0(long baud);           //make sure to use predefined baud rates
void setBaud_UCA3(long baud);           //(9600, 115200, 460800)
unsigned long current_Baud_0 = EMPTY;
unsigned long current_Baud_3 = EMPTY;
  

// Project 8 Stuffs ============================================================
void Project8_Process(void);            // communicate with a lab bot
char project8_writing = NO;
char project8_string[NUM_DISPLAY_CHARS] = "";
char project8_received = NO;
unsigned int project8_index = EMPTY;


// Command-Related Stuffs ======================================================
// When we connect to the port via TCP, every command we send is framed by the IOT module
//                              <ESC>S<n>COMMAND<ESC>E
//
char Command_Char[COMMAND_MAX_LENGTH] = "";     //the command being entered
int command_wr = COUNT_RESET;                   //where to write to the command char
int get_Pin_From_Command_Char(void);            //parse for the pin, as a security measure
int get_Time_From_Command_Char(void);           //parse for the time to use with timed movement functions

void IOT_Communication(void);                   // handles communication between FRAM and IOT
void Execute_Command(void);                     // routes a command to its appropriate recipient - FRAM or IOT
void Execute_Command_FRAM(void);                // deciphers and executes a FRAM command

char TCP_escape_found = NO;             //yes means the <ESC> char has been found - the next char should be S or E
char TCP_store_command = NO;            //yes means we're writing the command to Command_Char
char IOT_Enable_OneTime = NO;           //yes means the IOT module port has been enabled - we can now connect via TCP
char IOT_Setup_oneTime = YES;           //yes means the IOT information needs to be reset (index vars and ring buffers cleared)



//WIFI INFO CAPTURE ============================================================
void getWirelessInfo(void);
void showWirelessInfo(void);

extern volatile char store_wireless_info = NO;  //if YES, the incoming string contains network info
char wireless_info[WIRELESS_LENGTH] = "";       //holds the network info to parse
char my_SSID[NUM_DISPLAY_CHARS] = "";           //value obtained by parsing
char my_IP[IP_LENGTH] = "";                     //value obtained by parsing
char IP_top_half[NUM_DISPLAY_CHARS];            //string format for IP display
char IP_bottom_half[NUM_DISPLAY_CHARS];         //string format for IP bottom half
const char SSID_Identifier[SSID_IDENTITY_LENGTH] = " SSID=\"";  //what to parse for
const char IP_Identifier[IP_IDENTITY_LENGTH] = "IP addr=";      //what to parse for

unsigned volatile int network_parse_index = COUNT_RESET;

//==============================================================================
//-------------
//   -------------
//      -------------
//         -------------
//            -------------
//               -------------                                                 -
//                  -------------                                           ----
//                     -------------                                     -------
//                        -------------                               ----------
//                           -------------                         -------------
//              _____      _____-----_____---    __     ____    __------------
//            /   _  |   /  ___/   / ___ |-----/  /   /    |  /  /---------  
//           /  /_/  /  /  /      / / -|_|----/  /-  /  /  | /  /------- 
//          /   _  /   /  /__    / /  ____---/  /---/  /|  |/  /-----
//         /  /  \ \  /   __/   | |  |   |  /  /---/  /-|  |  /---
//        /  /__/  / /  /__     \ \___| |  /  /---/  /--|    /-
//       /_______/  /_____/      \_____/  /__/   /__/---\___/-- 
//                                           ----------------------
//                                        -------------  -------------
//                                     -------------        -------------
//                                  -------------              -------------
//                               -------------                    ------------- 
//                            -------------                          ------------
//==============================================================================

void UART0_Setup(void) {
  UCA0IE |= UCRXIE;                //enable the receive interrupt
  clear_UCA0_Ring_Buffers();
}

void UART3_Setup(void) {
  UCA3IE |= UCRXIE;                //enable the receive interrupt
  clear_UCA3_Ring_Buffers();
}

void clear_UCA0_Ring_Buffers(void){
  int i;
  for(i=EMPTY; i<SMALL_RING_SIZE; i++){
    UCA0_Char_Rx[i] = EMPTY;
    UCA0_Char_Tx[i] = EMPTY;
  }
  UCA0_rx_ring_wr = COUNT_RESET;  //index for received characters
  UCA0_rx_ring_rd = COUNT_RESET;  
  UCA0_tx_ring_wr = COUNT_RESET;  //index for received characters
  UCA0_tx_ring_rd = COUNT_RESET;  
}

void clear_UCA3_Ring_Buffers(void){
  int i;
  for(i=EMPTY; i<SMALL_RING_SIZE; i++){
    UCA3_Char_Rx[i] = EMPTY;
    UCA3_Char_Tx[i] = EMPTY;
  }
  UCA3_rx_ring_wr = COUNT_RESET;
  UCA3_rx_ring_rd = COUNT_RESET;
  UCA3_tx_ring_wr = COUNT_RESET;
  UCA3_tx_ring_rd = COUNT_RESET;
}


//==============================================================================
//gets a character and then increases the read index
//clears the ring buffer behind it
char get_UCA0_RX(void){
  char myChar = UCA0_Char_Rx[UCA0_rx_ring_rd];  //the char we pull
  if(myChar == EMPTY)   return EMPTY;           //don't increment past the write index
  UCA0_Char_Rx[UCA0_rx_ring_rd] = EMPTY;        //clear for the next go-around
  UCA0_rx_ring_rd++;                            //next index to read
  if(UCA0_rx_ring_rd >= SMALL_RING_SIZE)        //keep index in bounds
    UCA0_rx_ring_rd = COUNT_RESET;
  return myChar;
}

char get_UCA3_RX(void){
  char myChar = UCA3_Char_Rx[UCA3_rx_ring_rd];  //the char we pull
  if(myChar == EMPTY)   return EMPTY;
  UCA3_Char_Rx[UCA3_rx_ring_rd] = EMPTY;
  UCA3_rx_ring_rd++;
  if(UCA3_rx_ring_rd >= SMALL_RING_SIZE)
    UCA3_rx_ring_rd = COUNT_RESET;
  return myChar;
}

//==============================================================================
//these functions handle putting chars onto the TX ring buffers
//chars that are directly written to TXBUF in an interrupt bypass this
void queue_TX_Char_UCA0(char c){
  UCA0_Char_Tx[UCA0_tx_ring_wr] = c;            //copy to the queue
  UCA0_numToTransmit++;                         //number of chars in queue
  UCA0_tx_ring_wr++;                            //index of next item in queue
  if(UCA0_tx_ring_wr >= SMALL_RING_SIZE){       //keep index in bounds
    UCA0_tx_ring_wr = COUNT_RESET;
  }
}

void queue_TX_Char_UCA3(char c){
  UCA3_Char_Tx[UCA3_tx_ring_wr] = c;
  UCA3_numToTransmit++;
  UCA3_tx_ring_wr++;
  if(UCA3_tx_ring_wr >= SMALL_RING_SIZE){
    UCA3_tx_ring_wr = COUNT_RESET;
  }
}

//==============================================================================
//basically a loop for transmitting strings using the above functions
void transmitString_UCA0(char* str){
  int i;
  int len = strlen(str);        //doesn't include terminating null char
  for(i=EMPTY; i<len; i++){
    if(str[i] == EMPTY) break;
    queue_TX_Char_UCA0(str[i]);
  }
  StartTransmit_UCA0();         //begin transmitting
}

void transmitString_UCA3(char* str){
  int i;
  int len = strlen(str);        //doesn't include terminating null char
  for(i=EMPTY; i<len; i++){
    if(str[i] == EMPTY) break;
    queue_TX_Char_UCA3(str[i]);
  }
  StartTransmit_UCA3();
}

//==============================================================================
//begin transmission, captain
void StartTransmit_UCA0(void){
  UCA0IFG |= UCTXIFG;   //set the interrupt flag
  UCA0IE |= UCTXIE;     //enable the interrupt
}

void StartTransmit_UCA3(void){    
  UCA3IFG |= UCTXIFG;   //set the interrupt flag
  UCA3IE |= UCTXIE;     //enable the interrupt
}




//==============================================================================
//              Serial Transmission Project 9 ==================================
//==============================================================================
//Communication via IOT (UCA3) port
//configure UCA0 to 115200 baud, no parity, 8 bits, 1 stop
//check device manager to find MSP430 port
//configure putty to use this port
//      same baud, data/stop bits, no hardware flow control
//      don't configure local echo
//      don't append additional characters
//test comms with the PC

void IOT_Process(void) {
  if(IOT_Setup_oneTime){        
    clearDisplay();
    UART0_Setup();
    UART3_Setup();
    IOT_Setup_oneTime = NO;
  }
  //Project8_Process();
  IOT_Communication();
  
  if(Check_Button_1()){
    IOT_Setup_oneTime = YES;     //reset things
  }
  if(Check_Button_2()){          //return to the menu
    endEvent();
    IOT_Setup_oneTime = YES;
  }
}



void Project8_Process(void){
  char UCA0_Char_In = get_UCA0_RX();    //echo chars from UCA0 to UCA3
  if(UCA0_Char_In != EMPTY){
    queue_TX_Char_UCA3(UCA0_Char_In);   //transmit this char back to UCA3
    StartTransmit_UCA3();               //do it
  }
  char UCA3_Char_In = get_UCA3_RX();    //char incoming from UCA3 - lab bot
  if(UCA3_Char_In != EMPTY){
    switch (UCA3_Char_In){
    case (NEW_LINE_CHAR):
    case (RETURN_CHAR):         //end of transmission
      if(project8_writing)     
        project8_received = YES;
      project8_writing = NO;
      break;
    case ('$'):                 //beginning of rx
      project8_writing = YES;
      project8_index = COUNT_RESET;
      break;
    }
    project8_string[project8_index] = UCA3_Char_In;
    project8_index++;
    if(project8_index >= NUM_DISPLAY_CHARS)
      project8_index--;         //keep it in bounds
    
  }
  if(!project8_received){
    if(Check_Button_1())
      IOT_Setup_oneTime = YES;
  }
  if(project8_received){
    if(Check_Button_1()){
      UCA3_tx_ring_wr = COUNT_RESET;    //NECESSARY?
      UCA3_tx_ring_rd = COUNT_RESET;    //?
      transmitString_UCA3(project8_string);
      project8_received = NO;
      project8_index = COUNT_RESET;
      strcpy(display_line[DISPLAY_LINE_1], " Transmit ");
      strcpy(display_line[DISPLAY_LINE_2], project8_string);
      strcpy(display_line[DISPLAY_LINE_4], "          ");
    }
    else{
      strcpy(display_line[DISPLAY_LINE_1], " Received ");
      strcpy(display_line[DISPLAY_LINE_4], project8_string);
    }
  }
  if(Check_Button_2()){
    toggle_Baud_Rate();
  }
}

//==============================================================================
//              SERIAL COMM PROCESS ============================================
//==============================================================================
void IOT_Communication(void){
  int i;
  char UCA0_Char_In = get_UCA0_RX();    //bring in one char each loop
  char UCA3_Char_In = get_UCA3_RX();    //so long as there is a char
  //============================================================================
  //    UCA0 RX - messages from terminal
  //============================================================================
  if(UCA0_Char_In){                             //automatically write to Command_Char
    Command_Char[command_wr] = UCA0_Char_In;    //Store incoming chars into a command string
    command_wr++;
    if(UCA0_Char_In == RETURN_CHAR)             //The user hit Enter
      Execute_Command();                        //Time to do something with the command
  }
  //============================================================================
  //    UCA3 RX - messages from IOT module
  //============================================================================
  if(UCA3_Char_In){
    switch(UCA3_Char_In){     //commands in the form of <ESC>S<n>...<ESC>E
    case (TCP_ESCAPE_CHAR):   //indicates a frame char for the command
        TCP_escape_found = YES;         
      break;
    case ('S'):
        if(TCP_escape_found){           //the 'S' is part of an escape char
          TCP_store_command = YES;      //marks the beginning of a command
          command_wr = COUNT_RESET;     //start writing the command from the beginning
          TCP_escape_found = NO;
        }
        else if(TCP_store_command){     //the 'S' is not part of an escape char
          Command_Char[command_wr] = UCA3_Char_In;      //and can be used in commands
          command_wr++;
        }
      break;
    case ('E'):
        if(TCP_escape_found){           //the 'E' is part of an escape char
          TCP_escape_found = NO;        //marking the end of the command
          TCP_store_command = NO;       //we are finished storing the command
          for(i=COUNT_RESET; i<command_wr; i++)   //we don't need the character <n>
            Command_Char[i] = Command_Char[i+1];  //shift the string to ignore <n>
          Execute_Command();
        }
        else if(TCP_store_command){     //the 'E' is not part of an escape char
          Command_Char[command_wr] = UCA3_Char_In;      //and can be used in commands
          command_wr++;
        }
      break;
      default:
        if(TCP_store_command){          //the char is part of the command
          Command_Char[command_wr] = UCA3_Char_In;  //store it into the command
          command_wr++;                             //index for the next character
        }
        TCP_escape_found = NO;
        break;
    }
  }
}


//After we get a command, we have to execute it
void Execute_Command(void){
  int i;
  //we have to verify the pin as per instructions
  strcpy(display_line[DISPLAY_LINE_1], Command_Char);
  int my_pin = get_Pin_From_Command_Char();
  if(my_pin == COMMAND_PIN){
    if(Command_Char[COMMAND_DIRECTION_INDEX] == '^')    //this is the pathway decider
      Execute_Command_FRAM();    //the command is meant for the MSP430
    //  UNCOMMENT AND/OR WORK THIS OUT IF YOU WANT 
    //  TO CONNECT AND TYPE COMMANDS DIRECTLY TO THE IOT MODULE
    //  
    //  TO SEND COMMANDS TO THE IOT MODULE OTHERWISE
    //  USE TransmitString FUNCTIONS
    //  else
//      transmitString_UCA3(Command_Char);  //the command is meant for the IOT Module
  }

  for(i=EMPTY; i<COMMAND_MAX_LENGTH; i++)       //clear the command string
    Command_Char[i] = EMPTY;
  command_wr = COUNT_RESET;
}




//==============================================================================
//                  EXECUTE FRAM COMMAND
//==============================================================================
//for my own sake, try to keep the commands to a single letter
//timed movement functions require a number
//------------------------------------------------------------
//COMMAND CHARS           COMMAND
//------------------------------------------------------------
//      ^               test response
//      K               personalized greeting
//      C               clear LCD display
//      L               toggle LCD backlight
//
//      S               slow baud - 9600
//      F               fast baud - 115200
//
//      H               use home wifi
//      U               use UNCA wifi
//      I               reset/enable IOT
//      R               reset IOT ring buffers
//      T               test IOT module (USB only)
//      W               show wireless info
//
//      f<n>            forward for   <n>*100 ms
//      b<n>            backwards for <n>*100 ms
//      l<n>            left for      <n>*100 ms
//      r<n>            right for     <n>*100 ms
//      Z               intercept and follow line
//      B               turn motors off

void Execute_Command_FRAM(void){
  char CommChar1 = Command_Char[COMMAND_LETTER_INDEX];  //for one-character commands
    switch(CommChar1){
      case '^':         
        strcpy(display_line[DISPLAY_LINE_3], "Hey World!");
        break;
      case 'B':
        Motors_Off();
        break;
      case 'C':         
        clearDisplay();
        break;
      case 'F':
        setBaud_UCA3(BAUD115200);
        strcpy(display_line[DISPLAY_LINE_3], "UCA3 BAUD ");
        strcpy(display_line[DISPLAY_LINE_4], "  115200  ");
        break;
      case 'f':
        Forward_Timed(get_Time_From_Command_Char());
        break;
      case 'r':
        Right_Timed(get_Time_From_Command_Char());
        break;
      case 'l':
        Left_Timed(get_Time_From_Command_Char());
        break;
      case 'b':
        Reverse_Timed(get_Time_From_Command_Char());
        break;
      case 'H':
        transmitString_UCA3("AT&Y1\r\n");
        delay_100ms(ONE_SECOND);
        transmitString_UCA3("AT+RESET=1\r\n");
        delay_100ms(ONE_SECOND);
        break;
      case 'I':
        P3OUT &= ~IOT_RESET;
        delay_100ms(TWOHUNDRED_MS);     //delay for between 100-200 ms
        P3OUT |= IOT_RESET;
        strcpy(display_line[DISPLAY_LINE_4], "reset-ed  ");
        break;
      case 'K':
        strcpy(display_line[DISPLAY_LINE_3], "I <3 KT   ");
        strcpy(display_line[DISPLAY_LINE_4], "x 99999999");
        break;
      case 'L':
        P5OUT ^= LCD_BACKLITE;
        break;
      case 'R':
        IOT_Setup_oneTime=YES;
        break;
      case 'S':
        setBaud_UCA3(BAUD9600);
        strcpy(display_line[DISPLAY_LINE_3], "UCA3 BAUD ");
        strcpy(display_line[DISPLAY_LINE_4], "   9600   ");   
        break;
      case 'T':
        transmitString_UCA3("AT\r\n");
        break; 
      case 'U':
        transmitString_UCA3("AT&Y0\r\n");
        delay_100ms(ONE_SECOND);
        transmitString_UCA3("AT+RESET=1\r\n");
        delay_100ms(ONE_SECOND);
        break;
      case 'W':
        showWirelessInfo();
        break;
      case 'Z':
        event = FIND_LINE;
        break;
  }
}

//if the pin is incorrect, don't run the command
int get_Pin_From_Command_Char(void){
  int i = COMMAND_PIN_INDEX;
  int parse_num = EMPTY;
  while(Command_Char[i] != '^'){
    parse_num*=MOVE_UP_A_TENS_PLACE;
    parse_num+=(Command_Char[i]-MAKE_A_CHAR);
    i++;
    if(i==COMMAND_DIRECTION_INDEX){
      break;
    }
  }
  return parse_num;
}

//get the time to run a timed movement function for
//command comes in the form <pin><^><letter><number>
int get_Time_From_Command_Char(void){
  int i = COMMAND_TIME_INDEX;   //where to begin looking for the time
  int parse_num = EMPTY;        //the time value thus far
  while(Command_Char[i] != RETURN_CHAR && Command_Char[i] != TCP_ESCAPE_CHAR && Command_Char[i] != EMPTY){        //keep going until the number ends
    parse_num*=MOVE_UP_A_TENS_PLACE;            //what a great macro!
    parse_num+=(Command_Char[i]-MAKE_A_CHAR);   //add it to the total time value
    i++;
  }
  return parse_num;
}



//==============================================================================
//              IOT / TCP Communication Enable
//==============================================================================
//the IOT is enabled before the OS begins
//this function enables the TCP communications so we can send it commands via wifi
void IOT_Enable_Process(void){
  if(!IOT_Enable_OneTime){      //only enable the port one time
    IOT_Enable_OneTime = YES; 
    getWirelessInfo();    //connect and obtain an IP address
    showWirelessInfo();   //show relevant data 
    transmitString_UCA3("AT+NSTCP=4166,1\r");   //enable port 4166
  }
  
  if(Check_Button_2()){
    endEvent();
    IOT_Enable_OneTime = NO;
  }
}



void getWirelessInfo(void){
  int i;
  int bottom_half_index = COUNT_RESET;
  char* SSID_location = NULL;
  char* IP_location = NULL;
  int dot_counter = COUNT_RESET;
  network_parse_index = COUNT_RESET;           
  store_wireless_info = YES;            //tells the ISR to save the entire response
  transmitString_UCA3("AT+NSTAT=?\r");  //send command to IOT module, retrieves network info

  //next, we should receive a response from the IOT Module
  //the ISR should store the entire response in a BOS (Big Ol' String)          
  delay_100ms(ONE_SECOND);      //wait a second
  store_wireless_info = NO;     //now we (should) have it
  
  SSID_location = strstr(wireless_info, SSID_Identifier);   //pointer to where SSID identifier begins
  SSID_location += (SSID_IDENTITY_LENGTH - NEXT_TO_LAST);   //offset the identifier size, to get to the data
  IP_location = strstr(wireless_info, IP_Identifier);
  IP_location += (IP_IDENTITY_LENGTH - NEXT_TO_LAST);
  
  for(i=COUNT_RESET; i<NUM_DISPLAY_CHARS; i++){   //store the SSID to a string
    if (*(SSID_location+i) == '\"')               //the SSID data ends with a quotation mark
      break;
    my_SSID[i] = *(SSID_location+i);
  }
  
  for(i=COUNT_RESET; i<IP_LENGTH; i++){           //store the IP into a single string
    my_IP[i] = *(IP_location+i);
    if(my_IP[i] == ' ')
      break;
  }

  for(i=COUNT_RESET; i<NUM_DISPLAY_CHARS; i++){   //break the IP to two strings
    if(my_IP[i] == '.') {
      dot_counter++;
      if(dot_counter>=NUM_DOTS_TOP_HALF) {
        bottom_half_index = i;
        break;
      }
    }
    IP_top_half[i] = my_IP[i];
  }

  for(i=COUNT_RESET; i<NUM_DISPLAY_CHARS; i++){   //begins at the second dot
    IP_bottom_half[i] = my_IP[(i+bottom_half_index)];
    if(my_IP[i+bottom_half_index] == ' ')       //there are no spaces in an IP
      break;
  }
}

//display SSID and IP address on the LCD
void showWirelessInfo(void){
  strcpy(display_line[DISPLAY_LINE_1], my_SSID);
  strcpy(display_line[DISPLAY_LINE_2], "IP Address");
  strcpy(display_line[DISPLAY_LINE_3], IP_top_half);
  strcpy(display_line[DISPLAY_LINE_4], IP_bottom_half);
}



//==============================================================================
//                  Baud Rate Change
//==============================================================================
//converts the analog wheel into a baud rate
void toggle_Baud_Rate(void){
  switch((ADC_Thumb>>REMOVE_LOWER_10BITS)){
  case TOGGLE_9600:
    setBaud_UCA0(BAUD9600);
    setBaud_UCA3(BAUD9600);
    strcpy(display_line[DISPLAY_LINE_3], "   9600   ");
    break;
  case TOGGLE_115200:
    setBaud_UCA0(BAUD115200);
    setBaud_UCA3(BAUD115200);
    strcpy(display_line[DISPLAY_LINE_3], "  115200  ");
    break;
  case TOGGLE_460800:
    setBaud_UCA0(BAUD460800);
    setBaud_UCA3(BAUD460800);
    strcpy(display_line[DISPLAY_LINE_3], "  460800  ");
    break;
  default:
    setBaud_UCA0(BAUD9600);
    setBaud_UCA3(BAUD9600);
    break;
  }
  UCA0IE |= UCRXIE;
  UCA3IE |= UCRXIE;
}


//==============================================================================
//                  Baud Rate Initialization
//==============================================================================
//UCAxBRW     Baud Rate Control Word Register
//            UCBRx   Clock Prescaler Setting for BR Generator
//UCAxMCTLW   Modulation Control Word Register
//            UCBRSx  Second modulation stage select
//            UCBRFx  First modulation stage select (UCOS16 == 1 only)
//            UCOS16  Oversampling mode enable
//
//All values are obtained from the user guide (no math for me, no sir)
void setBaud_UCA0(long baud){
  UCA0CTLW0 |= UCSWRST; //software reset enable
  switch (baud) {
  case BAUD9600:
    UCA0BRW = PRESCALE_9600;
    UCA0MCTLW = UCOS16;
    UCA0MCTLW |= STAGE1_9600;
    UCA0MCTLW |= STAGE2_9600;
    current_Baud_0 = BAUD9600;
    break;
  case BAUD115200:
    UCA0BRW = PRESCALE_115200;
    UCA0MCTLW = UCOS16;
    UCA0MCTLW |= STAGE1_115200;
    UCA0MCTLW |= STAGE2_115200;
    current_Baud_0 = BAUD115200;
    break;
  case BAUD460800:
    UCA0BRW = PRESCALE_460800;
    UCA0MCTLW = EMPTY;
    UCA0MCTLW |= STAGE1_460800;
    UCA0MCTLW |= STAGE2_460800;
    current_Baud_0 = BAUD460800;
    break;
  }
  UCA0CTL1 &= ~UCSWRST; //disable the reset
}



void setBaud_UCA3(long baud){
  UCA3CTLW0 |= UCSWRST; //software reset enable
  switch (baud) {
  case BAUD9600:
    UCA3BRW = PRESCALE_9600;
    UCA3MCTLW = UCOS16;
    UCA3MCTLW |= STAGE1_9600;
    UCA3MCTLW |= STAGE2_9600;
    current_Baud_3 = BAUD9600;
    break;
  case BAUD115200:
    UCA3BRW = PRESCALE_115200;
    UCA3MCTLW = UCOS16;
    UCA3MCTLW |= STAGE1_115200;
    UCA3MCTLW |= STAGE2_115200;
    current_Baud_3 = BAUD115200;
    break;
  case BAUD460800:
    UCA3BRW = PRESCALE_460800;
    UCA3MCTLW = EMPTY;
    UCA3MCTLW |= STAGE1_460800;
    UCA3MCTLW |= STAGE2_460800;
    current_Baud_3 = BAUD460800;
    break;
  }
  UCA3CTL1 &= ~UCSWRST; //disable the reset
}




//==============================================================================
//              Serial Ports Initialization
//==============================================================================
void Init_Serial(void){
  //UCA0 initialization --------------------------------------------------------
  UCA0CTLW0 = EMPTY;            //initialize
  UCA0CTLW0 |= UCSSEL__SMCLK;   //BRCLK = SMCLK
  UCA0CTLW0 &= ~UCPEN;          //parity disable
  UCA0CTLW0 &= ~UC7BIT;         //8 data bits
  UCA0CTLW0 &= ~UCSPB;          //1 stop bit
  
  setBaud_UCA0(BAUD115200);
  UCA0IE |= UCRXIE;             //enable RX interrupt
  PC_TX_Enable = NO;            //disable TX until RX occurs
  
  
  //UCA3 initialization --------------------------------------------------------
  UCA3CTLW0 = EMPTY;
  UCA3CTLW0 |= UCSSEL__SMCLK;
  UCA3CTLW0 &= ~UCPEN;          //parity disable
  UCA3CTLW0 &= ~UC7BIT;         //8 data bits
  UCA3CTLW0 &= ~UCSPB;          //1 stop bit
  setBaud_UCA3(BAUD115200);
  UCA3IE |= UCRXIE;             //enable RX interrupt
  
  clear_UCA0_Ring_Buffers();
  clear_UCA3_Ring_Buffers();
}

