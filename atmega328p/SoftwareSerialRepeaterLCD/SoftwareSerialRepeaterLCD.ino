// SoftwareSerialRepeaterLCD
// 
// Utility to debug serial message on LCD 16x2 display.
// 
// PIN A4 = LCD.SDA
// PIN A5 = LCD.SCL
// PIN 01 = Serial.RX 115200 Baud
// PIN 00 = Serial.TX 115200 Baud
// PIN 11 = Serial.RX 9600 Baud
// PIN 10 = Serial.TX 9600 Baud

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// set the LCD address to 0x27 or 0x3F for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections: addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address 0x20
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address 0x27
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);     // Set the LCD I2C address 0x3F

#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);

void setup() 
{
  // Serial
  Serial.begin(115200);
  while (!Serial) {;}
  Serial.println(F("Serial-Repeater: TX00=115200 BAUD / TX10=9600 BAUD (V20171024)"));

  // SoftwareSerial
  mySerial.begin(9600);
  mySerial.println("");

  // LCD 16x2
  lcd.begin(16,2);                      // initialize lcd for 16 chars 2 lines
  lcd.noBacklight();                    // turn off backlight
  lcd.setCursor(0,0);                   // Start at character 0 on line 0
  lcd.backlight();                      // turn on backlight
  lcd.print(F("Serial-Repeater:"));     // Debug 
  lcd.setCursor(0,1);                   // Start at character 0 on line 1
  lcd.print(F("TX0: 112000 BAUD"));     // Debug 
  delay(500);                           // Wait
  lcd.setCursor(0,1);                   // Start at character 0 on line 1
  lcd.print(F("TX10:  9600 BAUD"));     // Debug 
  delay(500);                           // Wait
  lcd.setCursor(0,0);                   // Start at character 0 on line 0
  lcd.print(F("TX 0:1152 10:9.6"));     // Debug 
  lcd.setCursor(0,1);                   // Start at character 0 on line 1
  lcd.print(F("Ready..V20171024"));     // Debug 
  delay(500);                           // Wait
}

void loop() 
{
  char serialBuffer[16] = {0};
  char serialRead = 0;
  byte serialLen = 0;
  byte serialPin = 99;
  delay(1);

  // Serial.Read
  if (Serial.available()) 
  {
    serialPin = 00;
    delay(100);
    while (Serial.available() > 0) 
    {
      serialLen++;
      serialRead = (char)Serial.read();
      if (serialLen<sizeof(serialBuffer)) {
        serialBuffer[serialLen-1] = serialRead;
      }
    }
  }

  // SoftwareSerial.Read
  if (mySerial.available()) 
  {
    serialPin = 10;
    delay(100);
    while (mySerial.available() > 0) 
    {
      serialLen++;
      serialRead = (char)mySerial.read();
      if (serialLen<sizeof(serialBuffer)) {
        serialBuffer[serialLen-1] = serialRead;
      }
    }
  }

  // LCD.Debug
  if (serialPin < 99) {
    lcd.clear();
    switch(serialPin) {
      case  0: lcd.setCursor(0,0); lcd.print(F("TX00 115200 BAUD")); break;
      case 10: lcd.setCursor(0,0); lcd.print(F("TX10 9600 BAUD")); break;
    }
    lcd.setCursor(0,1); lcd.print(F("................")); 
    lcd.setCursor(0,1);
    for (byte i=0; i<serialLen; i++) {
      lcd.write(serialBuffer[i]);
      switch(serialPin) {
        case  0: mySerial.print(serialBuffer[i]); break;
        case 10: Serial.print(serialBuffer[i]); break;
      }
    }
  }
}
