//==============================================================================
//      Chris Hamby Presents...
//
//      shapeMacros.h
//
//      code updated 3-Dec-2018
//      formatted 5-Dec-2018
//
//      shape macros related to motor movement
//
//==============================================================================

#ifndef _SHAPE_MACROS
#define _SHAPE_MACROS 
//==================================================
//              Global Function List
//==================================================   
extern void Forward_Move(void);
extern void Reverse_Move(void);
extern void Motors_Off(void);

extern void Brake_Left(void);
extern void Brake_Right(void);
extern void Brake_All(void);

extern void Forward_Timed(int num_ms);
extern void Reverse_Timed(int num_ms);
extern void Left_Timed(int num_ms);
extern void Right_Timed(int num_ms);

extern void Turn_Right(void);
extern void Turn_Left(void);
extern void Turn_180(void);

extern char followingLine;

extern char TURN_STATE;
#define NO_TURN                 (0)
#define TURN_LEFT               (1)
#define TURN_RIGHT              (2)

#define TURN_LEFT_TIME          (9)
#define TURN_RIGHT_TIME         (9)
#define TURN_180_TIME           (18)

#define FINDLINE_SETUP          (0)
#define FINDLINE_RUN            (1)
#define FINDLINE_FOUND          (2)

#define FOLLOWLINE_SETUP        (0)
#define FOLLOWLINE_RUN          (1)
#define FOLLOWLINE_INTO_CIRCLE  (2)
#define FOLLOWLINE_COMPLETE     (3)

//PWM-related macros for the wheels
//The wheels use TB0
#define LEFT_REVERSE_SPEED      (TB0CCR3)
#define LEFT_FORWARD_SPEED      (TB0CCR4)
#define RIGHT_REVERSE_SPEED     (TB0CCR5)
#define RIGHT_FORWARD_SPEED     (TB0CCR6)

#define WHEEL_PERIOD                    10000
#define WHEEL_OFF                       0
#define DEFAULT_SPEED_REVERSE           2000
#define LEFT_DEFAULT_TRAVEL_SPEED       4000
#define RIGHT_DEFAULT_TRAVEL_SPEED      4000
#define LEFT_DEFAULT_FOLLOW_SPEED       2800//1800
#define RIGHT_DEFAULT_FOLLOW_SPEED      2900//1900
#define LEFT_MAX_SPEED                  4000//2700
#define RIGHT_MAX_SPEED                 4000//2700
#define LEFT_MIN_SPEED                  800
#define RIGHT_MIN_SPEED                 800
#define SPEED_INCREMENT                 10
#define SPEED_DECREMENT                 1
#define TEST_STATE1             (1)
#define TEST_STATE2             (2)
#define TEST_STATE3             (3)
#define TEST_STATE4             (4)
#define TEST_STATE5             (5)
#define MAX_TEST_STATE          (6)

#define COOLDOWN_TIME           (500000)
#define COOLDOWN_DONE           (0)

#define COUNT_RESET                     (0)

#endif