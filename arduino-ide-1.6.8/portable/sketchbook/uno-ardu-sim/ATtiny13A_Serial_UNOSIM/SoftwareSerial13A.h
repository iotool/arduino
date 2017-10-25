// SoftwareSerial13A.h
// Equivalent syntax for UnoArduSim

char serialBuffer[SERIAL_BUFFER] = {0};
byte serialPosition = 0;
bool SERIAL_DATA = false;

void SERIAL_PRINT(const char* s) {
  Serial.print(s);
}

  byte n = 0; int m = 0;

void SERIAL_PRINT(const int i) {
  m = i;
  for (n=0; n<sizeof(serialBuffer); n++) {
    serialBuffer[n] = 0;
  }
  n = 0;
  while(m>=10) {
    serialBuffer[n] = m % 10;
    m = (m - serialBuffer[n]) / 10;
    serialBuffer[n] += 48;
    n++;
  }
  serialBuffer[n] = 48 + m % 10;
  n++;
  while(n>0) {
    n--;
    Serial.print((char)serialBuffer[n]);
  }
}

void SERIAL_READLN() {
  char serialRead = ' ';
  byte serialNoop = 255;
  serialPosition = 0;
  SERIAL_DATA = false;
  if (Serial.available()) {
    while(((serialRead = (char)Serial.read()) != '\n') && (serialPosition < SERIAL_BUFFER) && (serialNoop > 0)) {
      if (serialRead >= 0) {
        serialBuffer[serialPosition] = serialRead;
        serialPosition++;
        SERIAL_DATA = true;
        serialNoop = 255;
      } else {
        serialNoop--;
      }
    }
    if ((serialNoop == 0) && (serialPosition < SERIAL_BUFFER)) {
      if (serialBuffer[serialPosition] != '\n') {
        serialBuffer[serialPosition] = '\n';
      }
    }
  }
}

