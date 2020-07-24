//==============================================================================
//      Chris Hamby Presents...
//
//      macros.h
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      general use macros
//
//      I thought it would be cleaner to have some macros separated by file
//      Hence shapeMacros.h and timerMacros.h
//==============================================================================

#include "shapeMacros.h"    //probably best to leave this here for now    
#include "timerMacros.h"    //leave them outside #ifndef for some reason i can't quite elaborate on

#ifndef INCL_MACROS         //Prevents double inclusions
#define INCL_MACROS (1)

//Functions from around the world
//menu.c ==========================
extern void Menu_Process(void);
extern void Event_Process(void);
extern void endEvent(void);


//ADC.c ===========================
extern void Init_ADC(void);
extern void ADC_Process(void);
extern void Calibrate_Process(void);
extern void Show_Adc_Process(void);
extern void Enable_Emitter(void);
extern void Disable_Emitter(void);


//lcd.c ============================
extern void clearDisplay(void);
extern void HEXtoBCD(int hex_value);
extern void showADC(int ADC_value, int line);
extern void showRTC200(int line);

//shapes.c =========================
extern void MotorTest_Setup(void);
extern void MotorTest_Process(void);
extern void FindLine_Setup(void);
extern void FindLine_Process(void);
extern void FollowLine_Setup(void);
extern void FollowLine_Process(void);


//switch.c =========================
extern char Check_Button_1(void);
extern char Check_Button_2(void);


//serial.c =========================
extern void Init_Serial(void);
extern void IOT_Process(void);
extern void UART0_Setup(void);
extern void UART3_Setup(void);
extern void IOT_Enable_Process(void);
extern char get_UCA3_RX(void);



//====================================================
//Menu Events =================================
//=======================================
//simply rearrange these to rearrange the menu (don't mess with 1)
#define CALIBRATE               (1)
#define FIND_LINE               (2)
#define FOLLOW_LINE             (3)
#define IOT_PROCESS             (4)
#define SHOW_ADC                (5)
#define MOTORTEST               (6)
#define IOT_ENABLE              (7)
#define SHOW_RTC200_PROCESS     (8)
#define NUM_EVENTS              (8)



//GLOBALS from main.c ==========================================================
//declaring them here makes them visible elsewhere
//the extern means that the compiler looks for the definition elsewhere
extern volatile unsigned int my_update_count;      
extern volatile char one_Time;
extern volatile char event;     //what's happening?
extern char next_event;

//magic number destroyers
#define ALWAYS                  (1)
#define RESET_STATE             (0)

#define YES                     (1)
#define NO                      (0)
#define IT_CHANGED              (1)
#define CHECK_RESET             (0)
#define TIME_RESET              (0)

//clocks.c
#define FLLN_BITS               (0x03ffu)
#define FLLN_255                (0x00ffu)
#define CSLOCK                  (0x01) // Any incorrect password locks registers
#define EMPTY                   (0x00)
#define DIR_ALL_OUT             (0xFF)

#define REMOVE_LOWER_8BITS      (8)
#define REMOVE_LOWER_9BITS      (9)
#define REMOVE_LOWER_10BITS     (10)
// ============================================================================
// ======================           Display             =======================
// ============================================================================
extern char shrug_guy[11];
#define SHRUG_MAN       ("  _(�)_/  ")
#define SHRUG_HAND      (0xFF)
#define SHRUG_ARM       (0x60)
#define SHRUG_FACE      (0xC2)
#define SHRUG_LHAND     (0)
#define SHRUG_LARM      (1)
#define SHRUG_FACE_POS  (4)
#define SHRUG_RHAND     (8)

#define NUM_DISPLAY_LINES  (4)
#define NUM_DISPLAY_CHARS  (11)

extern char display_line[NUM_DISPLAY_LINES][NUM_DISPLAY_CHARS];
extern char *display[NUM_DISPLAY_LINES];
extern volatile unsigned char update_display;
extern volatile unsigned char display_changed;
extern volatile unsigned int update_display_count;

// init.c magic number destroyers
#define DISPLAY_RESET   (0)
#define FIRST_CHAR      (0)
#define LAST_CHAR       (10)
#define DISPLAY_1       (0)
#define DISPLAY_2       (1)
#define DISPLAY_3       (2)
#define DISPLAY_4       (3)
#define DISPLAY_LINE_1     (0)
#define DISPLAY_LINE_2     (1)
#define DISPLAY_LINE_3     (2)
#define DISPLAY_LINE_4     (3)
#define CS_REG_LOCK        (0)


//P3 selection values
#define USE_L_REVERSE   (0x00)
#define USE_SMCLK       (0x01)
#define USE_TB0         (0x02)


//=============================================================================
//========  Button Pressing, Mutton Stressing, Flutton Dressing  ==============
//=============================================================================
#define BUTTON1_FLAG            PX6_FLAG
#define BUTTON2_FLAG            PX5_FLAG
extern volatile char button1_seen;
extern volatile char button2_seen;
extern volatile char button1_debouncing;
extern volatile char button2_debouncing;
extern volatile unsigned char button1_pressed;
extern volatile unsigned char button2_pressed;
extern unsigned volatile int button1_debounce_count;
extern unsigned volatile int button2_debounce_count;

#define BUTTON1_DEBOUNCE_MAX_COUNT      (10)  //1000 ms
#define BUTTON2_DEBOUNCE_MAX_COUNT      (10)  //1000 ms
#define BUTTON_ON                       (0x01)
#define BUTTON_OFF                      (0x00)

// HEX to BCD macros
#define THOUSANDS_CHECK         999
#define DECREASE_THOUSAND       1000
#define HUNDREDS_CHECK          99
#define DECREASE_HUNDRED        100
#define TENS_CHECK              9
#define DECREASE_TEN            10
#define ADJUST_1                1
#define CONVERSION_CHAR_1       0
#define CONVERSION_CHAR_2       1
#define CONVERSION_CHAR_3       2
#define CONVERSION_CHAR_4       3
#define ADC_STRING_SIZE         4
#define MAKE_A_CHAR             0x30



//=============================================================================
//========================           ADC          =============================
//=============================================================================
extern volatile int ADC_Thumb;
extern volatile int ADC_Left_Detector;
extern volatile int ADC_Right_Detector;

extern float on_threshold;      //calculate
extern float black_threshold;   //calculate
extern float white_threshold;   //calculate

#define DEFAULT_BLACK_THRESHOLD           (2000)
#define DEFAULT_WHITE_THRESHOLD           (3500)
#define DEFAULT_ON_THRESHOLD              (4000)
#define BLACK_MARGIN                      (50)
#define WHITE_MARGIN                      (100)
#define NUM_DETECTORS                     (2)
#define IR_TOGGLE_TIME                    (2)   //200 ms
#define AVERAGE_2                         (2)

//calibration event colors
#define CAL_OFF                     (0)
#define CAL_WHITE                   (1)
#define CAL_BLACK                   (2)
#define CAL_LINE                    (3)
#define DONE_CALIBRATING            (4)
#define OFF_MARGIN                  (200)


// ============================================================================
// =======================        Serial Comms          =======================
// ============================================================================
#define BAUD9600        (9600)
#define BAUD115200      (115200)
#define BAUD460800      (460800)
//UCABRW        prescaler setting for BR generator
#define PRESCALE_9600           (52)
#define PRESCALE_115200         (4)
#define PRESCALE_460800         (17)
//UCBRF         first modulation stage select
#define STAGE1_9600             (0x0010)
#define STAGE1_115200           (0x0050)        //5 = 0b0101 
#define STAGE1_460800           (0)             //n/a
//UCBRS         second modulation stage select
#define STAGE2_9600             (0x4900)
#define STAGE2_115200           (0x5500)        //0x55 = 0b01010101
#define STAGE2_460800           (0x4A00)        //0xEE = 0b11101110

#define TOGGLE_9600             (1)
#define TOGGLE_115200           (2)
#define TOGGLE_460800           (3)

#define NEW_LINE_CHAR           ('\n')          //0x0A
#define RETURN_CHAR             ('\r')          //0x0D

#define USCI_NO_FLAG            (0)
#define USCI_RX_FLAG            (0x02)
#define USCI_TX_FLAG            (0x04)
#define USCI_MAX_FLAG           (8)
#define BEGINNING               (0)
#define SMALL_RING_SIZE         (50)    //ring buffer size
#define LARGE_RING_SIZE         (32)    //idk if we'll need large ring buffers


// Ring Buffers
extern volatile char UCA0_Char_Rx[SMALL_RING_SIZE];     //UCA0 RX Ring Buffer
extern volatile char UCA0_Char_Tx[SMALL_RING_SIZE];     //UCA0 TX Ring Buffer
extern volatile char UCA3_Char_Rx[SMALL_RING_SIZE];     //UCA3 RX Ring Buffer
extern volatile char UCA3_Char_Tx[SMALL_RING_SIZE];     //UCA3 TX Ring Buffer

extern volatile unsigned int UCA0_TX_Index;
extern volatile unsigned int UCA3_TX_Index;     //transmit index TX_RING_BUF -> TXBUF

extern volatile unsigned int UCA0_rx_ring_wr;   //write index RXBUF -> RING_BUF
extern volatile unsigned int UCA0_rx_ring_rd;   //read index RING_BUF -> program
extern volatile unsigned int UCA3_rx_ring_wr;
extern volatile unsigned int UCA3_rx_ring_rd;

extern volatile unsigned int UCA0_tx_ring_wr;   
extern volatile unsigned int UCA0_tx_ring_rd;   
extern volatile unsigned int UCA3_tx_ring_wr;
extern volatile unsigned int UCA3_tx_ring_rd;

extern volatile char test_command[11];  //transmit this to test serial comms
extern volatile char endChar_found;
extern volatile char tempChar;

extern volatile char PC_TX_Enable;
extern unsigned volatile int network_parse_index;
    
extern volatile int UCA0_numToTransmit;
extern volatile int UCA3_numToTransmit;
// ============================================================================
// =======================        Wireless Info         =======================
// ============================================================================
#define IP_IDENTITY_LENGTH      9
#define SSID_IDENTITY_LENGTH    8
#define IP_COMMAND_LENGTH       11
#define IP_LENGTH               15
#define NEXT_TO_LAST            1
#define COMMAND_MAX_LENGTH      (50)
#define WIRELESS_LENGTH         (300)
#define IP_TOP_HALF_LENGTH      (8)
#define IP_BOTTOM_HALF_LENGTH   (8)
#define NUM_DOTS_TOP_HALF       (2)

#define TCP_LOOK_FOR_START      (0)
#define TCP_STORE_COMMAND       (1)
#define TCP_EXECUTE_COMMAND     (2)
#define TCP_ESCAPE_CHAR         (0x1B)

#define COMMAND_PIN             (6824)
#define COMMAND_PIN_INDEX       (0)
#define COMMAND_DIRECTION_INDEX (4)
#define COMMAND_LETTER_INDEX    (5)
#define COMMAND_TIME_INDEX      (6)

#define MOVE_UP_A_TENS_PLACE    (10)

extern char Command_Char[COMMAND_MAX_LENGTH];
extern volatile char Command_Char_Receive;
extern volatile char store_wireless_info;
extern char wireless_info[WIRELESS_LENGTH];

extern char my_IP[IP_LENGTH];
extern char IP_top_half[NUM_DISPLAY_CHARS];
extern char IP_bottom_half[NUM_DISPLAY_CHARS];
extern char my_SSID[NUM_DISPLAY_CHARS];


extern char WIFI_Command[COMMAND_MAX_LENGTH];
extern unsigned int WIFI_Command_Index;



// ============================================================================
// ======================           Port Pins            ======================
// ============================================================================
// Port 1 Pins
#define RED_LED         (0x01) //0000 0001
#define GRN_LED         (0x02) //0000 0010
#define V_THUMB         (0x04) //0000 0100
#define TESTPROBE       (0x08) //0000 1000
#define V_DETECT_R      (0x10) //0001 0000
#define V_DETECT_L      (0x20) //0010 0000
#define SD_UCB0SIMO     (0x40) //0100 0000
#define SD_UCB0SOMI     (0x80) //1000 0000

// Port 2 Pins
#define UCA0TXD         (0x01)
#define UCA0RXD         (0x02)
#define SD_SPICLK       (0x04)
#define P2_3            (0x08)
#define P2_4            (0x10)
#define UCA1TXD         (0x20)
#define UCA1RXD         (0x40)
#define P2_7            (0x80)

// Port 3 Pins
#define IOT_RESET       (0x01)
#define IOT_PROG_MODE   (0x02)
#define IOT_LINK        (0x04)
#define IOT_PROG_SEL    (0x08)
#define L_REVERSE       (0x10)
#define L_FORWARD       (0x20)
#define R_REVERSE       (0x40)
#define R_FORWARD       (0x80)

// Port 4 Pins
#define SD_CS           (0x01)
#define J4_31           (0x02)
#define J4_32           (0x04)
#define J4_33           (0x08)
#define UCB1_CS_LCD     (0x10)
#define P4_4            (0x20)
#define P4_5            (0x40)
#define J3_29           (0x80)

// Port 5 Pins
#define UCB1SIMO        (0x01)
#define UCB1SOMI        (0x02)
#define UCB1SCK         (0x04)
#define RESET_LCD       (0x08)
#define P5_4            (0x10)
#define BUTTON2         (0x20)
#define BUTTON1         (0x40)
#define LCD_BACKLITE    (0x80) 

// Port 6 Pins
#define UCA3TXD         (0x01)
#define UCA3RXD         (0x02)
#define J1_5            (0x04)
#define MAG_INT         (0x08)
#define P6_4            (0x10)
#define P6_5            (0x20)
#define P6_6            (0x40)
#define P6_7            (0x80)

// Port 7 Pins
#define I2CSDA          (0x01)
#define I2CSCL          (0x02)
#define SD_DETECT       (0x04)
#define J4_36           (0x08)
#define P7_4            (0x10)
#define P7_5            (0x20)
#define P7_6            (0x40)
#define P7_7            (0x80)

// Port 8 Pins
#define IR_LED          (0x01)
#define OPT_INT         (0x02)
#define TMP_INT         (0x04)
#define INT2            (0x08)

// Port J Pins
#define PJ_0            (0x01)
#define PJ_1            (0x02)
#define PJ_2            (0x04)
#define PJ_3            (0x08)
#define LFXIN           (0x10)
#define LFXOUT          (0x20)
#define HFXIN           (0x40)
#define HFXOUT          (0x80)

// Port J Pins pt.2 (?)
#define XINR                 (0x10) // XINR
#define XOUTR                (0x20) // XOUTR
#define CLEAR_REGISTER     (0X0000)

//Port Interrupt Macros
#define PORT_IV_MAX     (16)
#define PX0_FLAG        (2)
#define PX1_FLAG        (4)
#define PX2_FLAG        (6)
#define PX3_FLAG        (8)
#define PX4_FLAG        (10)
#define PX5_FLAG        (12)
#define PX6_FLAG        (14)
#define PX7_FLAG        (16)

#endif