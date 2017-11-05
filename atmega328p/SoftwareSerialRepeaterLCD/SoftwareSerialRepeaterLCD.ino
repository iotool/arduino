// SoftwareSerialRepeaterLCD (V20171105)
// 
// Utility to debug serial message on LCD 16x2 display.
// Baud rates 1.200, 2.400, 9.600, 19.200, 115.200 bits per second
// 
// PIN A4 = LCD.SDA
// PIN A5 = LCD.SCL
// PIN 00 = Serial.TX 115200 Baud
// PIN 01 = Serial.RX 115200 Baud
// PIN 02 = Serial.TX 1200 Baud
// PIN 03 = Serial.RX 1200 Baud
// PIN 04 = Serial.TX 2400 Baud
// PIN 05 = Serial.RX 2400 Baud
// PIN 06 = Serial.TX 19200 Baud
// PIN 07 = Serial.RX 19200 Baud
// PIN 10 = Serial.TX 9600 Baud
// PIN 11 = Serial.RX 9600 Baud

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// set the LCD address to 0x27 or 0x3F for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections: addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
// LiquidCrystal_I2C lcd(0x20, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address 0x20
// LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address 0x27
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);     // Set the LCD I2C address 0x3F

#include <SoftwareSerial.h>
SoftwareSerial mySerial_02_03(02,03);
SoftwareSerial mySerial_04_05(04,05);
SoftwareSerial mySerial_06_07(06,07);
SoftwareSerial mySerial_10_11(10,11);

void setup() 
{
  // Serial
  Serial.begin(115200);
  while (!Serial) {;}
  Serial.println(F("Serial-Repeater: TX00=115200 TX02=1200 TX04=19200 TX10=9600 BAUD (V20171105)"));

  // SoftwareSerial
  mySerial_02_03.begin(1200);  mySerial_02_03.println("");
  mySerial_04_05.begin(2400);  mySerial_04_05.println("");
  mySerial_06_07.begin(19200); mySerial_06_07.println("");
  mySerial_10_11.begin(9600);  mySerial_10_11.println("");

  // LCD 16x2
  lcd.begin(16,2);                      // initialize lcd for 16 chars 2 lines
  lcd.noBacklight();                    // turn off backlight
  lcd.setCursor(0,0);                   // Start at character 0 on line 0
  lcd.backlight();                      // turn on backlight
  lcd.print(F("Serial-Repeater:"));     // Debug 
  lcd.setCursor(0,1); lcd.print(F("TX0: 115200 BAUD")); delay(1000);
  lcd.setCursor(0,1); lcd.print(F("TX2: 1200 BAUD  ")); delay(1000);
  lcd.setCursor(0,1); lcd.print(F("TX4: 2400 BAUD  ")); delay(1000);
  lcd.setCursor(0,1); lcd.print(F("TX6: 19200 BAUD ")); delay(1000);
  lcd.setCursor(0,1); lcd.print(F("T10: 9600 BAUD  ")); delay(1000);
  lcd.setCursor(0,0);                   // Start at character 0 on line 0
  lcd.print(F("TX 115/1/2/19/9 "));     // Debug 
  lcd.setCursor(0,1);                   // Start at character 0 on line 1
  lcd.print(F("Ready..V20171105"));     // Debug 
  delay(1000);                          // Wait
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
  if (mySerial_02_03.available()) 
  {
    serialPin = 02;
    delay(100);
    while (mySerial_02_03.available() > 0) 
    {
      serialLen++;
      serialRead = (char)mySerial_02_03.read();
      if (serialLen<sizeof(serialBuffer)) {
        serialBuffer[serialLen-1] = serialRead;
      }
    }
  }
  if (mySerial_04_05.available()) 
  {
    serialPin = 04;
    delay(100);
    while (mySerial_04_05.available() > 0) 
    {
      serialLen++;
      serialRead = (char)mySerial_04_05.read();
      if (serialLen<sizeof(serialBuffer)) {
        serialBuffer[serialLen-1] = serialRead;
      }
    }
  }
  if (mySerial_06_07.available()) 
  {
    serialPin = 06;
    delay(100);
    while (mySerial_06_07.available() > 0) 
    {
      serialLen++;
      serialRead = (char)mySerial_06_07.read();
      if (serialLen<sizeof(serialBuffer)) {
        serialBuffer[serialLen-1] = serialRead;
      }
    }
  }
  if (mySerial_10_11.available()) 
  {
    serialPin = 10;
    delay(100);
    while (mySerial_10_11.available() > 0) 
    {
      serialLen++;
      serialRead = (char)mySerial_10_11.read();
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
      case 02: lcd.setCursor(0,0); lcd.print(F("TX02 1200 BAUD  ")); break;
      case 04: lcd.setCursor(0,0); lcd.print(F("TX04 2400 BAUD  ")); break;
      case 06: lcd.setCursor(0,0); lcd.print(F("TX06 19200 BAUD ")); break;
      case 10: lcd.setCursor(0,0); lcd.print(F("TX10 9600 BAUD  ")); break;
    }
    lcd.setCursor(0,1); lcd.print(F("................")); 
    lcd.setCursor(0,1);
    for (byte i=0; i<serialLen; i++) {
      lcd.write(serialBuffer[i]);
      switch(serialPin) {
        case  0: 
          mySerial_02_03.print(serialBuffer[i]); 
          mySerial_04_05.print(serialBuffer[i]); 
          mySerial_06_07.print(serialBuffer[i]); 
          mySerial_10_11.print(serialBuffer[i]); 
          break;
        case  2: 
          mySerial_04_05.print(serialBuffer[i]); 
          mySerial_06_07.print(serialBuffer[i]); 
          mySerial_10_11.print(serialBuffer[i]); 
          Serial.print(serialBuffer[i]); 
          break;
        case  4: 
          mySerial_02_03.print(serialBuffer[i]); 
          mySerial_06_07.print(serialBuffer[i]); 
          mySerial_10_11.print(serialBuffer[i]); 
          Serial.print(serialBuffer[i]); 
          break;
        case  6: 
          mySerial_02_03.print(serialBuffer[i]); 
          mySerial_04_05.print(serialBuffer[i]); 
          mySerial_10_11.print(serialBuffer[i]); 
          Serial.print(serialBuffer[i]); 
          break;
        case 10: 
          mySerial_02_03.print(serialBuffer[i]); 
          mySerial_04_05.print(serialBuffer[i]); 
          mySerial_06_07.print(serialBuffer[i]); 
          Serial.print(serialBuffer[i]); 
          break;
      }
    }
  }
}
