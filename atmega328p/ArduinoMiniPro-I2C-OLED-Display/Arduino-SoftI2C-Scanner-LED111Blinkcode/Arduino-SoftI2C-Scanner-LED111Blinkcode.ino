// Arduino-SoftI2C-Scanner-LED111Blinkcode.ino
//
// Detect I2C-address of OLED and display 111-blinkcode.
// 
// 1. Connect display to the Arduino
// 2. Push reset button
// 3. Short blink of LED during scan
// 4. Long signal of LED if found a device
// 5. Blink address + 111
//    1xx (1..10 = 0..900)
//    1x (1..10 = 0..90)
//    1 (1..10 = 0..9)
//    Example: 1x 7x 1x - 111 = 060 = 0x3C
// 6. Short blink of LED during scan
//
// Arduino  OLED
// GND ---- GND
// VCC ---- VCC
// A0 ----- SDA
// A1 ----- SCL
//
// Required libraries:
// https://github.com/felias-fogg/SoftI2CMaster
 
// >>> SoftI2C >>> #include <Wire.h>
#define I2C_TIMEOUT 0
#define I2C_NOINTERRUPT 0
#define I2C_FASTMODE 0
#define FAC 1
#define I2C_CPUFREQ (F_CPU/FAC)
#define SDA_PORT PORTC  // A0 SDA (not A4)
#define SDA_PIN 0
#define SCL_PORT PORTC  // A1 SCL (not A5)
#define SCL_PIN 1
#define I2C_FASTMODE 0
#include <SoftWire.h>
SoftWire Wire = SoftWire(); 
// <<< SoftI2C

void setup()
{
  Wire.begin();
  pinMode(13, OUTPUT);
}
 
 
void loop()
{
  byte error, address, blinkcode;
  int nDevices;
  
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10);                // wait for 10 millis
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(40);                // wait for 40 millis

    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);               // wait for a second
      digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);               // wait for a second
      nDevices++;

      blinkcode = address;
      while (blinkcode >= 100) {
        blinkcode -= 100;
        digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(500);
      }
      digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(1500);
      while (blinkcode >= 10) {
        blinkcode -= 10;
        digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(500);
      }
      digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(1500);
      while (blinkcode > 0) {
        blinkcode -= 1;
        digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(500);
      }
      digitalWrite(13, HIGH); delay(100); digitalWrite(13, LOW); delay(2000);
    }
    else if (error==4)
    {
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(500);               // wait for a second
      digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
      delay(500);               // wait for a second
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(500);               // wait for a second
      digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
      delay(500);               // wait for a second
    }    
  }
 
  delay(3000);           // wait 3 seconds for next scan
}
