#pragma once

#include <Adafruit_MCP23X17.h>
#include "Lock.h" // !!!
// #include "EnclosureTamper.h"
// #include "FireSensor.h"
#include "Reader.h"
#include "Button.h"

class Mcp {
  public:
    Mcp();
    void init();
    void handle(Lock* lock, Reader* firstReader, Reader* secondReader, Button* firstButton, Button* secondButton/*, EnclosureTamper* enclosureTamper, FireSensor* fireSensor*/);
    Adafruit_MCP23X17* inst();
  private:
    // _interruptCallback(); // !
    // bool _interrupt_trigger; // !
    Adafruit_MCP23X17* _mcp = nullptr;
};

extern Mcp* mcp;
