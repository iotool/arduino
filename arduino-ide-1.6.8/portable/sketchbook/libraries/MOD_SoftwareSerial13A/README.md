# SoftwareSerial ATtiny13A

Aduino IDE alike syntax based on Łukasz Marcin Podkalicki UART example for ATtiny13.
https://raw.githubusercontent.com/lpodkalicki/blog/master/avr/attiny13/008_software_uart/main.c

## Config

### speed

```
#define SERIAL_BEGIN   9600
```

Define the speed 9600 or 19200 baud.

### pinout

```
#define SERIAL_TX      PB3
#define SERIAL_RX      PB4
```

Define the TX and RX pin PB0, PB1, PB2, PB3 or PB4.

### buffer

```
#define SERIAL_BUFFER  16
```

Define 16 bytes for serial buffer (minimum 8 byte).

### library

```
#include <SoftwareSerial13A.h>
```

Include the SoftwareSerial library for ATtiny13A.

## Output

```
SERIAL_PRINT("Text");
SERIAL_PRINT("Line\n");
SERIAL_PRINT(123);
```

Send data to serial output.

## Input

```
SERIAL_READLN;
if (SERIAL_DATA) {
  SERIAL_PRINT(serialBuffer);
}
```
