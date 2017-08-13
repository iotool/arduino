
class EspUptime {
  public:
    EspUptime();
    void update();
    unsigned long seconds();
  private:
    unsigned long _seconds;
    unsigned long _lastMillis;
};

EspUptime::EspUptime() {
  this->_seconds = 0;
  this->_lastMillis = 0;
}

void EspUptime::update() {
  // must called once a minute
  unsigned long thisMillis = millis();
  unsigned long deltaMillis = thisMillis - this->_lastMillis;
  if (deltaMillis >= 1000) {
    this->_lastMillis = thisMillis;
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
