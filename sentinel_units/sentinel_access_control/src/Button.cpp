#include "Button.h"
#include "Filesystem.h"
#include "Network.h"
#include "Mcp.h"
// #include "Lock.h" // !!!
#include "pins.h"
#include "prefs.h"
#include "events.h"
#include "defaults.h"

Button::Button(int id) : _id(id) {
  switch(_id) {
    case 1:
      _halt = false;
      _enabled = filesystem->getButton1(); // !
      _timeout = filesystem->getButton1Timeout();
      _debounce = DEFAULT_BUTTON_DEBOUNCE;
      _last_time = 0;
      _trigger = true;
      _check = true;
      _pin = MCP_BUTTON_1;
      mcp->inst()->pinMode(MCP_BUTTON_1, INPUT); // external pull-up resistor
      mcp->inst()->setupInterruptPin(MCP_BUTTON_1, CHANGE);
      _event_button = event_button_1;
      _event_button_state_trigger = event_button_1_state_trigger;
      break;
    case 2:
      _halt = false;
      _enabled = filesystem->getButton2(); // !
      _timeout = filesystem->getButton2Timeout();
      _debounce = DEFAULT_BUTTON_DEBOUNCE;
      _last_time = 0;
      _trigger = true;
      _check = true;
      _pin = MCP_BUTTON_2;
      mcp->inst()->pinMode(MCP_BUTTON_2, INPUT); // external pull-up resistor
      mcp->inst()->setupInterruptPin(MCP_BUTTON_2, CHANGE);
      _event_button = event_button_2;
      _event_button_state_trigger = event_button_2_state_trigger;
      break;
  }
}

void Button::success(bool callOthers, int timeoutOverride, Lock* lock, Button* secondButton, Reader* firstReader, Reader* secondReader) {
  _ticker.detach();
  _halt = true;
  int timeout = _timeout;
  if (callOthers) {
    secondButton->success(false, _timeout, lock, this, firstReader, secondReader);
    firstReader->success(false, _timeout, lock, secondReader);
    secondReader->success(false, _timeout, lock, firstReader);
    lock->timedOpen(_timeout);
  } else {
    timeout = timeoutOverride;
  }
  _ticker.once_ms(timeout, +[](Button* button) {
    button->setHalt(false);
  }, this);
}

bool Button::getHalt() {
  return _halt;
}

void Button::setHalt(bool halt) {
  _halt = halt;
}

void Button::trigger() {
  if (!_trigger) _trigger = true;
}

void Button::bypass() {
  _ticker.detach();
  _halt = true;
}

void Button::block() {
  _ticker.detach();
  _halt = true;
}

void Button::normal() {
  _ticker.detach();
  _halt = false;
}

Ticker Button::getTicker() {
  return _ticker;
}

void Button::_logButtonTrigger() {
  // network->publishMqttEventMessage(_event_button_state_trigger, _event_button, false, "", false, "", false, "", event_level_info, event_type_access);
  network->publishMqttEventMessage(_event_button_state_trigger, _event_button, "", "", "", event_level_info, event_type_access);
}

void Button::handle(Lock* lock, Button* secondButton, Reader* firstReader, Reader* secondReader) {
  if (_trigger) {
    int current_state = mcp->inst()->digitalRead(_pin);
    if (current_state != _last_state) {
      _last_time = millis();
    }
    if (((millis() - _last_time) >= _debounce) || _check) {
      if ((current_state != _state) || _check) {
        _state = current_state;
        if (_state == LOW) {
          if (!_halt) {
            success(true, -1, lock, secondButton, firstReader, secondReader);
            _logButtonTrigger();
          }
        }
      }
      _trigger = false;
      _check = false;
    }
    _last_state = current_state;
  }
}
