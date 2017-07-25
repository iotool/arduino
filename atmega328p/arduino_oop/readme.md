# Arduino Firmware Layer

* FW = Main / Microcontroller (setup, infinite loop, timer interrupt)
* CL = Control / Application (workflow, business logic)
* UI = Interaction / Realtime (input, output, communication)
* HW = Hardware / Driver (hardware, libraries)

In conformity with the OSI model I designed a straightforward firmware layer model.
One of the goal is to separate the business logic from the realtime interaction and 
the hardware dependent drivers for different shields / pinouts.

UnoArduSim compatible source code without double-colon to execute method of the base class.
