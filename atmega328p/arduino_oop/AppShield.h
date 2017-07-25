// APPHW : AppShield.h (Driver)

class AppShield {
  private:
    void _init();
    void _timer();
    unsigned char _hwInit;
    unsigned int _hwTimer;
  protected:
    void hwInit() { _init(); }
    void hwTimer() { _timer(); }
  public:
    AppShield();
};

