#include "STM32F4_motorShield.h"

// set motor shield latch state OK
// set up PWM OK
// send PWM signal to motor 1 OK

int main(void) {
    init();

    do {
        loop();
    } while (1);
}

void init() {
    
    initButton();
    initLatchPins();
    initMotorShield();
    initLeds();
    initTimer();
    initPWM();
    runMotor(1, FORWARD);
}

void loop() {
    
    uint8_t currentButtonStatus = GPIO_ReadInputDataBit(GPIOA, USER_BUTTON);
    
    if (lastButtonStatus != currentButtonStatus && currentButtonStatus != RESET) {
        motorSpeed *= 2;
        if (motorSpeed >= 500 ) {
            motorSpeed = 5;
        }
        TIM_SetCompare2(TIM4, motorSpeed);
    }
    lastButtonStatus = currentButtonStatus;

    delay(4);
}

void delay(uint32_t ms) {
    ms *= 3360;
    while(ms--) {
        __NOP();
    }
}
void initLeds() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LEDS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//leds
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
    
    //motor on PD0
    //GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_TIM4);
    
}
void initLatchPins()
{
	//set pins 0,1,2,3 as output pins to control the motor latch on Arduino motor shield
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}
void initButton() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_IN;
    gpio.GPIO_Pin = USER_BUTTON;
    GPIO_Init(GPIOA, &gpio);
}

void initTimer() {
    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* Compute the prescaler value */
    u32 PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 21000000) - 1;//PrescalerValue = 3 in current configuration

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    /* Time base configuration */
    //PWM frequency
    TIM_TimeBaseStructure.TIM_Period = 1000;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
}

void initPWM() {
    TIM_OCInitTypeDef TIM_OCInitStructure;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;//http://embeddedsystemengineering.blogspot.cz/2015/08/stm32f4-discovery-tutorial-10-pwm.html : TIM_Pulse=2099
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    /* PWM1 Mode configuration: Channel2 (GPIOD Pin 13)*/
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM1 Mode configuration: Channel4 (GPIOD Pin 15)*/
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel3 (GPIOD Pin 1) - motor*/
    /*TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);*/

    TIM_Cmd(TIM4, ENABLE);
}
void latch_tx()//from AFmotor library for Arduino
{
	//GPIO_SetBits(GPIOC, GPIO_Pin_0);
	//GPIO_WriteBit(GPIOA, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_13)));
	/*PC0 MOTORLATCH 12
	  PC1 MOTORCLK 4
	  PC2 MOTORENABLE 7
	  PC3 MOTORDATA 8 
*/	
	//GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1);
  uint8_t i;

/*
 * BitVal,:	specifies the value to be written to the selected bit. This parameter can be one of the BitAction enum values:

    Bit_RESET: to clear the port pin
    Bit_SET: to set the port pin
*/

  //LATCH_PORT &= ~_BV(LATCH);
  //digitalWrite(MOTORLATCH, LOW);
  GPIO_WriteBit(GPIOC, GPIO_Pin_0, 0);	
  //SER_PORT &= ~_BV(SER);
  //digitalWrite(MOTORDATA, LOW);
  GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
  
  for (i=0; i<8; i++) {
    //CLK_PORT &= ~_BV(CLK);
    //digitalWrite(MOTORCLK, LOW);
	GPIO_WriteBit(GPIOC, GPIO_Pin_1, 0);
    if (latch_state & _BV(7-i)) {
      //SER_PORT |= _BV(SER);
      //digitalWrite(MOTORDATA, HIGH);
	  GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
    } else {
      //SER_PORT &= ~_BV(SER);
      //digitalWrite(MOTORDATA, LOW);
      GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
    }
    //CLK_PORT |= _BV(CLK);
    //digitalWrite(MOTORCLK, HIGH);
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, 1);
  }
  //LATCH_PORT |= _BV(LATCH);
  //digitalWrite(MOTORLATCH, HIGH);
  GPIO_WriteBit(GPIOC, GPIO_Pin_0, 1);
}
void initMotorShield()
{
	latch_state = 0;

	latch_tx();
	// enable the chip outputs!
	//digitalWrite(MOTORENABLE, LOW);
	GPIO_WriteBit(GPIOC, GPIO_Pin_2, 0);
	
	latch_state &= ~_BV(MOTOR1_A) & ~_BV(MOTOR1_B); // set both motor pins to 0
    latch_tx();
    //initPWM1(freq);
}
void runMotor(uint8_t motornum, uint8_t cmd)
{
  uint8_t a, b;
  
  switch (motornum) {
  case 1:
    a = MOTOR1_A; b = MOTOR1_B; break;
  case 2:
    a = MOTOR2_A; b = MOTOR2_B; break;
  case 3:
    a = MOTOR3_A; b = MOTOR3_B; break;
  case 4:
    a = MOTOR4_A; b = MOTOR4_B; break;
  default:
    return;
  }
  
  switch (cmd) {
  case FORWARD:
    latch_state |= _BV(a);
    latch_state &= ~_BV(b); 
    latch_tx();
    break;
  case BACKWARD:
    latch_state &= ~_BV(a);
    latch_state |= _BV(b); 
    latch_tx();
    break;
  case RELEASE:
    latch_state &= ~_BV(a);     // A and B both low
    latch_state &= ~_BV(b); 
    latch_tx();
    break;
  }
}

