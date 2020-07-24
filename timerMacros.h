//==============================================================================
//      Chris Hamby Presents...
//
//      timerMacros.h
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      macros for the timer
//==============================================================================
#ifndef INCL_TIMERMACROS
#define INCL_TIMERMACROS        (0xFF)

extern void Init_Timers(void);
extern void Init_Timer_A0(void);
extern void Init_Timer_B0(void);

extern void Timer_Process(void);
extern void delay_100ms(int delay_amount);
extern void Show_RTC200_Process(void);


//extern unsigned volatile char update_display_count;
extern unsigned volatile int TA0_CCR0_COUNT;
extern unsigned volatile int TA0_CCR1_COUNT;
extern volatile unsigned int my_lcd_count;


//Use Timer A0 to raise an interrupt flag every 50ms
//        FREQUENCY     PERIOD          10ms            50ms
//SMCLK   8 MHz         0.125 us        80000           400000
//TIMERA0 500 kHz       2 us            5000            25000
#define SMCLK_FREQUENCY (8000000)
#define TIMERA0_10MS    (5000)
#define TIMERA0_100MS   (50000)
#define TA0CCR0_INTERVAL        TIMERA0_100MS    //the main timer used in Timer Process
#define TA0CCR1_INTERVAL        TIMERA0_100MS    //the button debounce timer, enabled/disabled often

#define TIMER_IV_MAX    (14)
#define CCR_NOFLAG      (0x00)
#define CCR1_FLAG       (0x02)



//Real Time Clock - 200ms (RTC200) increases every 200 milliseconds
//The function ShowRTC200(DISPLAY_LINE) shows the formatted clock
extern unsigned int RTC200;            //RTC200 value
extern char runTimer;                  //Start/Pause RTC200; used in TimerProcess
extern void resetRTC200(void);         //restarts RTC200 at 00:00.00
#define RTC200_CIRCLE           150    //approximate time to complete a circle
#define RTC200_INCREMENT        2
#define MS_PLACE                6
#define DEC_PLACE               5
#define SEC_PLACE_L             4
#define SEC_PLACE_M             3
#define SEC_PLACE_H             2


//how many TA0CCR0 intervals correspond with various lengths of time
#define HUNDRED_MS_TA0CCR0              (1)
#define TWOHUNDRED_MS_TA0CCR0           (2)
#define VHUNDRED_MS_TA0CCR0             (5)
#define ONE_SECOND_TA0CCR0              (10) 
#define TWO_SECOND_TA0CCR0              (20)
#define THREE_SECOND_TA0CCR0            (30)
#define FOUR_SECOND_TA0CCR0             (40)
#define FIVE_SECOND_TA0CCR0             (50)
#define my_lcd_max                      (2*HUNDRED_MS)    //200ms


//I don't think another timer will be used
#define TEN_MS          TEN_MS_TA0CCR0
#define TWENTY_MS       TWENTY_MS_TA0CCR0
#define FIFTY_MS        FIFTY_MS_TA0CCR0
#define HUNDRED_MS      HUNDRED_MS_TA0CCR0
#define TWOHUNDRED_MS   TWOHUNDRED_MS_TA0CCR0
#define VHUNDRED_MS     VHUNDRED_MS_TA0CCR0
#define ONE_SECOND      ONE_SECOND_TA0CCR0
#define TWO_SECOND      TWO_SECOND_TA0CCR0
#define THREE_SECOND    THREE_SECOND_TA0CCR0
#define FOUR_SECOND     FOUR_SECOND_TA0CCR0
#define FIVE_SECOND     FIVE_SECOND_TA0CCR0

#define DISPLAY_UPDATE_TIME  (2)          //how often to update the lcd display (in hundred ms)

#endif
