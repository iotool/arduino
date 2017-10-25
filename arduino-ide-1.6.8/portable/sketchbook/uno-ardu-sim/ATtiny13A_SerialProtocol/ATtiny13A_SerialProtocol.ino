// ATtiny13A_SerialProtocol.ino
// POC for UnoArduSim

#define SERIAL_BEGIN   9600
#define SERIAL_TX      PB3
#define SERIAL_RX      PB4
#define SERIAL_BUFFER  16
#include "SoftwareSerial13A.h"

void setup() {
  Serial.begin(9600);
  SERIAL_PRINT("HI\n");
}

void sendOK() {
  SERIAL_PRINT("OK\n");
}

void sendERR() {
  SERIAL_PRINT("ERR\n");
}

#define AT_COMMAND_VERSION 'V'
#define AT_MEMORY_BUFFER 32
byte memoryBuffer[AT_MEMORY_BUFFER] = {0};

void loop() {
  SERIAL_READLN();
  if (SERIAL_DATA) {
    if (serialBuffer[0]=='A' || serialBuffer[1]=='T') {
      byte rc = 0;
      SERIAL_PRINT(serialBuffer);
      if (serialBuffer[2]=='\n') {rc=1;} 
      else if (serialBuffer[2]=='+') {
        if (serialBuffer[3] == AT_COMMAND_VERSION) {rc=1;SERIAL_PRINT("V:1\n");}
      }
      if (rc == 1) {sendOK();} else {sendERR();}
    }
  }
}

