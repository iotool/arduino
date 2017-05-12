// ATmega328P 3-axis unipolar stepper driver
// 
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
// The pulse wide for each step must be greater than 250 us and the minimum interval should be 3 ms (333 MHz).
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
//  14 L14  B.0   8  output     
//  15 R14  B.1   9  output     AxisZ_IN1
//  16 R13  B.2  10  output     AxisZ_IN2
//  17 R12  B.3  11  output     AxisZ_IN3
//  18 R11  B.4  12  output     AxisZ_IN4
//  19 R10  B.5  13  output     
//  23 R06  C.0  A0  in_pullup  GRBL_StepX
//  24 R05  C.1  A1  in_pullup  GRBL_StepY
//  25 R04  C.2  A2  in_pullup  GRBL_StepZ
//  26 R03  C.3  A3  in_pullup  GRBL_DirX
//  27 R02  C.4  A4  in_pullup  GRBL_DirY
//  28 R01  C.5  A5  in_pullup  GRBL_DirZ
// 
//  history
//  2017-05-11  RoHa  optimized for 250 us pulse wide
// 

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
#define PIN_POWER   8 /* PIN8 PortB.0 IC14(L14) - not used */
#define PIN_LED    13 /* PN13 PortB.5 IC19(R10) - not used */

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

#define TIMEINTERVAL_1000MS 2000 /* 8 MHz */
#define TIMEINTERVAL_100MS 200 /* 8 MHz */
#define TIMEINTERVAL_10MS 20 /* 8 MHz */
#define TIMEINTERVAL_5MS 10 /* 8 MHz */
#define TIMEINTERVAL_4MS 8 /* 8 MHz */
#define TIMEINTERVAL_3MS 6 /* 8 MHz */
#define TIMEINTERVAL_2MS 4 /* 8 MHz */
#define TIMEINTERVAL_1MS 2 /* 8 MHz */

unsigned long gUptimeCurrentMillis = 0;

uint8_t gDriverCurrentPinC = 0;
uint8_t gDriverLastPinC = 0;
uint8_t gDriverCurrentXYZ = 0;

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

#define STEP_NEXT 64
#define STEP_1 0
#define STEP_2 64
#define STEP_3 128
#define STEP_4 192

unsigned long gDriverOutputTimeout = 0;      
byte gDriverOutputStepX = STEP_1;
byte gDriverOutputStepY = STEP_1;
byte gDriverOutputStepZ = STEP_1;

void loop() {

  // --- uptime ---
  gUptimeCurrentMillis = millis();
  
  // --- fast read ---
  gDriverCurrentPinC = PINC;                                   // fast read
  gDriverCurrentPinC &= ~B11000000;                            // ignore pin 6..7
  if (gDriverCurrentPinC != gDriverLastPinC) {                 // pins changed
    // --- new signal ---
    gDriverLastPinC = gDriverCurrentPinC;
    gDriverCurrentXYZ = gDriverCurrentPinC & B00000111;        // one or more step signal
    if (gDriverCurrentXYZ > 0) {
      // --- step signal found ---
      gDriverLastStepX = gDriverCurrentStepX;   
      gDriverLastStepY = gDriverCurrentStepY;   
      gDriverLastStepZ = gDriverCurrentStepZ;   
      gDriverLastDirX  = gDriverCurrentDirX;    
      gDriverLastDirY  = gDriverCurrentDirY;    
      gDriverLastDirZ  = gDriverCurrentDirZ;    
      gDriverCurrentStepX = bitRead(gDriverCurrentPinC,PIN_STEP_X-14); // bit 0 at port c
      gDriverCurrentStepY = bitRead(gDriverCurrentPinC,PIN_STEP_Y-14); // bit 1 at port c
      gDriverCurrentStepZ = bitRead(gDriverCurrentPinC,PIN_STEP_Z-14); // bit 2 at port c
      gDriverCurrentDirX  = bitRead(gDriverCurrentPinC,PIN_DIR_X-14);  // bit 3 at port c
      gDriverCurrentDirY  = bitRead(gDriverCurrentPinC,PIN_DIR_Y-14);  // bit 4 at port c
      gDriverCurrentDirZ  = bitRead(gDriverCurrentPinC,PIN_DIR_Z-14);  // bit 5 at port c
      // --- move ---
      if (gDriverCurrentStepX > gDriverLastStepX) {
        // --- x axis goes high ---
        if (gDriverCurrentDirX == 0) {
          gDriverOutputStepX += STEP_NEXT;
        } else {
          gDriverOutputStepX -= STEP_NEXT;
        }
        SET_AXIS_X_FASTLOW;
        if (gDriverOutputStepX == STEP_1) { SET_AXIS_X_STEP1_FASTHIGH; }
        if (gDriverOutputStepX == STEP_2) { SET_AXIS_X_STEP3_FASTHIGH; }
        if (gDriverOutputStepX == STEP_3) { SET_AXIS_X_STEP5_FASTHIGH; }
        if (gDriverOutputStepX == STEP_4) { SET_AXIS_X_STEP7_FASTHIGH; }
      }
      if (gDriverCurrentStepY > gDriverLastStepY) {
        // --- y axis goes high ---
        if (gDriverCurrentDirY == 0) {
          gDriverOutputStepY += STEP_NEXT;
        } else {
          gDriverOutputStepY -= STEP_NEXT;
        }
        SET_AXIS_Y_FASTLOW;
        if (gDriverOutputStepY == STEP_1) { SET_AXIS_Y_STEP1_FASTHIGH; }
        if (gDriverOutputStepY == STEP_2) { SET_AXIS_Y_STEP3_FASTHIGH; }
        if (gDriverOutputStepY == STEP_3) { SET_AXIS_Y_STEP5_FASTHIGH; }
        if (gDriverOutputStepY == STEP_4) { SET_AXIS_Y_STEP7_FASTHIGH; }
      }
      if (gDriverCurrentStepZ > gDriverLastStepZ) {
        // --- z axis goes high ---
        if (gDriverCurrentDirZ == 0) {
          gDriverOutputStepZ += STEP_NEXT;
        } else {
          gDriverOutputStepZ -= STEP_NEXT;
        }
        SET_AXIS_Z_FASTLOW;
        if (gDriverOutputStepZ == STEP_1) { SET_AXIS_Z_STEP1_FASTHIGH; }
        if (gDriverOutputStepZ == STEP_2) { SET_AXIS_Z_STEP3_FASTHIGH; }
        if (gDriverOutputStepZ == STEP_3) { SET_AXIS_Z_STEP5_FASTHIGH; }
        if (gDriverOutputStepZ == STEP_4) { SET_AXIS_Z_STEP7_FASTHIGH; }
      }
      // --- timeout ---
      gDriverOutputTimeout = gUptimeCurrentMillis + TIMEINTERVAL_5MS;
    }
  } else {
    // --- old signal ---
    if (gUptimeCurrentMillis > gDriverOutputTimeout) {
      // --- turn off motor ---
      SET_AXIS_X_FASTLOW;
      SET_AXIS_Y_FASTLOW;
      SET_AXIS_Z_FASTLOW;
      // --- timeout ---
      gDriverOutputTimeout = gUptimeCurrentMillis + TIMEINTERVAL_1000MS;
    }
  }
}

/*

Grbl 0.9j ['$' for help]
$0=232 (step pulse, usec)
$1=3 (step idle delay, msec)
$2=0 (step port invert mask:00000000)
$3=0 (dir port invert mask:00000000)
$4=0 (step enable invert, bool)
$5=0 (limit pins invert, bool)
$6=0 (probe pin invert, bool)
$10=3 (status report mask:00000011)
$11=0.010 (junction deviation, mm)
$12=0.002 (arc tolerance, mm)
$13=0 (report inches, bool)
$20=0 (soft limits, bool)
$21=0 (hard limits, bool)
$22=0 (homing cycle, bool)
$23=0 (homing dir invert mask:00000000)
$24=25.000 (homing feed, mm/min)
$25=500.000 (homing seek, mm/min)
$26=250 (homing debounce, msec)
$27=1.000 (homing pull-off, mm)
$100=18.655 (x, step/mm)
$101=18.655 (y, step/mm)
$102=18.655 (z, step/mm)
$110=1900.000 (x max rate, mm/min)
$111=1900.000 (y max rate, mm/min)
$112=1900.000 (z max rate, mm/min)
$120=10.000 (x accel, mm/sec^2)
$121=10.000 (y accel, mm/sec^2)
$122=10.000 (z accel, mm/sec^2)
$130=200.000 (x max travel, mm)
$131=200.000 (y max travel, mm)
$132=200.000 (z max travel, mm)
ok

*/