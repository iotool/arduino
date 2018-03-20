//////////////////////////////////////////////////////////////////////
//                                                                  //
//               Reset-Button-Counter by IoTool                     //
//                                                                  //
//    https://github.com/iotool/arduino/tree/master/atmega328p/     //
//////////////////////////////////////////////////////////////////////

typedef struct {
  uint32_t initMemory;
  uint32_t uptimePrev;
  uint8_t  resetCount;
} tResetSafeMemory;

tResetSafeMemory gResetSafeMemory \
  __attribute__ ((__section__ (".noinit")))
;

void setup() {
  if (gResetSafeMemory.initMemory != 0xFACE800C) {
    gResetSafeMemory.initMemory = 0xFACE800C;
    gResetSafeMemory.uptimePrev = 0;
    gResetSafeMemory.resetCount = 0;
  } else {
    gResetSafeMemory.resetCount++;
  }

  Serial.begin(9600);
  delay(10);
  Serial.print("ResetCounter = "); Serial.println(gResetSafeMemory.resetCount);  
  Serial.print("UptimePrev = "); Serial.println(gResetSafeMemory.uptimePrev);  

  gResetSafeMemory.uptimePrev = 0;
}

void loop() {
  delay(1);
  if (gResetSafeMemory.uptimePrev < 60000) {
    if ((millis()-gResetSafeMemory.uptimePrev) > 1000) {
      gResetSafeMemory.uptimePrev = millis();
      Serial.print("UptimePrev = "); Serial.println(gResetSafeMemory.uptimePrev); 
    }
  }
}
