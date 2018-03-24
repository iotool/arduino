//////////////////////////////////////////////////////////////////////
//                                                                  //
//                    Arduino Two-Wheel Robot                       //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////
// 
// ___Hardware___
// controller : Arduino Uno Rev3 ATmega328P
// shield     : Arduino Sensor Shield v5.0
// h-bridge   : L298N
// battery    : AAA (4x)
// switch     : power
// wheel      : TT Motor 130 (2x)
// servo      : SG90
// ultrasonic : HC-SR04
// infrared   : RPM infrared LM393 (2x)
// wire       : 20x dupont famle-female
// capacitor  : 1000 uF (servo), 20 nF (2x motor)
// resistor   : 1 kOhm (2x motor)
// 
// ___Pinout___
// 00 ... serial
// 01 ... serial
// 02 --- sonar.echo
// 03 ~~~ servo
// 04 --- sonar.trig 
// 05 --- dc.left.backward
// 06 --- dc.left.forward
// 07 --- dc.right.forward
// 08 --- dc.right.backward
// 09 ~~~ dc.left.speed
// 10 ~~~ dc.right.speed
// 11 :
// 12 :
// 13 ... led
// A0 --- rpm.trig
// A1 --- rpm.echo
// A2 :
// A3 :
// A4 :
// A5 :

// configuration
#define CFG_TASK_INTERVAL    1000  /* micros (fix for pwm) */
#define CFG_TASK_TIMESLOTS      2  /* number (multitasking) */
#define CFG_SERIAL_BAUD      9600  /* baud (serial) */
#define PIN_LED_BUILTIN        13  /* LED ATmega328P */
#define CFG_MOTOR_SLOWLY    50000  /* micros */
#define CFG_MOTOR_BLOCKED  125000  /* micros (autostop) */
#define PIN_MOTOR_L_BACK        5  /* L298N DC-Motor (left) */ 
#define PIN_MOTOR_L_DRIVE       6  /* L298N DC-Motor (left) */ 
#define PIN_MOTOR_L_SPEED       9  /* L298N DC-Motor (left) */ 
#define PIN_MOTOR_R_BACK        8  /* L298N DC-Motor (right) */ 
#define PIN_MOTOR_R_DRIVE       7  /* L298N DC-Motor (right) */ 
#define PIN_MOTOR_R_SPEED      10  /* L298N DC-Motor (right) */ 
#define PIN_SONAR_ECHO          2  /* HCSRC04-Echo */
#define PIN_SONAR_TRIG          4  /* HCSRC04-Trigger */
#define CFG_SONAR_TIMEOUT   65000  /* HCSRC04-Timeout 65 millis */
#define CFG_SONAR_DISTANCE     25  /* HCSRC04-Distance 25 cm */
#define USE_SONAR_SERVO         1  /* flag use servo */
#define CFG_SONAR_CALI          0  /* SG90 Calibartion (center) */
#define PIN_SONAR_SERVO         3  /* SG90 Servo-Motor (pwm~) */ 
#define USE_SPEED_SENSOR        1  /* flag use rpm */
#define PIN_SPEED_TRIG         14  /* LM393 Speed-Sensor-Trigger */
#define PIN_SPEED_ECHO         A1  /* LM393 Speed-Sensor-Echo */

// interface
uint8_t gMotorToogleSide = 0;
#define LED_ON            digitalWrite(PIN_LED_BUILTIN, HIGH)
#define LED_OFF           digitalWrite(PIN_LED_BUILTIN, LOW)
#define MOTOR_L_STOP      digitalWrite(PIN_MOTOR_L_DRIVE,LOW); digitalWrite(PIN_MOTOR_L_BACK,LOW)
#define MOTOR_R_STOP      digitalWrite(PIN_MOTOR_R_DRIVE,LOW); digitalWrite(PIN_MOTOR_R_BACK,LOW)
#define MOTOR_L_FORWARD   digitalWrite(PIN_MOTOR_L_DRIVE,HIGH); digitalWrite(PIN_MOTOR_L_BACK,LOW)
#define MOTOR_R_FORWARD   digitalWrite(PIN_MOTOR_R_DRIVE,HIGH); digitalWrite(PIN_MOTOR_R_BACK,LOW)
#define MOTOR_L_BACKWARD  digitalWrite(PIN_MOTOR_L_DRIVE,LOW); digitalWrite(PIN_MOTOR_L_BACK,HIGH)
#define MOTOR_R_BACKWARD  digitalWrite(PIN_MOTOR_R_DRIVE,LOW); digitalWrite(PIN_MOTOR_R_BACK,HIGH)
#define MOTOR_TOOGLE      gMotorToogleSide++
#define MOTOR_STOP        if((gMotorToogleSide%2)==0) {MOTOR_L_STOP;MOTOR_R_STOP;} else {MOTOR_R_STOP;MOTOR_L_STOP;}
#define MOTOR_FORWARD     if((gMotorToogleSide%2)==0) {MOTOR_L_FORWARD;MOTOR_R_FORWARD;} else {MOTOR_R_FORWARD;MOTOR_L_FORWARD;}
#define MOTOR_BACKWARD    if((gMotorToogleSide%2)==0) {MOTOR_L_BACKWARD;MOTOR_R_BACKWARD;} else {MOTOR_R_BACKWARD;MOTOR_L_BACKWARD;}
#define MOTOR_LEFT        if((gMotorToogleSide%2)==0) {MOTOR_L_FORWARD;MOTOR_R_BACKWARD;} else {MOTOR_R_BACKWARD;MOTOR_L_FORWARD;}
#define MOTOR_RIGHT       if((gMotorToogleSide%2)==0) {MOTOR_L_BACKWARD;MOTOR_R_FORWARD;} else {MOTOR_R_FORWARD;MOTOR_L_BACKWARD;}
#define SERVO_CENTER      analogWrite(PIN_SONAR_SERVO, 127+CFG_SONAR_CALI)
#define SERVO_RIGHT       analogWrite(PIN_SONAR_SERVO,  79+CFG_SONAR_CALI)
#define SERVO_LEFT        analogWrite(PIN_SONAR_SERVO, 175+CFG_SONAR_CALI)

void setupHardware() 
{
  // define gpio
  pinMode(PIN_LED_BUILTIN, OUTPUT);
  pinMode(PIN_MOTOR_L_BACK, OUTPUT);
  pinMode(PIN_MOTOR_L_DRIVE, OUTPUT);
  pinMode(PIN_MOTOR_L_SPEED, OUTPUT);
  pinMode(PIN_MOTOR_R_BACK, OUTPUT);
  pinMode(PIN_MOTOR_R_DRIVE, OUTPUT);
  pinMode(PIN_MOTOR_R_SPEED, OUTPUT);
  pinMode(PIN_SONAR_ECHO, INPUT);
  pinMode(PIN_SONAR_TRIG, OUTPUT);
  pinMode(PIN_SONAR_SERVO, OUTPUT);
  pinMode(PIN_SPEED_TRIG, OUTPUT);
  pinMode(PIN_SPEED_ECHO, INPUT);  

  // init gpio
  digitalWrite(PIN_LED_BUILTIN, LOW);
  digitalWrite(PIN_MOTOR_L_BACK, LOW);
  digitalWrite(PIN_MOTOR_L_DRIVE, LOW);
  digitalWrite(PIN_MOTOR_R_BACK, LOW);
  digitalWrite(PIN_MOTOR_R_DRIVE, LOW);
  analogWrite(PIN_MOTOR_L_SPEED, 0);
  analogWrite(PIN_MOTOR_R_SPEED, 0);
  digitalWrite(PIN_SONAR_TRIG, LOW);
  digitalWrite(PIN_SPEED_TRIG, LOW);
  analogWrite(PIN_SONAR_SERVO, 127);
}

