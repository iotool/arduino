#include "ResetButton.h"

void setup() 
{
  SetupResetButton();
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.print("Reset.sketchMode = "); Serial.println(gResetSafeMemory.sketchMode);
  Serial.print("Reset.pushButton = "); Serial.println(gResetSafeMemory.pushButton);
  Serial.print("Reset.resetCount = "); Serial.println(gResetSafeMemory.resetCount);
  Serial.print("Reset.uptimePrev = "); Serial.println(gResetSafeMemory.uptimePrev);
}

void loop() 
{
  delay(1);
}
