// APPCL : AppControl.h (Workflow)
// 
// v003: loop

#include "AppInteraction.h"

class AppControl: public AppInteraction {
  private:
    /* --- class (internal) --- */
    void _boot();
    void _init();
    void _loop();
    unsigned int _clLoop;
  protected:
    /* --- package (internal) --- */
  public:
    /* --- application (external) --- */
    AppControl();
    void init() { uiInit(); _init(); }
    void loop() { uiLoop(); _loop(); }
};

