// Example to control a servo motor by reset button
// 
// mode1: =90 degree (reset)
// mode2: -10 degree (decrement)
// mode3: +10 degree (increment)
// 
// Poweron turn servo to 90 degree.
// Reset 1x blink .. Reset 2x blink .. 80 degree
// Reset 2x blink .. 70 degree
// Reset 2x blink .. 60 degree
// Reset 2x blink .. Reset 3x blink  .. Reset 1x blink .. 90 degree
// Reset 1x blink .. Reset 2x blink  .. Reset 3x blink .. 100 degree
// Reset 3x blink .. 110 degree
// Reset 3x blink .. 120 degree
// 

#include "ResetButton.h"
#include <Servo.h>

Servo gServoPin3;

void setup() 
{
  SetupResetButton();

  Serial.begin(9600);
  delay(10);
  Serial.println();

  if (gResetSafeMemory.userData[0] == 0) 
  {
    gResetSafeMemory.userData[0] = 1;
    gResetSafeMemory.userData[1] = 90;
  }

  switch(gResetSafeMemory.sketchMode) 
  {
    case 0: // mode 0 ~ reset to 90 degree    
      gResetSafeMemory.userData[1] = 90;
      break;
    case 1: // mode 1 
      gResetSafeMemory.userData[1] -= 10;
      if (gResetSafeMemory.userData[1] == 0) 
      {
        gResetSafeMemory.userData[1] = 10;
      }
      break;
    case 2:
      gResetSafeMemory.userData[1] += 10;
      if (gResetSafeMemory.userData[1] == 180) 
      {
        gResetSafeMemory.userData[1] = 170;
      }
      break;
  }

  gServoPin3.attach(3);
  gServoPin3.write(gResetSafeMemory.userData[1]);

  Serial.print("Reset.sketchMode  = "); Serial.println(gResetSafeMemory.sketchMode);
  Serial.print("Reset.pushButton  = "); Serial.println(gResetSafeMemory.pushButton);
  Serial.print("Reset.resetCount  = "); Serial.println(gResetSafeMemory.resetCount);
  Serial.print("Reset.uptimePrev  = "); Serial.println(gResetSafeMemory.uptimePrev);
  Serial.print("Reset.userData[0] = "); Serial.println(gResetSafeMemory.userData[0]);
  Serial.print("Reset.userData[1] = "); Serial.println(gResetSafeMemory.userData[1]);

}

void loop() 
{
  delay(1);
}
