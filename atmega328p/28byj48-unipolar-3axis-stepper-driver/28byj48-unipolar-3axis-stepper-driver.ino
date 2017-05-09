// ATmega328P 3-axis unipolar stepper driver
// https://github.com/iotool/arduino/atmega328p/28byj48-unipolar-3axis-stepper-driver
// Version 1.0 by RoHa 2017-05-10 by 

// hardware   : standalone chip ATmega328P
// sourcecode : Arduino IDE 1.6.8
// board      : Arduino/Genuino Uno (ATmega328P)
// clock      : 8 MHz (intern) Chip, 1.8-5V
// programmer : Arduino as ISP
// 
// Use an ATmega328P chip without external clock as stepper driver for 28BYJ-48 motors.
// I want to build a CNC mashine with GRBL firmware and cheep 28BYJ48 steppers without any hacks.
// There are a solution with bipolar hardware hacks of the motor or GRBL firmware mods,
// but I try to build a soltution without hacks and mods.
// 
// This sketch use the six ADC pins of the ATmega chip as driver input, so it can fast read 
// all commands at once by reading PINC. This pins are using there internal pullup resistors 
// to pullup the signal high, because if you don't connect a signal line (don't drive tree motors)
// there will not be any output.
// Mostly you will get UNL2003 based shields to drive 28BYJ-48 stepper motors by four pins.
// The X and Y axis are controlled by PORTD (pin 0..7) and Z axis by PORTB (pin 8..12).
// 
// The pulse wide for each step must be greater than 500 us and the minimum interval should be 3 ms.
// 
// 
//                       ATmega328P             
//                   RST 01 (*)  28 PC5  ADC5   GRBL_DirZ
//  AxisX_IN1  PIN0  PD0 02      27 PC4  ADC4   GRBL_DirY
//  AxisX_IN2  PIN1  PD1 03      26 PC3  ADC3   GRBL_DirX
//  AxisX_IN3  PIN2  PD2 04      25 PC2  ADC2   GRBL_StepZ
//  AxisX_IN4  PIN3  PD3 05      24 PC1  ADC1   GRBL_StepY
//  AxisY_IN1  PIN4  PD4 06      23 PC0  ADC0   GRBL_StepX
//                   VCC 07      22 GND         
//                   GND 08      21 AREF        
//                   CLK 09      20 ACC         
//                   CLK 10      19 PB5  PIN13  LED
//  AxisY_IN2  PIN5  PD5 11      18 PB4  PIN12  AxisZ_IN4
//  AxisY_IN3  PIN6  PD6 12      17 PB3  PIN11  AxisZ_IN3
//  AxisY_IN4  PIN7  PD7 13      16 PB2  PIN10  AxisZ_IN2
//             PIN8  PB0 14      15 PB1  PIN9   AxisZ_IN1
// 
//  ic bb   port  pin  mode     function
//  02 L02  D.0   0  output     AxisX_IN1
//  03 L03  D.1   1  output     AxisX_IN2
//  04 L04  D.2   2  output     AxisX_IN3
//  05 L05  D.3   3  output     AxisX_IN4
//  06 L06  D.4   4  output     AxisY_IN1
//  11 L11  D.5   5  output     AxisY_IN2
//  12 L12  D.6   6  output     AxisY_IN3
//  13 L13  D.7   7  output     AxisY_IN4
//  14 L14  B.0   8  output     POWER
//  15 R14  B.1   9  output     AxisZ_IN1
//  16 R13  B.2  10  output     AxisZ_IN2
//  17 R12  B.3  11  output     AxisZ_IN3
//  18 R11  B.4  12  output     AxisZ_IN4
//  19 R10  B.5  13  output     LED
//  23 R06  C.0  A0  in_pullup  GRBL_StepX
//  24 R05  C.1  A1  in_pullup  GRBL_StepY
//  25 R04  C.2  A2  in_pullup  GRBL_StepZ
//  26 R03  C.3  A3  in_pullup  GRBL_DirX
//  27 R02  C.4  A4  in_pullup  GRBL_DirY
//  28 R01  C.5  A5  in_pullup  GRBL_DirZ

// --- config ---

#define CFG_LEDBLINK_IDLE_INTERVAL 900   /* ms idle mode - blink every interval millis (default 900) */
#define CFG_LEDBLINK_IDLE_PULSE 100      /* ms idle mode - turn led on for millis (default 100) */
#define CFG_LEDBLINK_WORK_INTERVAL 200   /* ms work mode - blink every interval millis (default 200) */
#define CFG_LEDBLINK_WORK_PULSE 800      /* ms work mode - turn led on for millis (default 800) */
#define CFG_AXIS_PULSE_TIMEOUT 4         /* ms turn off motor after millis (default 4) */
// #define CFG_HALF_STEPS 1              /* use half steps / slow speed (disable = comment) */

// --- pinout ---

#define PIN_STEP_X 14 /* ADC0 PortC.0 IC23(R06) */
#define PIN_STEP_Y 15 /* ADC1 PortC.1 IC24(R05) */
#define PIN_STEP_Z 16 /* ADC2 PortC.2 IC25(R04) */
#define PIN_DIR_X  17 /* ADC3 PortC.3 IC26(R03) */
#define PIN_DIR_Y  18 /* ADC4 PortC.4 IC27(R02) */
#define PIN_DIR_Z  19 /* ADC5 PortC.5 IC28(R01) */
#define PIN_IN1_X   0 /* PIN0 PortD.0 IC02(L02) */
#define PIN_IN2_X   1 /* PIN1 PortD.1 IC03(L03) */
#define PIN_IN3_X   2 /* PIN2 PortD.2 IC04(L04) */
#define PIN_IN4_X   3 /* PIN3 PortD.3 IC05(L05) */
#define PIN_IN1_Y   4 /* PIN4 PortD.4 IC06(L06) */
#define PIN_IN2_Y   5 /* PIN5 PortD.5 IC11(L11) */
#define PIN_IN3_Y   6 /* PIN6 PortD.6 IC12(L12) */
#define PIN_IN4_Y   7 /* PIN7 PortD.7 IC13(L13) */
#define PIN_IN1_Z   9 /* PIN9 PortB.1 IC15(R14) */
#define PIN_IN2_Z  10 /* PN10 PortB.2 IC16(R13) */
#define PIN_IN3_Z  11 /* PN11 PortB.3 IC17(R12) */
#define PIN_IN4_Z  12 /* PN12 PortB.4 IC18(R11) */
#define PIN_POWER   8 /* PIN8 PortB.0 IC14(L14) */
#define PIN_LED    13 /* PN13 PortB.5 IC19(R10) */

// --- fast pinmode ---

#define SET_LED_FASTLOW            PORTB &= ~B00100000  /* digitalWrite(13,LOW); */
#define SET_LED_FASTHIGH           PORTB |= B00100000   /* digitalWrite(13,HIGH); */
#define SET_AXIS_X_FASTLOW         PORTD &= ~B00001111  /* digitalWrite(0+1+2+3,LOW); */
#define SET_AXIS_Y_FASTLOW         PORTD &= ~B11110000  /* digitalWrite(4+5+6+7,LOW); */
#define SET_AXIS_Z_FASTLOW         PORTB &= ~B00011110  /* digitalWrite(9+10+11+12,LOW); */
#define SET_AXIS_X_STEP1_FASTHIGH  PORTD |= B00000011   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP2_FASTHIGH  PORTD |= B00000010   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP3_FASTHIGH  PORTD |= B00000110   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP4_FASTHIGH  PORTD |= B00000100   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP5_FASTHIGH  PORTD |= B00001100   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP6_FASTHIGH  PORTD |= B00001000   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP7_FASTHIGH  PORTD |= B00001001   /* digitalWrite(,HIGH); */
#define SET_AXIS_X_STEP8_FASTHIGH  PORTD |= B00000001   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP1_FASTHIGH  PORTD |= B00110000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP2_FASTHIGH  PORTD |= B00100000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP3_FASTHIGH  PORTD |= B01100000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP4_FASTHIGH  PORTD |= B01000000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP5_FASTHIGH  PORTD |= B11000000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP6_FASTHIGH  PORTD |= B10000000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP7_FASTHIGH  PORTD |= B10010000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Y_STEP8_FASTHIGH  PORTD |= B00010000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP1_FASTHIGH  PORTB |= B00000110   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP2_FASTHIGH  PORTB |= B00000100   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP3_FASTHIGH  PORTB |= B00001100   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP4_FASTHIGH  PORTB |= B00001000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP5_FASTHIGH  PORTB |= B00011000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP6_FASTHIGH  PORTB |= B00010000   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP7_FASTHIGH  PORTB |= B00010010   /* digitalWrite(,HIGH); */
#define SET_AXIS_Z_STEP8_FASTHIGH  PORTB |= B00000010   /* digitalWrite(,HIGH); */

// --- flags ---

#define FLG_LEDBLINK_MODE_IDLE 1    /* idle mode - no driver signals */
#define FLG_LEDBLINK_MODE_WORK 2    /* work mode - get driver signals */
#define FLG_LEDBLINK_STATUS_LOW 1   /* led off - pin low */
#define FLG_LEDBLINK_STATUS_HIGH 2  /* led on - pin high */

#define FLG_INPUT_MODE_IDLE 1       /* idle mode - no driver signals */
#define FLG_INPUT_MODE_WORK 2       /* work mode - get driver signals */
#define FLG_OUTPUT_MODE_IDLE 1      /* idle mode - no driver signals send */
#define FLG_OUTPUT_MODE_STEP1 2     /* step1 mode - driver signals IN1+2 send */
#define FLG_OUTPUT_MODE_STEP2 3     /* step2 mode - driver signals IN2 send */
#define FLG_OUTPUT_MODE_STEP3 4     /* step3 mode - driver signals IN2+3 send */
#define FLG_OUTPUT_MODE_STEP4 5     /* step4 mode - driver signals IN3 send */
#define FLG_OUTPUT_MODE_STEP5 6     /* step5 mode - driver signals IN3+4 send */
#define FLG_OUTPUT_MODE_STEP6 7     /* step6 mode - driver signals IN4 send */
#define FLG_OUTPUT_MODE_STEP7 8     /* step7 mode - driver signals IN4+1 send */
#define FLG_OUTPUT_MODE_STEP8 9     /* step8 mode - driver signals IN1 send */

// --- globals ---

unsigned long gUptimeCurrentMillis = 0;

byte gLedBlinkMode = FLG_LEDBLINK_MODE_IDLE;
byte gLedBlinkStatus = FLG_LEDBLINK_STATUS_LOW;
unsigned long gLedBlinkTimeout = 0;

byte gDriverCurrentStatus = FLG_INPUT_MODE_IDLE;
uint8_t gDriverCurrentPinc = 0;
byte gDriverCurrentStepX = 0;
byte gDriverCurrentStepY = 0;
byte gDriverCurrentStepZ = 0;
byte gDriverCurrentDirX = 0;
byte gDriverCurrentDirY = 0;
byte gDriverCurrentDirZ = 0;
byte gDriverLastStepX = 0;
byte gDriverLastStepY = 0;
byte gDriverLastStepZ = 0;
byte gDriverLastDirX = 0;
byte gDriverLastDirY = 0;
byte gDriverLastDirZ = 0;
byte gDriverOutputModeX = FLG_OUTPUT_MODE_IDLE;
byte gDriverOutputModeY = FLG_OUTPUT_MODE_IDLE;
byte gDriverOutputModeZ = FLG_OUTPUT_MODE_IDLE;
unsigned long gDriverTimeoutX = 0;
unsigned long gDriverTimeoutY = 0;
unsigned long gDriverTimeoutZ = 0;

void setup() {
  // define pinmode (use slow arduino code for better understanding)
  pinMode(PIN_STEP_X,INPUT_PULLUP); pinMode(PIN_STEP_Y,INPUT_PULLUP); pinMode(PIN_STEP_Z,INPUT_PULLUP);  
  pinMode(PIN_DIR_X,INPUT_PULLUP);  pinMode(PIN_DIR_Y,INPUT_PULLUP);  pinMode(PIN_DIR_Z,INPUT_PULLUP);
  pinMode(PIN_IN1_X,OUTPUT);        pinMode(PIN_IN1_Y,OUTPUT);        pinMode(PIN_IN1_Z,OUTPUT);  
  pinMode(PIN_IN2_X,OUTPUT);        pinMode(PIN_IN2_Y,OUTPUT);        pinMode(PIN_IN2_Z,OUTPUT);  
  pinMode(PIN_IN3_X,OUTPUT);        pinMode(PIN_IN3_Y,OUTPUT);        pinMode(PIN_IN3_Z,OUTPUT);  
  pinMode(PIN_IN4_X,OUTPUT);        pinMode(PIN_IN4_Y,OUTPUT);        pinMode(PIN_IN4_Z,OUTPUT);  
  pinMode(PIN_POWER,INPUT_PULLUP); 
  pinMode(PIN_LED,OUTPUT);
}

void currentUptime() {
  // get uptime since power on
  gUptimeCurrentMillis = millis();
}

void signalLedBlink() {
  // switch led after timeout
  if (gLedBlinkTimeout < gUptimeCurrentMillis) {
    if (gLedBlinkStatus == FLG_LEDBLINK_STATUS_LOW) {
      // turn led on
      SET_LED_FASTHIGH;
      gLedBlinkStatus = FLG_LEDBLINK_STATUS_HIGH;
      // set new timeout
      if (gLedBlinkMode == FLG_LEDBLINK_MODE_IDLE) {
        gLedBlinkTimeout = gUptimeCurrentMillis + CFG_LEDBLINK_IDLE_PULSE;
      } else {
        gLedBlinkTimeout = gUptimeCurrentMillis + CFG_LEDBLINK_WORK_PULSE;
      }
    } else {
      // turn led on
      SET_LED_FASTLOW;
      gLedBlinkStatus = FLG_LEDBLINK_STATUS_LOW;
      // set new timeout
      if (gLedBlinkMode == FLG_LEDBLINK_MODE_IDLE) {
        gLedBlinkTimeout = gUptimeCurrentMillis + CFG_LEDBLINK_IDLE_INTERVAL;
      } else {
        gLedBlinkTimeout = gUptimeCurrentMillis + CFG_LEDBLINK_WORK_INTERVAL;
      }
    }
  }
}

void inputDriver() {
  // read adc input pins
  gDriverCurrentPinc = PINC;                                       // read all pins at once
  gDriverCurrentPinc = ~gDriverCurrentPinc;                        // invert input_pullup
  gDriverCurrentStepX = bitRead(gDriverCurrentPinc,PIN_STEP_X-14); // bit 0 at port c
  gDriverCurrentStepY = bitRead(gDriverCurrentPinc,PIN_STEP_Y-14); // bit 1 at port c
  gDriverCurrentStepZ = bitRead(gDriverCurrentPinc,PIN_STEP_Z-14); // bit 2 at port c
  gDriverCurrentDirX  = bitRead(gDriverCurrentPinc,PIN_DIR_X-14);  // bit 3 at port c
  gDriverCurrentDirY  = bitRead(gDriverCurrentPinc,PIN_DIR_Y-14);  // bit 4 at port c
  gDriverCurrentDirZ  = bitRead(gDriverCurrentPinc,PIN_DIR_Z-14);  // bit 5 at port c
  // exists signal
  gDriverCurrentStatus = FLG_INPUT_MODE_IDLE;
  if (gDriverCurrentStepX == 1) gDriverCurrentStatus = FLG_INPUT_MODE_WORK;
  if (gDriverCurrentStepY == 1) gDriverCurrentStatus = FLG_INPUT_MODE_WORK;
  if (gDriverCurrentStepZ == 1) gDriverCurrentStatus = FLG_INPUT_MODE_WORK;
  // change led blink mode
  if (gDriverCurrentStatus == FLG_INPUT_MODE_IDLE) {
    gLedBlinkMode = FLG_LEDBLINK_MODE_IDLE;
  } else {
    gLedBlinkMode = FLG_LEDBLINK_MODE_WORK;
  }
}

void outputDriver() {
  // signal exists
  if (gDriverCurrentStatus == FLG_INPUT_MODE_WORK) {
    // X - detect pulse begin
    if (gDriverCurrentStepX > gDriverLastStepX) {
      SET_AXIS_X_FASTLOW;
      if (gDriverCurrentDirX == 0) {
        // move forward
        switch(gDriverOutputModeX) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_X_STEP2_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_X_STEP3_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_X_STEP4_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_X_STEP5_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_X_STEP6_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_X_STEP7_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_X_STEP8_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_X_STEP1_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeX = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_X_STEP3_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_X_STEP5_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_X_STEP7_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_X_STEP1_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      } else {
        // move backward
        switch(gDriverOutputModeX) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_X_STEP8_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_X_STEP7_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_X_STEP6_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_X_STEP5_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_X_STEP4_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_X_STEP3_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_X_STEP2_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_X_STEP1_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeX = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_X_STEP7_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_X_STEP1_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP1; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_X_STEP3_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_X_STEP5_FASTHIGH; gDriverOutputModeX = FLG_OUTPUT_MODE_STEP5; break;
          default: gDriverOutputModeX = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      }
      gDriverTimeoutX = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;      
    }
    // Y - detect pulse begin
    if (gDriverCurrentStepY > gDriverLastStepY) {
      SET_AXIS_Y_FASTLOW;
      if (gDriverCurrentDirY == 0) {
        // move forward
        switch(gDriverOutputModeY) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Y_STEP2_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_Y_STEP3_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Y_STEP4_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_Y_STEP5_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Y_STEP6_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_Y_STEP7_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Y_STEP8_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_Y_STEP1_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeY = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Y_STEP3_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Y_STEP5_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Y_STEP7_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Y_STEP1_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      } else {
        // move backward
        switch(gDriverOutputModeY) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Y_STEP8_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_Y_STEP7_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Y_STEP6_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_Y_STEP5_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Y_STEP4_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_Y_STEP3_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Y_STEP2_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_Y_STEP1_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeY = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Y_STEP7_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Y_STEP1_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP1; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Y_STEP3_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Y_STEP5_FASTHIGH; gDriverOutputModeY = FLG_OUTPUT_MODE_STEP5; break;
          default: gDriverOutputModeY = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      }
      gDriverTimeoutY = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;      
    }
    // Z - detect pulse begin
    if (gDriverCurrentStepZ > gDriverLastStepZ) {
      SET_AXIS_Z_FASTLOW;
      if (gDriverCurrentDirZ == 0) {
        // move forward
        switch(gDriverOutputModeZ) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Z_STEP2_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_Z_STEP3_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Z_STEP4_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_Z_STEP5_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Z_STEP6_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_Z_STEP7_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Z_STEP8_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_Z_STEP1_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Z_STEP3_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Z_STEP5_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Z_STEP7_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Z_STEP1_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      } else {
        // move backward
        switch(gDriverOutputModeZ) {
        #ifdef CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Z_STEP8_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP8; break;
          case FLG_OUTPUT_MODE_STEP2: SET_AXIS_Z_STEP7_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Z_STEP6_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP6; break;
          case FLG_OUTPUT_MODE_STEP4: SET_AXIS_Z_STEP5_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP5; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Z_STEP4_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP4; break;
          case FLG_OUTPUT_MODE_STEP6: SET_AXIS_Z_STEP3_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Z_STEP2_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP2; break;
          case FLG_OUTPUT_MODE_STEP8: SET_AXIS_Z_STEP1_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP1; break;
          default: gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP8; break;
        #else // CFG_HALF_STEPS
          case FLG_OUTPUT_MODE_STEP1: SET_AXIS_Z_STEP7_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
          case FLG_OUTPUT_MODE_STEP3: SET_AXIS_Z_STEP1_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP1; break;
          case FLG_OUTPUT_MODE_STEP5: SET_AXIS_Z_STEP3_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP3; break;
          case FLG_OUTPUT_MODE_STEP7: SET_AXIS_Z_STEP5_FASTHIGH; gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP5; break;
          default: gDriverOutputModeZ = FLG_OUTPUT_MODE_STEP7; break;
        #endif
        } 
      }
      gDriverTimeoutZ = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;      
    }
  }
  // timeout (maximal pulse wide)
  if (gDriverTimeoutX < gUptimeCurrentMillis) {
    SET_AXIS_X_FASTLOW;
    gDriverTimeoutX = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;
  }
  if (gDriverTimeoutY < gUptimeCurrentMillis) {
    SET_AXIS_Y_FASTLOW;
    gDriverTimeoutY = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;
  }
  if (gDriverTimeoutZ < gUptimeCurrentMillis) {
    SET_AXIS_Z_FASTLOW;
    gDriverTimeoutZ = gUptimeCurrentMillis + CFG_AXIS_PULSE_TIMEOUT;
  }
  // remember current state to compare at next round
  gDriverLastStepX = gDriverCurrentStepX;   
  gDriverLastStepY = gDriverCurrentStepY;   
  gDriverLastStepZ = gDriverCurrentStepZ;   
  gDriverLastDirX  = gDriverCurrentDirX;    
  gDriverLastDirY  = gDriverCurrentDirY;    
  gDriverLastDirZ  = gDriverCurrentDirZ;    
}

void loop() {
  currentUptime();   // need for timeout handling 
  signalLedBlink();  // display driver status
  inputDriver();     // read input signals from GRBL
  outputDriver();    // write output signals to 28BYJ48
}