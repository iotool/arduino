// ATtiny13A_SerialProtocol.ino
// POC for UnoArduSim
// 
// 13 Byte SerialBuffer
// 32 Byte MemoryBuffer

#define SERIAL_BEGIN   9600
#define SERIAL_TX      PB3
#define SERIAL_RX      PB4
#define SERIAL_BUFFER  13
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
#define AT_COMMAND_MEMORY 'M'
#define AT_COMMAND_MEMORY_CLEAR 'C'
#define AT_COMMAND_MEMORY_GET 'G'
#define AT_COMMAND_MEMORY_SET 'S'
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
        // AT
        if (serialBuffer[4]=='\n' && serialBuffer[3]==AT_COMMAND_VERSION) {
          SERIAL_PRINT("V:1.0\n"); 
          rc=1; 
        }
        // AT+V
        else if (serialBuffer[4]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY) {
          SERIAL_PRINT("M:"); SERIAL_PRINT(AT_MEMORY_BUFFER); SERIAL_PRINT("\n"); 
          rc=1;
        }
        // AT+M
        else if (serialBuffer[5]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_CLEAR) {
          for (rc=0; rc<AT_MEMORY_BUFFER; rc++) {memoryBuffer[rc]=0;} 
          rc=1; 
        }
        // AT+MG00
        else if (serialBuffer[7]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_GET) {
          rc = (serialBuffer[5]-48)*10+(serialBuffer[6]-48);
          SERIAL_PRINT("MG"); SERIAL_PRINT(rc); SERIAL_PRINT(":"); SERIAL_PRINT(memoryBuffer[rc]); SERIAL_PRINT("\n");
          rc = 1;
        }
        // AT+MS00=123
        else if (serialBuffer[7]=='=' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_SET) {
          rc = (serialBuffer[5]-48)*10+(serialBuffer[6]-48);
          if (serialBuffer[9]=='\n')  {memoryBuffer[rc] = (serialBuffer[8]-48);} 
          else if (serialBuffer[10]=='\n') {memoryBuffer[rc] = (serialBuffer[8]-48)*10+(serialBuffer[9]-48);} 
          else {memoryBuffer[rc] = (serialBuffer[8]-48)*100+(serialBuffer[9]-48)*10+(serialBuffer[10]-48);} 
          rc = 1;
        }
      }
      if (rc == 1) {sendOK();} else {sendERR();}
    }
  }
}

