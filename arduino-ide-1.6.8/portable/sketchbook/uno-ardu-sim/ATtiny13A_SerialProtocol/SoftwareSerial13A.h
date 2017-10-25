// SoftwareSerial13A.h
// Equivalent syntax for UnoArduSim

char serialBuffer[SERIAL_BUFFER] = {0};
byte serialPosition = 0;
bool SERIAL_DATA = false;

void SERIAL_PRINT(const char* s) {
  Serial.print(s);
}

void SERIAL_READLN() {
  char serialRead = ' ';
  byte serialNoop = 255;
  serialPosition = 0;
  SERIAL_DATA = false;
  if (Serial.available()) {
    while(((serialRead = (char)Serial.read()) != '\n') && (serialPosition < sizeof(serialBuffer)) && (serialNoop > 0)) {
      if (serialRead >= 0) {
        serialBuffer[serialPosition] = serialRead;
        serialPosition++;
        SERIAL_DATA = true;
        serialNoop = 255;
      } else {
        serialNoop--;
      }
    }
    if ((serialNoop == 0) && (serialPosition+1 < sizeof(serialBuffer))) {
      if (serialBuffer[serialPosition] != 'n') {
        serialBuffer[serialPosition] = '\n';
      }
    }
  }
}

