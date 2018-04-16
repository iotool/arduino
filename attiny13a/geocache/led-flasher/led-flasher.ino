// ATtiny13A - Geocache LED Flasher
// Brandenburger Tor, Germany (52.516270,13.377783)
// 
// This sketch signal the next geo position by morse code.
// The morse speed reduced to 5 words per minute.
// 
// PB3 led red
// PB4 led green
//
// History:
// 2018-04-17  proof-of-concept

#include "avr/pgmspace.h"
#include "hardware.h"
#include "morsecode.h"

int main() 
{
  CPU_PRESCALE_8;
  ADC_PRESCALE_8;
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;
  DIGITALWRITE_PB3_LOW;
  DIGITALWRITE_PB4_LOW;
  
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
  DIGITALWRITE_PB3_HIGH; MORSE_DELAY_DAH;
  DIGITALWRITE_PB3_LOW;  MORSE_DELAY_WORD;

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

}
