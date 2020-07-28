// Arduino-SoftI2C-SSD1306Ascii-OLED.ino
//
// Display text on OLED with GPIO A0 and A1 by SoftI2C.
// 
// Arduino  OLED
// GND ---- GND
// VCC ---- VCC
// A0 ----- SDA
// A1 ----- SCL
//
// Required libraries:
// https://github.com/felias-fogg/SoftI2CMaster
// https://github.com/greiman/SSD1306Ascii
//
// Required modifications:
// SSD1306AsciiWire.h
// + #ifndef _SoftWire_h
//   #include <Wire.h>
// + #endif  // _SoftWire_h
// SSD1306Ascii.h
// + #ifndef AVRI2C_FASTMODE
//   #define AVRI2C_FASTMODE 1
// + #endif  // AVRI2C_FASTMODE
// 

// >>> SoftI2C >>> #include <Wire.h>
#define I2C_TIMEOUT 0
#define I2C_NOINTERRUPT 0
#define I2C_FASTMODE 0
#define FAC 1
#define I2C_CPUFREQ (F_CPU/FAC)
#define SDA_PORT PORTC  // SDA (regular A4)
#define SDA_PIN 0
#define SCL_PORT PORTC  // SCL (regular A5)
#define SCL_PIN 1
#define I2C_FASTMODE 0
#include <SoftWire.h>
SoftWire Wire = SoftWire(); 
// <<< SoftI2C
#define AVRI2C_FASTMODE 0
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;
//------------------------------------------------------------------------------
void setup() {
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!");
}
//------------------------------------------------------------------------------
void loop() {}
