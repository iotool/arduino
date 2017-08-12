class EspUptime {
  public:
    EspUptime();
    void update();
    unsigned long seconds();
  private:
    unsigned long _seconds;
    unsigned int _thisMillis;
    unsigned int _lastMillis;
};

EspUptime::EspUptime() {
  this->_seconds = 0;
  this->_thisMillis = 0;
  this->_lastMillis = 0;
}

void EspUptime::update() {
  // must called once a minute
  this->_thisMillis = (unsigned int)(millis() % 0xFFFF);
  unsigned int deltaMillis = (unsigned int)(this->_thisMillis - this->_lastMillis);
  if (deltaMillis >= 1000) {
    this->_lastMillis = this->_thisMillis;
    while (deltaMillis >= 1000) {
      deltaMillis -= 1000;
      this->_seconds++;
    }
  }
}

unsigned long EspUptime::seconds() {
  // up to 136 years
  return this->_seconds;
}

