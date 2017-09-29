// app.h

#include <EEPROM.h>
#include <avr/pgmspace.h>

static unsigned long g_appStaticUint32[APP_STATIC_UINT32_BUFFER] = {};

#define APP_BUTTON_UP     HIGH
#define APP_BUTTON_DOWN   LOW
#define APP_MODE_BUTTON   1
#define APP_MODE_START    2
#define APP_MODE_WAIT     3
#define APP_MODE_NEXT     4

static byte g_appId = 0;

unsigned long appCRC(byte app);

byte appSetup(byte count,byte button, byte led) {
  unsigned long crc = 0;
  byte app = 0;
  byte mode = APP_MODE_START;
  // EEPROM READ
  EEPROM.get(0,crc);
  EEPROM.get(4,app);
  if (crc != appCRC(app)) {
    // EEPROM INIT
    app = 0;
    crc = appCRC(app);
    EEPROM.put(0,crc);
    EEPROM.put(4,app);
  } else {
    pinMode(button,INPUT_PULLUP);
    pinMode(led,OUTPUT);
    if (digitalRead(button) == APP_BUTTON_DOWN) {
      app = 0;
      crc = appCRC(app);
      EEPROM.put(0,crc);
      EEPROM.put(4,app);
      mode = APP_MODE_BUTTON;
    }
  }
  g_appId = app;
  // START APP
  if (g_appId != 0) {
    pinMode(led,OUTPUT);
    delay(250);
    for (byte i=0;i<g_appId; i++) {
      digitalWrite(led,HIGH); delay(100);
      digitalWrite(led,LOW); delay(250);
    }  
    delay(100);
    return g_appId;
  }
  // SWITCH APP
  //# Serial.begin(9600);
  //# while (!Serial) {;}
  //# if (mode != APP_MODE_BUTTON) {
  //#   Serial.println(F("workshop - app"));
  //# }
  while(true) {
    switch(mode) {
      case APP_MODE_BUTTON:
        digitalWrite(led,HIGH);
        mode = APP_MODE_START;
        //# Serial.println(F("workshop - app"));
        break;
      case APP_MODE_START:
        if (digitalRead(button) == APP_BUTTON_UP) {
          digitalWrite(led,LOW);
          mode = APP_MODE_WAIT;
          //# Serial.print(F("choice "));
        }
        break;
      case APP_MODE_WAIT:
        if (digitalRead(button) == APP_BUTTON_DOWN) {
          digitalWrite(led,HIGH);
          mode = APP_MODE_NEXT;
          //# Serial.print(F(" > "));
        }
        break;
      case APP_MODE_NEXT:
        if (digitalRead(button) == APP_BUTTON_UP) {
          digitalWrite(led,LOW);
          app++;
          if (app > count) {
            app = 1;
          }
          crc = appCRC(app);
          EEPROM.put(0,crc);
          EEPROM.put(4,app);
          mode = APP_MODE_WAIT;
          //# Serial.println(app,HEX);
          //# Serial.print(F("choice "));
          delay(250);
          for (byte i=0;i<app; i++) {
            digitalWrite(led,HIGH); delay(100);
            digitalWrite(led,LOW); delay(250);
          }  
          delay(100);
        }
        break;
    }
    delay(50);
  }
  return 0;
}

byte appLoop() {
  return g_appId;
}

unsigned long appCRC(byte app) {
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };
  unsigned long crc = ~0L;
  byte data[1] = {0};
  data[0] = app;
  for (int i=0; i<sizeof(data); ++i) {
    crc = crc_table[(crc ^ data[i]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (data[i] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}
