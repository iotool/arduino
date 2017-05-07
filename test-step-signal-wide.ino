// Arduino Uno - test puls wide for step signals
// 
// Test sketch to detect minimal puls wide for fast read at 500 Hz.
// Set pin 0 to 7 HIGH and turn off after different interval.
// 
// PWM  .........|.........| 2000us interval (500 Hz)
// Pin0 . 0us HIGH (0.00%)
// Pin1 # 1us HIGH (0.05%)
// Pin2 # 5us HIGH (0.25%)
// Pin3 ## 10us HIGH (0.5%)
// Pin4 ##### 50us HIGH (2.5%)
// Pin5 ########## 100us HIGH (5%)
// Pin6 ############~### 500us HIGH (33%)
// Pin7 ############~###### 1000us HIGH (50%)
// 

#define PIN00_LOW   PORTD &= ~B00000001 /* digitalWrite(00,LOW)  */
#define PIN00_HIGH  PORTD |=  B00000001 /* digitalWrite(00,HIGH) */
#define PIN01_LOW   PORTD &= ~B00000010 /* digitalWrite(01,LOW)  */
#define PIN01_HIGH  PORTD |=  B00000010 /* digitalWrite(01,HIGH) */
#define PIN02_LOW   PORTD &= ~B00000100 /* digitalWrite(02,LOW)  */
#define PIN02_HIGH  PORTD |=  B00000100 /* digitalWrite(02,HIGH) */
#define PIN03_LOW   PORTD &= ~B00001000 /* digitalWrite(03,LOW)  */
#define PIN03_HIGH  PORTD |=  B00001000 /* digitalWrite(03,HIGH) */
#define PIN04_LOW   PORTD &= ~B00010000 /* digitalWrite(04,LOW)  */
#define PIN04_HIGH  PORTD |=  B00010000 /* digitalWrite(04,HIGH) */
#define PIN05_LOW   PORTD &= ~B00100000 /* digitalWrite(05,LOW)  */
#define PIN05_HIGH  PORTD |=  B00100000 /* digitalWrite(05,HIGH) */
#define PIN06_LOW   PORTD &= ~B01000000 /* digitalWrite(06,LOW)  */
#define PIN06_HIGH  PORTD |=  B01000000 /* digitalWrite(06,HIGH) */
#define PIN07_LOW   PORTD &= ~B10000000 /* digitalWrite(07,LOW)  */
#define PIN07_HIGH  PORTD |=  B10000000 /* digitalWrite(07,HIGH) */

void setup() {
  pinMode(0,OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(13,OUTPUT);
}

unsigned long lastMicros = 0;
byte countRounds = 0;

void loop() {
  unsigned long currentMicros = micros();
  if (currentMicros - lastMicros >= 2000) {
    lastMicros = currentMicros;
    countRounds++;
    if (countRounds == 0)   {digitalWrite(13,HIGH);}
    if (countRounds == 128) {digitalWrite(13,LOW);}
  }
  unsigned long intervalMicros = currentMicros - lastMicros;
  if (intervalMicros == 0) {
    PIN00_HIGH;
    PIN01_HIGH;
    PIN02_HIGH;
    PIN03_HIGH;
    PIN04_HIGH;
    PIN05_HIGH;
    PIN06_HIGH;
    PIN07_HIGH;
    PIN00_LOW;
  }
  if (intervalMicros >= 1   ) {PIN01_LOW;} // 1 us
  if (intervalMicros >= 5   ) {PIN02_LOW;} // 5 us
  if (intervalMicros >= 10  ) {PIN03_LOW;} // 10 us
  if (intervalMicros >= 50  ) {PIN04_LOW;} // 50 us
  if (intervalMicros >= 100 ) {PIN05_LOW;} // 100 us
  if (intervalMicros >= 500 ) {PIN06_LOW;} // 500 us
  if (intervalMicros >= 1000) {PIN07_LOW;} // 1000 us
}