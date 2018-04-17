#define MOSECODE static const     \
  uint8_t cMoseCode[] PROGMEM = { \
    0b11111101, /* 0[5] ----- */  \
    0b11110101, /* 1[5] .---- */  \
    0b11100101, /* 2[5] ..--- */  \
    0b11000101, /* 3[5] ...-- */  \
    0b10000101, /* 4[5] ....- */  \
    0b00000101, /* 5[5] ..... */  \
    0b00001101, /* 6[5] -.... */  \
    0b00011101, /* 7[5] --... */  \
    0b00111101, /* 8[5] ---.. */  \
    0b01111101, /* 9[5] ----. */  \
    0b00010011, /* R[3] .-.   */  \
    0b00001010, /* N[2] -.    */  \
    0b00000011, /* S[3] ...   */  \
    0b00000001, /* E[1] .     */  \
    0b00110011  /* W[3] .--   */  \
  }

MOSECODE;

uint8_t getMorseCode(uint8_t pCode) 
{
  return pgm_read_byte(&(cMoseCode[pCode]));
}

#define MC_R 10
#define MC_N 11
#define MC_S 12
#define MC_E 13
#define MC_W 14

// 8 words per minute 1.2 MHz / 150 kHz / 37.5 kHz
#ifndef MORSE_PRESCALE
  #define MORSE_PRESCALE 8
#endif
#if MORSE_PRESCALE == 64
  #define MORSE_DELAY_DIT         _delay_ms(20)
  #define MORSE_DELAY_DAH         _delay_ms(48)
  #define MORSE_DELAY_NEXT        _delay_ms(24)
  #define MORSE_DELAY_NEXT_SPACE  _delay_ms(48)
  #define MORSE_DELAY_NEXT_WORD   _delay_ms(96)
  #define MORSE_DELAY_SPACE       _delay_ms(72)
  #define MORSE_DELAY_WORD        _delay_ms(168)
  #define MORSE_DELAY_2S          _delay_ms(300)
#elif MORSE_PRESCALE == 256
  #define MORSE_DELAY_DIT         _delay_ms(5)
  #define MORSE_DELAY_DAH         _delay_ms(12)
  #define MORSE_DELAY_NEXT        _delay_ms(6)
  #define MORSE_DELAY_NEXT_SPACE  _delay_ms(12)
  #define MORSE_DELAY_NEXT_WORD   _delay_ms(64)
  #define MORSE_DELAY_SPACE       _delay_ms(18)
  #define MORSE_DELAY_WORD        _delay_ms(42)
  #define MORSE_DELAY_2S          _delay_ms(75)
#elif MORSE_PRESCALE == 8
  #define MORSE_DELAY_DIT         _delay_ms(160)
  #define MORSE_DELAY_DAH         _delay_ms(384)
  #define MORSE_DELAY_NEXT        _delay_ms(192)
  #define MORSE_DELAY_NEXT_SPACE  _delay_ms(384)
  #define MORSE_DELAY_NEXT_WORD   _delay_ms(768)
  #define MORSE_DELAY_SPACE       _delay_ms(576)
  #define MORSE_DELAY_WORD        _delay_ms(1344)
  #define MORSE_DELAY_2S          _delay_ms(2400)
#endif 
