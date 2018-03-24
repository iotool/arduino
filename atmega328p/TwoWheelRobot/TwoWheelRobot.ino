//////////////////////////////////////////////////////////////////////
//                                                                  //
//                    Arduino Two-Wheel Robot                       //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////
// 
//  This sketch use the Arduino 2WD Robot kit and a RPM sensor.
//  The speed align to 200 mm/s depending on battery level.
//  I implemented a collision detection by the speed sensor.
//  The RPM speed sensor handle timer1 as a background task.
//  The Ultrasonic distance sensor handle by main loop.
// 

#include "TwoWheelRobot_Hardware.h"
#include "TwoWheelRobot_Runtime.h"

#define CFG_ROBOT_MOTOR_PWM_INIT   850  /* 200 mm per second */
#define CFG_ROBOT_MOTOR_PWM_MIN    640  /* 170 mm per second */
#define CFG_ROBOT_MOTOR_PWM_MAX   1023  /* 310 mm per second */
#define CFG_ROBOT_SPEED_MIN        195  /* 200 mm per second */
#define CFG_ROBOT_SPEED_MAX        205  /* 200 mm per second */

uint16_t gSpeedPwm = CFG_ROBOT_MOTOR_PWM_INIT;
uint32_t gMotionStart = 0;

void setup() 
{
  // 
  // setup two wheel robot
  // 
  setupHardware();
  setupRuntime();
  LED_ON;
  Serial.print("TwoWheelRobot");
  Serial.println();
  LED_OFF;
  setMotorSpeed(gSpeedPwm);
  delay(1);
  LED_ON;  
  SERVO_CENTER;
  delay(1000);
  LED_OFF;
  SERVO_CENTER;  delay(100); 

  // 
  // align speed (turn left / turn right)
  // 
  Serial.println("AlignSpeed");
  turnMotorOn();
  MOTOR_LEFT;
  while (runMotionByCount(50,1))
  {
    Serial.print(gTaskData.speedMotionMmPerSec);
    Serial.println(" mm/s");
    delay(50);
  }
  turnMotorOff();
  if (gTaskData.speedMotionBlocked == 1) 
  {
    Serial.println("ERROR: MOTION BLOCKED!");
    while (1) 
    {
      LED_ON;  delay(100);
      LED_OFF; delay(100);
    }
  }
  delay(500);
  turnMotorOn();
  MOTOR_RIGHT;
  while (runMotionByCount(50,1))
  {
    Serial.print(gTaskData.speedMotionMmPerSec);
    Serial.println(" mm/s");
    
    delay(50);
  }
  turnMotorOff();
  if (gTaskData.speedMotionBlocked == 1) 
  {
    Serial.println("ERROR: MOTION BLOCKED!");
    while (1) 
    {
      LED_ON;  delay(100);
      LED_OFF; delay(100);
    }
  }
  delay(500);

}

void loop() 
{
  delay(1000);

  // 
  // search largest distance
  // 
  byte max_i = 0;
  long max_distance = 0, distance = 0; 
  for (byte i=(255-64); i>=63; i-=8) {
    analogWrite(PIN_SONAR_SERVO, i+CFG_SONAR_CALI);
    distance = getDistance(); 
    if (distance > max_distance) {
      max_distance = distance;
      max_i = i;
    }
    Serial.println(distance); delay(100); 
  }
  analogWrite(PIN_SONAR_SERVO, max_i+CFG_SONAR_CALI);
  delay(100); 

  // 
  // move direction
  // 
  if ((max_i < 127) || (max_i > 127)) {
    if (max_i < 127) {
      Serial.print("move right "); Serial.println(10*(max_i-62)/64); delay(100); 
      turnMotorOn();
      MOTOR_RIGHT;
      while (runMotionByCount(10*(max_i-62)/64,0)) {delay(1);}
      turnMotorOff();
    }
    if (max_i > 127) {
      Serial.print("move left "); Serial.println(10*(max_i-127)/64); delay(100); 
      turnMotorOn();
      MOTOR_TOOGLE;
      MOTOR_LEFT;
      while (runMotionByCount(10*(max_i-127)/64,0)) {delay(1);}
      turnMotorOff();
    }
  }

  // 
  // drive forward
  // 
  analogWrite(PIN_SONAR_SERVO, 127+CFG_SONAR_CALI);
  distance = getDistance(); 
  delay(100);
  if (distance > 200) 
  {
    // forward
    turnMotorOn();
    MOTOR_TOOGLE;
    MOTOR_FORWARD;
    uint32_t lTimout = 2000;
    while (runMotionByTime(lTimout,1))
    {
      distance = getDistance(); 
      if (distance < 150) 
      {
        lTimout = 0;
      }
      Serial.print("distance "); Serial.print(distance); Serial.print(" mm ; speed "); 
      Serial.print(gTaskData.speedMotionMmPerSec); Serial.println(" mm/s");
      delay(20);
    }
    turnMotorOff();
    // backward (blocked)
    if (gTaskData.speedMotionBlocked == 1) 
    {
      Serial.println("FAIL: MOTION BLOCKED!");
      delay(1000);
      turnMotorOn();
      MOTOR_BACKWARD;
      while (runMotionByCount(40,0)) {delay(1);}
      turnMotorOff();
    }
  }
}

uint32_t getDistance() 
{
  long duration, distance;
  digitalWrite(PIN_SONAR_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PIN_SONAR_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_SONAR_TRIG, LOW);
  duration = pulseIn(PIN_SONAR_ECHO, HIGH, CFG_SONAR_TIMEOUT);
  distance = duration / 5.8;
  return distance;
}

void setMotorSpeed(uint16_t pPwm)
{
  // set pwm by timer1
  Timer1.pwm(PIN_MOTOR_L_SPEED, pPwm); 
  Timer1.pwm(PIN_MOTOR_R_SPEED, pPwm); 
}

void alignMotorSpeed()
{
  // auto align speed to 200 mm per second
  if (gTaskData.speedMotionMmPerSec > 0) 
  {
    // increase speed lower than 195 mm per second
    if (gTaskData.speedMotionMmPerSec < CFG_ROBOT_SPEED_MIN) 
    {
      // minimum pwm 640
      if (gSpeedPwm < CFG_ROBOT_MOTOR_PWM_MAX) 
      {
        gSpeedPwm++;
        setMotorSpeed(gSpeedPwm);
      }
    }
    // decrease speed higher than 205 mm per second
    else if (gTaskData.speedMotionMmPerSec > CFG_ROBOT_SPEED_MAX) 
    {
      // maximum pwm 1023
      if (gSpeedPwm > CFG_ROBOT_MOTOR_PWM_MIN) 
      {
        gSpeedPwm--;
        setMotorSpeed(gSpeedPwm);
      }
    }
  }
}

void turnMotorOn()
{
  // enable runtime
  gTaskData.speedEnabled = 1;
  gTaskData.speedMotionBlocked = 0;
  gTaskData.speedMotionCounter = 0;
  gTaskData.speedSectorCount = 0;
  gTaskData.speedStopAtCount = 0;
  gTaskData.speedSectorMicrosStart = micros();  
  gMotionStart = millis();  
}

void turnMotorOff()
{
  // disable runtime
  gTaskData.speedEnabled = 0;
  // stop the engine
  MOTOR_STOP;
}

uint8_t runMotion(uint8_t pAlignSpeed)
{
  // auto align speed to 200 mm per second
  if (pAlignSpeed > 0)
  {
    alignMotorSpeed();
  }

  // stop automaticly if motion blocked
  if (gTaskData.speedMotionBlocked == 1) 
  {
    return 0;
  }

  // motion
  return 1;
}

uint8_t runMotionByTime(uint32_t pMillis, uint8_t pAlignSpeed)
{
  // auto align speed to 200 mm per second
  if (pAlignSpeed > 0)
  {
    alignMotorSpeed();
  }

  // stop automaticly if motion blocked
  if (gTaskData.speedMotionBlocked == 1) 
  {
    return 0;
  }

  // stop automaticly if timeout reached
  if ((millis()-gMotionStart) >= pMillis) 
  {
    Serial.print("TIMEOUT "); 
    Serial.println(gMotionStart); 
    return 0;
  }

  // motion
  return 1;
}

uint8_t runMotionByCount(uint32_t pCount, uint8_t pAlignSpeed)
{
  // auto align speed to 200 mm per second
  if (pAlignSpeed > 0)
  {
    alignMotorSpeed();
  }

  // stop automaticly if motion blocked
  if (gTaskData.speedMotionBlocked == 1) 
  {
    return 0;
  }

  // stop automaticly if counter reached
  if (gTaskData.speedStopAtCount == 0) 
  {
    gTaskData.speedStopAtCount = pCount;
  }
  if (pCount <= gTaskData.speedSectorCount)
  {
    return 0;
  }

  // motion
  return 1;
}

