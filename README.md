# Embedded_Systems_Project
C Code written for a IoT-Enabled / Autonomous Vehicle 

The project was to create a small vehicle that ran off of an MSP 430 microcontroller.  The vehicle needed to follow a black line on the ground without any user interaction.  It also needed to incorporate a manual control mode, which was accomplished with a Wi-Fi module.  The user interface was a scroll-wheel, buttons, and an LCD display, all located directly on the vehicle.  All of the code here is my own work, unless otherwise stated.  

---

The general organization is as follows:

## macros.h
- global function declarations
- a lot of macros (to eliminate "Magic Numbers")

## ADC.c
- handles the thumb wheel and detectors
- includes calibration, emitter control, and displaying values

## clocks.c (provided by teacher)
- contains clock initiation functions

## init.c
- calls every initialization function

## interrupts_xxx.c
- interrupt handling, for events like button pressing or incoming wi-fi message

## lcd.c
- functions for interacting with the lcd screen and formatting data to be shown on the screen

## main.c
- operating system of the vehicle

## menu.c
- implements a menu system with different programs for the vehicle to run

## ports.c
- initializes the ports of the MSP 430

## serial.c
- handles all serial communications
- probably the *most impressive file*

## shapes.c
- handles moving the motors
- why "shapes"?  a sub-goal of the project was to have the vehicle maneuver in different shapes (i.e. triangle or square)

## switch.c
- for buttons

## system.c (provided by teacher)
- enables the interrupts

## timers.c
- implements real-time events, as required for the project
- timers are tied closely to interrupts in this program
