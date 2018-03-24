//////////////////////////////////////////////////////////////////////
//                                                                  //
//                    Arduino Two-Wheel Robot                       //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////

#include "TimerOne.h"

typedef struct {
  uint8_t  timeSlot;
  uint16_t speedMotionMmPerSec;
  uint8_t  speedStopAtCount;
  uint8_t  speedSectorCount;
  uint32_t speedSectorMicros;
  uint32_t speedSectorMicrosAvg;
  uint32_t speedSectorMicrosStart;
  unsigned speedSectorInfraredThis:1;
  unsigned speedSectorInfraredLast:1;
  unsigned speedEnabled:1;
  unsigned speedMotionBlocked:1;
  unsigned speedMotionCounter:1;
  unsigned :3;
} tTaskData;

tTaskData gTaskData = {0};

void task();
void taskSpeedSensor();

void setupRuntime() 
{
  // init timer
  Timer1.initialize(CFG_TASK_INTERVAL);
  Timer1.pwm(PIN_MOTOR_L_SPEED, 0); 
  Timer1.pwm(PIN_MOTOR_R_SPEED, 0); 
  Timer1.attachInterrupt(task); 

  // init serial
  Serial.begin(CFG_SERIAL_BAUD);
  delay(10);
  
}

void task() 
{
  // 
  // TASK background processing (use time1)
  // 

  // handle timeslot
  switch(gTaskData.timeSlot) 
  {
    case 0:
      taskSpeedSensor();
      break;
  }

  // next time slot
  gTaskData.timeSlot++;
  if (gTaskData.timeSlot >= CFG_TASK_TIMESLOTS)
  {
    gTaskData.timeSlot = 0;
  }
}

void taskSpeedSensor()
{
  // 
  // RPM speed sensor (messure rotations by infrared)
  // 
  // Detect rotation of the wheel by infrared photocell.
  // Calculate motion of the vehicle by wheel diameter.
  // Stop automaticly by collision detection.
  // 
  // speedEnabled            ~ enable/disable sensor task
  // speedSectorCount        ~ number of sector alterations
  // speedSectorInfraredThis ~ current infrared photocell
  // speedSectorInfraredLast ~ previous infrared photocell
  // speedSectorMicros       ~ micro seconds per sector
  // speedSectorMicrosAvg    ~ average micros per sector
  // speedSectorMicrosStart  ~ timestamp begin of sector
  // speedMotionMmPerSec     ~ millimeters per second
  // speedMotionBlocked      ~ motion blocked
  // 
  // 40 sectors per full rotation
  // 68 mm wheel diameteter (213.628 mm route per rotation)
  // 133 mm wheel distance (417.832 mm route per 360 turn)
  // 1.956 rotations per turn (78 sectors per full turn)
  // 

  if (gTaskData.speedEnabled == 1) 
  {
    // measure infrared photocell open or closed
    digitalWrite(PIN_SPEED_TRIG, HIGH);
    delayMicroseconds(1);
    gTaskData.speedSectorInfraredThis = digitalRead(PIN_SPEED_ECHO);
    digitalWrite(PIN_SPEED_TRIG, LOW); 

    // detect next sector by diffent state
    uint32_t lMicros = micros();
    if (gTaskData.speedSectorInfraredLast != gTaskData.speedSectorInfraredThis) 
    {
      // count number of sectors
      gTaskData.speedSectorCount++;
      gTaskData.speedSectorInfraredLast = gTaskData.speedSectorInfraredThis;

      // messure average time per sector
      gTaskData.speedSectorMicrosAvg *= 15;
      gTaskData.speedSectorMicrosAvg += lMicros - gTaskData.speedSectorMicrosStart;
      gTaskData.speedSectorMicrosAvg /= 16;
      gTaskData.speedSectorMicros *= 3;
      gTaskData.speedSectorMicros += lMicros - gTaskData.speedSectorMicrosStart;
      gTaskData.speedSectorMicros /= 4;
      gTaskData.speedSectorMicrosStart = lMicros;

      // calculate motion speed after stabilization by wheel 
      if (gTaskData.speedSectorMicrosAvg > 0) {
        if (gTaskData.speedSectorMicrosAvg >= gTaskData.speedSectorMicros) {
          if ((gTaskData.speedSectorMicrosAvg/16) > (gTaskData.speedSectorMicrosAvg-gTaskData.speedSectorMicros)) {
            gTaskData.speedMotionMmPerSec = (1000000/gTaskData.speedSectorMicrosAvg)*213.628/40; 
          }
        } else {
          if ((gTaskData.speedSectorMicrosAvg/16) > (gTaskData.speedSectorMicros-gTaskData.speedSectorMicrosAvg)) {
            gTaskData.speedMotionMmPerSec = (1000000/gTaskData.speedSectorMicrosAvg)*213.628/40; 
          }
        }
      }
    }

    // detect blocked wheels if the speed slow down
    else if ((lMicros - gTaskData.speedSectorMicrosStart) > CFG_MOTOR_SLOWLY) 
    {
      // calculate time per sector
      gTaskData.speedSectorMicros *= 3;
      gTaskData.speedSectorMicros += lMicros - gTaskData.speedSectorMicrosStart;
      gTaskData.speedSectorMicros /= 4;

      // autostop if permanently blocked more than 125 ms
      if ((lMicros - gTaskData.speedSectorMicrosStart) > 125000) {
        if (gTaskData.speedMotionBlocked == 0) {
          gTaskData.speedMotionBlocked = 1;
          MOTOR_STOP;
          delayMicroseconds(1);
        }
      }
    }

    // autostop if counter reached
    if (gTaskData.speedStopAtCount > 0) 
    {
      if (gTaskData.speedStopAtCount <= gTaskData.speedSectorCount) 
      {
        gTaskData.speedMotionCounter = 1;
        MOTOR_STOP;
        delayMicroseconds(1);
      }
    }
  }
}

