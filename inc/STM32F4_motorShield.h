/*
 * Code is from http://embeddedsystemengineering.blogspot.cz/2015/08/stm32f4-discovery-tutorial-10-pwm.html 
 * and /home/vasek/src/stm32f4-examples-master/Task-4-PWM/src/main.c 
 * */

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
//#include "tm_stm32f4_gpio.h" 
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

const uint16_t LEDS = GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_1;
const uint16_t USER_BUTTON = GPIO_Pin_0;

static uint8_t latch_state;

// Bit positions in the 74HCT595 shift register output
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

// Constants that the user passes in to the motor calls
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

// STM32F4-discovery pin names for interface to 74HCT595 latch
/*#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8*/

#define _BV(n) 1 << n

int brightLed1 = 0;
int brigthLed2 = 5;
int motorSpeed = 5;

int delta = 1;
int lastButtonStatus = RESET;

void init();
void loop();

void delay();

void initLatchPins();
void initMotorShield();
void initButton();
void initLeds();
void initTimer();
void initPWM();


