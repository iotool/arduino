#define RESET_BUTTON_MAGIC  0xDE49

#define USE_RESET_BUTTON                    \
  typedef struct {                          \
    uint16_t crc;                           \
    uint8_t  cnt;                           \
    unsigned rst:1;                         \
    unsigned :7;                            \
    uint32_t dat;                           \
  } tRstMem;                                \
  tRstMem gRstMem __attribute__             \
    ((__section__ (".noinit")))             \
  ;

#define INIT_RESET_BUTTON                   \
  if (gRstMem.crc != RESET_BUTTON_MAGIC) {  \
    gRstMem.crc = RESET_BUTTON_MAGIC;       \
    gRstMem.cnt = 0;                        \
    gRstMem.rst = 0;                        \
    gRstMem.dat = 0;                        \
  } else {                                  \
    gRstMem.cnt++;                          \
  }
