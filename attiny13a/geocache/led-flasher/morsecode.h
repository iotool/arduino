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

// 4 words per minute
#define MORSE_DELAY_DIT         _delay_ms(300)
#define MORSE_DELAY_DAH         _delay_ms(600)
#define MORSE_DELAY_NEXT        _delay_ms(300)
#define MORSE_DELAY_NEXT_SPACE  _delay_ms(600)
#define MORSE_DELAY_NEXT_WORD   _delay_ms(1200)
#define MORSE_DELAY_SPACE       _delay_ms(900)
#define MORSE_DELAY_WORD        _delay_ms(2100)
