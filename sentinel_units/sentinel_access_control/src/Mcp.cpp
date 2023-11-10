#include "Mcp.h"
#include "Lock.h" // !!!
#include "Logger.h"
#include "pins.h"
#include "config.h"

Mcp* mcp = new Mcp();

Mcp::Mcp() {
  _mcp = new Adafruit_MCP23X17();
}

void Mcp::init() {
  pinMode(ESP_MCP_INT, INPUT); // external pull-up resistor
  if (!_mcp->begin_I2C(MCP_I2C_ADDRESS)) {
    Log->println(F("[MCP] Error"));
    while (1);
  }
  _mcp->setupInterrupts(true, false, LOW);
  _mcp->clearInterrupts(); // ?
  // init readers red leds -> boot block info
  _mcp->pinMode(MCP_READER_1_RED_LED, OUTPUT);
  _mcp->digitalWrite(MCP_READER_1_RED_LED, LOW); // ON
  _mcp->pinMode(MCP_READER_2_RED_LED, OUTPUT);
  _mcp->digitalWrite(MCP_READER_2_RED_LED, LOW); // ON

  // _interrupt_trigger = false; // !
  // attachInterrupt(digitalPinToInterrupt(ESP_MCP_INT), _interruptCallback, FALLING); // !
}

// void IRAM_ATTR Mcp::_interruptCallback() { // !
//   _interrupt_trigger = true;
// }

Adafruit_MCP23X17* Mcp::inst() {
  return _mcp;
}

void Mcp::handle(Lock* lock, Reader* firstReader, Reader* secondReader, Button* firstButton, Button* secondButton/*, EnclosureTamper* enclosureTamper, FireSensor* fireSensor*/) { // ! redo to interrupt routine -> interrupt trigger variable, poll for variable change
  if (digitalRead(ESP_MCP_INT) == LOW) { // if (_interrupt_trigger) { // !
    switch (_mcp->getLastInterruptPin()) {
      case MCP_READER_1_TAMPER:
        firstReader->trigger();
        // _mcp->clearInterrupts(); // ?
        break;
      case MCP_READER_2_TAMPER:
        secondReader->trigger();
        // _mcp->clearInterrupts(); // ?
        break;
      case MCP_BUTTON_1:
        firstButton->trigger();
        // _mcp->clearInterrupts(); // ?
        break;
      case MCP_BUTTON_2:
        secondButton->trigger();
        // _mcp->clearInterrupts(); // ?
        break;
      case MCP_DOOR_SENSOR:
        lock->trigger();
        // _mcp->clearInterrupts(); // ?
        break;
      // case MCP_ENCLOSURE_TAMPER_SENSOR:
      //   enclosureTamper->trigger();
      //   // _mcp->clearInterrupts(); // ?
      //   break;
      // case MCP_FIRE_SENSOR:
      //   fireSensor->trigger();
      //   // _mcp->clearInterrupts(); // ?
      //   break;
      default:
        // _mcp->clearInterrupts(); // ?
        break;
    }
    _mcp->clearInterrupts(); // ?
    // _interrupt_trigger = false; // !
  }
}
