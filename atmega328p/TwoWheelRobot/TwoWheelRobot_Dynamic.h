//////////////////////////////////////////////////////////////////////
//                                                                  //
//                    Arduino Two-Wheel Robot                       //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////
// 
// high level distance interface
// high level motor interface
// 
// 2018-03-26  refactoring
// 

#define CFG_ROBOT_MOTOR_PWM_INIT   850  /* 200 mm per second */
#define CFG_ROBOT_MOTOR_PWM_MIN    640  /* 170 mm per second */
#define CFG_ROBOT_MOTOR_PWM_MAX   1023  /* 310 mm per second */
#define CFG_ROBOT_SPEED_MIN        195  /* 200 mm per second */
#define CFG_ROBOT_SPEED_MAX        205  /* 200 mm per second */
#define CFG_ROBOT_SONAR_AREA_R170    0  /* degree pwm 199 right */
#define CFG_ROBOT_SONAR_AREA_R130    1  /* degree pwm 163 right */
#define CFG_ROBOT_SONAR_AREA_C90     2  /* degree pwm 127 center */
#define CFG_ROBOT_SONAR_AREA_C90A    5  /* degree pwm 127 center */
#define CFG_ROBOT_SONAR_AREA_C90B    6  /* degree pwm 127 center */
#define CFG_ROBOT_SONAR_AREA_L50     3  /* degree pwm  91 left */
#define CFG_ROBOT_SONAR_AREA_L10     4  /* degree pwm  55 left */

uint16_t gSpeedPwm = CFG_ROBOT_MOTOR_PWM_INIT;
uint32_t gMotionStart = 0;
uint16_t gSonarAreaRaw[7] = {0};
uint16_t gSonarAreaAvg[5] = {0};

void initSonarDouble();                  // initial double scan
void scanSonarDouble();                  // double scan
void scanSonarCenter();                  // center scan
void clearSonar();                       // reset scan values
void scanSonarAverage(uint8_t pWeight);  // calculate average
void scanSonarLeftArea();                // scan left area
void scanSonarRightArea();               // scan right area
void scanSonarFullArea();                // scan left and right
uint32_t getDistance();                  // calculate distance cm

void setMotorSpeed(uint16_t pPwm);       // adjust speed by puls wide
void alignMotorSpeed();                  // auto adjust speed 200 mm/s
void turnMotorOn();                      // motor on
void turnMotorOff();                     // motor off
uint8_t runMotion(uint8_t pAlignSpeed);  // motion endless
uint8_t runMotionByTime(uint32_t pMillis, uint8_t pAlignSpeed);
uint8_t runMotionByCount(uint32_t pCount, uint8_t pAlignSpeed);

// 
// sonar
// 

void initSonarDouble()
{
  clearSonar();
  scanSonarFullArea(); scanSonarAverage(1);
  scanSonarFullArea(); scanSonarAverage(2);
}

void scanSonarDouble()
{
  scanSonarFullArea(); scanSonarAverage(3);
  // all right vs. all left
  if ( (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]<gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]) 
    && (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170]<gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) ) {
    scanSonarRightArea(); scanSonarAverage(2);
  } else 
  if ( (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]>gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]) 
    && (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170]>gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) ) {
    scanSonarLeftArea(); scanSonarAverage(2);
  } else
  // sum right vs. sum left
  if ( (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]+gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170])
     < (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]+gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) ) {
    scanSonarRightArea(); scanSonarAverage(2);
  } else 
  if ( (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]+gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170])
     > (gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]+gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) ) {
    scanSonarLeftArea(); scanSonarAverage(2);
  } else 
  // right vs. left
  if ( gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]<gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]) {
    scanSonarRightArea(); scanSonarAverage(2);
  } else 
  if ( gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R130]>gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L50]) {
    scanSonarLeftArea(); scanSonarAverage(2);
  } else 
  if ( gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170]<gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) {
    scanSonarRightArea(); scanSonarAverage(2);
  } else 
  if ( gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_R170]>gSonarAreaAvg[CFG_ROBOT_SONAR_AREA_L10]) {
    scanSonarLeftArea(); scanSonarAverage(2);
  } else {
    scanSonarFullArea(); scanSonarAverage(2);
  }
}

void scanSonarCenter()
{
  for (byte n=0; n<=4; n++) {
    gSonarAreaRaw[n] = 0;
  }
  gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = getDistance();
  scanSonarAverage(2);
}

void clearSonar()
{
  for (byte n=0; n<=4; n++) {
    gSonarAreaRaw[n] = 0;
    gSonarAreaAvg[n] = 0;
  }
}

void scanSonarAverage(uint8_t pWeight)
{
  for (byte n=0; n<=4; n++) {
    if (gSonarAreaAvg[n]==0) {
      gSonarAreaAvg[n] = gSonarAreaRaw[n];
    } else if (gSonarAreaRaw[n] > 0) {
      gSonarAreaAvg[n] *= (pWeight-1);
      gSonarAreaAvg[n] += gSonarAreaRaw[n];
      gSonarAreaAvg[n] /= pWeight;
    }
  }
}

void scanSonarLeftArea()
{
  // surrounding scan left 10/50/90 degree
  byte i,p,d,n;
  for (i=0; i<=5; i++) {
    switch(i) { // 163,199
      case 0: p=169; d=0;   n=CFG_ROBOT_SONAR_AREA_R130; break; // 130 degree (right)
      case 1: p=211; d=0;   n=CFG_ROBOT_SONAR_AREA_R170; break; // 170 degree (right)
      case 2: p=127; d=0;   n=CFG_ROBOT_SONAR_AREA_C90A; break; //  90 degree (center)
      case 3: p=91;  d=90;  n=CFG_ROBOT_SONAR_AREA_L50;  break; //  50 degree (left)
      case 4: p=55;  d=95;  n=CFG_ROBOT_SONAR_AREA_L10;  break; //  10 degree (left) 
      case 5: p=127; d=125; n=CFG_ROBOT_SONAR_AREA_C90B; break; //  90 degree (center)
    }
    if (d > 0) {
      analogWrite(PIN_SONAR_SERVO,p+CFG_SONAR_CALI); 
      delay(d);
      gSonarAreaRaw[n] = getDistance();
    } else {
      gSonarAreaRaw[n] = 0;
    }
  }
  gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90B];
}

void scanSonarRightArea()
{
  // surrounding scan right 90/130/170 degree
  byte i,p,d,n;
  for (i=0; i<=5; i++) {
    switch(i) {
      case 0: p=171; d=100; n=CFG_ROBOT_SONAR_AREA_R130; break; // 130 degree (right)
      case 1: p=215; d=115; n=CFG_ROBOT_SONAR_AREA_R170; break; // 170 degree (right)
      case 2: p=127; d=155; n=CFG_ROBOT_SONAR_AREA_C90A; break; //  90 degree (center)
      case 3: p=91;  d=0;   n=CFG_ROBOT_SONAR_AREA_L50;  break; //  50 degree (left)
      case 4: p=55;  d=0;   n=CFG_ROBOT_SONAR_AREA_L10;  break; //  10 degree (left) 
      case 5: p=127; d=0;   n=CFG_ROBOT_SONAR_AREA_C90B; break; //  90 degree (center) 
    }
    if (d > 0) {
      analogWrite(PIN_SONAR_SERVO,p+CFG_SONAR_CALI); 
      delay(d);
      gSonarAreaRaw[n] = getDistance();
    } else {
      gSonarAreaRaw[n] = 0;
    }
  }
  gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90A];
}

void scanSonarFullArea()
{
  // surrounding scan 10/50/90/130/170 degree
  byte i,p,d,n;
  for (i=0; i<=5; i++) {
    switch(i) {
      case 0: p=171; d=100; n=CFG_ROBOT_SONAR_AREA_R130; break; // 130 degree (right)
      case 1: p=215; d=115; n=CFG_ROBOT_SONAR_AREA_R170; break; // 170 degree (right)
      case 2: p=127; d=155; n=CFG_ROBOT_SONAR_AREA_C90A; break; //  90 degree (center)
      case 3: p=91;  d=90;  n=CFG_ROBOT_SONAR_AREA_L50;  break; //  50 degree (left)
      case 4: p=55;  d=95;  n=CFG_ROBOT_SONAR_AREA_L10;  break; //  10 degree (left) 
      case 5: p=127; d=125; n=CFG_ROBOT_SONAR_AREA_C90B; break; //  90 degree (center) 
    }
    analogWrite(PIN_SONAR_SERVO,p+CFG_SONAR_CALI); 
    delay(d);
    gSonarAreaRaw[n] = getDistance();
  }
  gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = 0;
  if ( (gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90A]>0)
    && (gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90B]>0) ) 
  {
    gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90A];
    gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] += 3*gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90B];
    gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] /= 4;
  } else if (gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90A]>0) {
    gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90A];
  } else if (gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90B]>0) {
    gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90] = gSonarAreaRaw[CFG_ROBOT_SONAR_AREA_C90B];
  }
}

uint32_t getDistance() 
{
  // ultrasonic distance
  long duration, distance;
  digitalWrite(PIN_SONAR_TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(PIN_SONAR_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_SONAR_TRIG, LOW);
  duration = pulseIn(PIN_SONAR_ECHO, HIGH, CFG_SONAR_TIMEOUT);
  distance = duration / 5.8;
  return distance;
}

// 
// motor
// 

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

