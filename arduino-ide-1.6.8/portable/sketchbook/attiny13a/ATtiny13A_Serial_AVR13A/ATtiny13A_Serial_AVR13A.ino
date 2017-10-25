// ATtiny13A_Serial_AVR13A.ino
// 
// Serial interface to use ATtiny13A as a co-controller.
// 
// 2017-10-25 Memory API
// 668 Bytes (65%) of 1.024 Bytes Sketch
//  50 Bytes (78%) of 64 Bytes RAM
// 
// $ AT
// $ AT+MGnn
// $ AT+MSnn=nnn
// 

#include <avr/pgmspace.h>

/* SoftwareSerial */
#define SERIAL_BEGIN   9600
#define SERIAL_TX      PB3
#define SERIAL_RX      PB4
#define SERIAL_BUFFER  16
#include "SoftwareSerial13A.h"

/* SerialProtocol */
#define AT_COMMAND_VERSION      'V' /* AT+V        */
#define AT_COMMAND_MEMORY       'M' /* AT+M        */
#define AT_COMMAND_MEMORY_CLEAR 'C' /* AT+MC       */
#define AT_COMMAND_MEMORY_GET   'G' /* AT+MGnn     */
#define AT_COMMAND_MEMORY_SET   'S' /* AT+MSnn=nnn */
#define AT_MEMORY_BUFFER 32

byte memoryBuffer[AT_MEMORY_BUFFER] = {0};

int
main(void)
{
  /* setup */
  SERIAL_PRINT(F("HI\n"));

  /* loop */
  while (1) {
    SERIAL_READLN;
    // >>>
    uint8_t rc;
    if (SERIAL_DATA) {
      if (serialBuffer[0]=='A' && serialBuffer[1]=='T') {
        SERIAL_PRINT(serialBuffer);
        if (serialBuffer[2]=='\n') {rc=2;} 
        else if (serialBuffer[2]=='+') {
          if (0) {}
          /* ---
          // AT+V           getVersion              
          else if (serialBuffer[4]=='\n' && serialBuffer[3]==AT_COMMAND_VERSION) {
            SERIAL_PRINT(F("V:1.0"));
            rc=1; 
          }
          // AT+M            getMemory
          else if (serialBuffer[4]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY) {
            SERIAL_PRINT(F("M:")); SERIAL_PRINT(AT_MEMORY_BUFFER);
            rc=1;
          }
          // AT+MC           clearMemory
          else if (serialBuffer[5]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_CLEAR) {
            for (rc=0; rc<AT_MEMORY_BUFFER; rc++) {memoryBuffer[rc]=0;} 
            rc=2;
          }
          --- */ 
          // AT+MG00          getMemory[00]
          else if (serialBuffer[7]=='\n' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_GET) {
            rc = (serialBuffer[5]-48)*10+(serialBuffer[6]-48);
            SERIAL_PRINT(F("MG")); SERIAL_PRINT(rc); SERIAL_PRINT(F(":")); SERIAL_PRINT(memoryBuffer[rc]); 
            rc = 1;
          }
          // AT+MS00=123      setMemory[00] = 123
          else if (serialBuffer[7]=='=' && serialBuffer[3]==AT_COMMAND_MEMORY && serialBuffer[4]==AT_COMMAND_MEMORY_SET) {
            rc = (serialBuffer[5]-48)*10+(serialBuffer[6]-48);
            if (serialBuffer[9]=='\n')  {memoryBuffer[rc] = (serialBuffer[8]-48);} 
            else if (serialBuffer[10]=='\n') {memoryBuffer[rc] = (serialBuffer[8]-48)*10+(serialBuffer[9]-48);} 
            else {memoryBuffer[rc] = (serialBuffer[8]-48)*100+(serialBuffer[9]-48)*10+(serialBuffer[10]-48);} 
            rc = 2;
          }
        }
        if (rc == 0) {SERIAL_PRINT(F("ERR"));}
        if (rc != 2) {SERIAL_PRINT(F("\n"));}
        if (rc >= 1) {SERIAL_PRINT(F("OK\n"));}
      }
    }
  }
  // <<<
}

