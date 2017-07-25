// APPCL : AppControl.h (Workflow)

#include "AppInteraction.h"

class AppControl: private AppInteraction {
  private:
    void _init();
    void _timer();
    unsigned char _clInit;
    unsigned int _clTimer;
  protected:
    void clInit() { uiInit(); _init(); }
    void clTimer() { uiTimer(); _timer(); }
  public:
    AppControl();
    void init() { clInit(); }
    void timer() { clTimer(); }
};

