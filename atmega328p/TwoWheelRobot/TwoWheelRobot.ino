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
// 2018-03-26  refactoring
// 

#include "TwoWheelRobot_Hardware.h"
#include "TwoWheelRobot_Runtime.h"
#include "TwoWheelRobot_Dynamic.h"

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
  SERVO_CENTER;

  // 
  // wait until start
  // 
  LED_ON;  
  delay(1000);
  LED_OFF;

  // 
  // initial scan area
  // 
  initSonarDouble();
}

void loop() 
{
  delay(100);
  
  // 
  // search for largest distance
  // 
  scanSonarDouble();
  byte max_i = 0;
  long max_distance = 0, distance = 0; 
  for (byte i=0; i<=4; i++) 
  {
    distance = gSonarAreaAvg[i]; 
    if (distance > max_distance) {
      max_distance = distance;
      max_i = i;
    }
    Serial.println(distance); delay(100); 
  }

  // 
  // move direction to largest distance
  // 
  if (distance <= 200) {
    if ((max_i < 2) || (max_i > 2)) {
      if (max_i < 2) {
        Serial.print("move right "); Serial.println(10*(2-max_i)); delay(100); 
        turnMotorOn();
        MOTOR_RIGHT;
        while (runMotionByCount(10*(2-max_i),0)) {delay(1);}
        turnMotorOff();
      }
      if (max_i > 2) {
        Serial.print("move left "); Serial.println(10*(max_i-2)); delay(100); 
        turnMotorOn();
        MOTOR_TOOGLE;
        MOTOR_LEFT;
        while (runMotionByCount(10*(max_i-2),0)) {delay(1);}
        turnMotorOff();
      }
    }
    scanSonarCenter();
    distance = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90]; 
  }

  // 
  // drive forward
  // 
  if (distance > 200) 
  {
    // forward
    turnMotorOn();
    MOTOR_TOOGLE;
    MOTOR_FORWARD;
    uint32_t lTimout = 10000;
    uint8_t lCount = 0;
    while (runMotionByTime(lTimout,1))
    {
      if (lCount < 250) {
        // detect deadlock
        lCount++;
      }
      scanSonarCenter();
      distance = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90]; 
      if (distance < 150) 
      {
        lTimout = 0;
      }
      Serial.print("distance "); Serial.print(distance); Serial.print(" mm ; speed "); 
      Serial.print(gTaskData.speedMotionMmPerSec); Serial.println(" mm/s");
      delay(distance/8);
      delay(10);
    }
    turnMotorOff();
    // u-turn (deadlock)
    if ((distance < 5) || (lCount<=3)) {
      Serial.println("FAIL: MOTION DEADLOCK!");
      turnMotorOn();
      MOTOR_BACKWARD;
      while (runMotionByCount(10,0)) {delay(1);}
      turnMotorOff();
      delay(100);
      turnMotorOn();
      MOTOR_RIGHT;
      while (runMotionByCount(10,0)) {delay(1);}
      turnMotorOff();      
    }
  }

  //
  // backward (blocked)
  // 
  if (gTaskData.speedMotionBlocked == 1) 
  {
    Serial.println("FAIL: MOTION BLOCKED!");
    delay(1000);
    turnMotorOn();
    MOTOR_BACKWARD;
    while (runMotionByCount(30,0)) {delay(1);}
    turnMotorOff();
  }  

}
