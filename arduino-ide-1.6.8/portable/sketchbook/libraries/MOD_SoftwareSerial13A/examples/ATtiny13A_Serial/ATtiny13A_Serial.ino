// ATtiny13A_Serial.ino
//
// 542 Bytes (52%) of 1.024 Bytes Sketch
// 42 Bytes (65%) of 64 Bytes RAM

/* SoftwareSerial */
#define SERIAL_BEGIN   9600
#define SERIAL_TX      PB3
#define SERIAL_RX      PB4
#define SERIAL_BUFFER  16
#include <SoftwareSerial13A.h>

int
main(void)
{
  /* setup */
  // print line to serial
  SERIAL_PRINT("ATtiny13A\n");

  /* loop */
  uint8_t i;
  uint16_t uptime = 0;
  while (1) {

    // read line from serial into buffer
    SERIAL_READLN;
    _delay_ms(10);

    // if serial buffer is not empty
    if (SERIAL_DATA) {
      SERIAL_PRINT(serialBuffer);
    }

    uptime++;
    if (uptime % 50 == 0) 
    {
      SERIAL_PRINT("Uptime:");
      SERIAL_PRINT(uptime);
      SERIAL_PRINT("\n");
    }
  }

}

