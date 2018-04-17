// ATtiny13A - Geocache LED Flasher
// 
// This sketch signal the next geo position by morse code.
// The morse speed reduced to 8 words per minute.
// 
// Brandenburger Tor, Germany (52.516270,13.377783)
// A 5230R586N 1322R400E => 52°30'58.6"N 13°22'40.0"E
// 
// PB3 led red
// PB4 led green or beeper
// 
// Power consumption = 5 mA (48% of led current)
// 56,361 seconds per cycle
// 23,392 seconds (41,5%) led on  = 272ms * ( 14x5 digits + 4x4 char)
// 32,969 seconds (58,5%) led off = 56,361 - 23,392
//  14,177 mAs led off =  0,430 mA * 32,969 s (quiescent current)
// 270,879 mAs led on  = 11,580 mA * 23,392 s
// 285,056 mAs / 56,361 s = 5,06 mA
// 
// History:
// 2018-04-17  proof-of-concept
// 2018-04-17  cpu: 37.5 kHz / current: 5 mA @ 4.75 V

#define MORSE_PRESCALE 256

#include "avr/pgmspace.h"
#include "hardware.h"
#include "morsecode.h"
#include "resetbutton.h"

USE_RESET_BUTTON;

int main() 
{
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;
  DIGITALWRITE_PB3_LOW;
  DIGITALWRITE_PB4_LOW;
  INIT_RESET_BUTTON;

  if (gRstMem.cnt > 0) {
    // programmer mode 1.2 MHz
    CPU_PRESCALE_8;
    ADC_PRESCALE_8;
    DIGITALWRITE_PB3_HIGH;
    DIGITALWRITE_PB4_HIGH;
    while(1){};
  }

  // low power 37.5 kHz
  CPU_PRESCALE_256;
  ADC_DISABLED;
  ACD_DISABLED;
  TIMER0_DISABLED;  
  while(1) 
  {
    loop();
  }
}

void loop() 
{

  uint8_t lGeoLat[] = {5,2,3,0,MC_R,5,8,6,MC_N};   // 52°30'58.6"N (+52.516270)
  uint8_t lGeoLon[] = {1,3,2,2,MC_R,4,0,0,MC_E,0}; // 13°22'40.0"E (+13.377783)
  uint8_t lMorseLen = 0;
  uint8_t lMorseSig = 0;

  // Start
  DIGITALWRITE_PB3_HIGH; MORSE_DELAY_DIT; DIGITALWRITE_PB3_LOW;  
  MORSE_DELAY_NEXT;
  DIGITALWRITE_PB3_HIGH; MORSE_DELAY_DAH; DIGITALWRITE_PB3_LOW;  
  MORSE_DELAY_WORD;

  MORSE_DELAY_2S;

  // Lat
  for (uint8_t c=0; c<9; c++) 
  {
    lMorseLen = getMorseCode(lGeoLat[c]) & 0b00000111;
    lMorseSig = getMorseCode(lGeoLat[c]) >> 3;
    if (lGeoLat[c] > MC_R) 
    {
      c = 99;
    }
    for (uint8_t s=0; s<lMorseLen; s++) 
    {
      DIGITALWRITE_PB4_HIGH;
      if ((lMorseSig & 1) == 1) {
        MORSE_DELAY_DAH;        
      } else {
        MORSE_DELAY_DIT;        
      }
      DIGITALWRITE_PB4_LOW;
      MORSE_DELAY_NEXT;        
      lMorseSig >>= 1;  
    }
    MORSE_DELAY_NEXT_SPACE;
  }
  MORSE_DELAY_NEXT_WORD;

  // Lon
  for (uint8_t c=0; c<10; c++) 
  {
    lMorseLen = getMorseCode(lGeoLon[c]) & 0b00000111;
    lMorseSig = getMorseCode(lGeoLon[c]) >> 3;
    if (lGeoLat[c] > MC_R) 
    {
      c = 99;
    }
    for (uint8_t s=0; s<lMorseLen; s++) 
    {
      DIGITALWRITE_PB4_HIGH;
      if ((lMorseSig & 1) == 1) {
        MORSE_DELAY_DAH;        
      } else {
        MORSE_DELAY_DIT;        
      }
      DIGITALWRITE_PB4_LOW;
      MORSE_DELAY_NEXT;        
      lMorseSig >>= 1;  
    }
    MORSE_DELAY_NEXT_SPACE;
  }
  MORSE_DELAY_NEXT_WORD;

  MORSE_DELAY_2S;
}
