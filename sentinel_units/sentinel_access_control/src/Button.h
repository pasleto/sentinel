#pragma once

#include <Ticker.h>
#include "Lock.h" // !!!
#include "Reader.h"

class Button {
  public:
    Button(int id);
    Ticker getTicker();
    bool getHalt();
    void setHalt(bool halt);
    void trigger();
    void bypass();
    void block();
    void normal();
    void success(bool callOthers, int timeoutOverride, Lock* lock, Button* secondButton, Reader* firstReader, Reader* secondReader);
    void handle(Lock* lock, Button* secondButton, Reader* firstReader, Reader* secondReader);
  private:
    void _logButtonTrigger();
    Ticker _ticker;
    bool _enabled; // !
    int _id;
    int _timeout; // ! ?
    bool _halt;
    int _pin;
    bool _trigger;
    bool _check;
    int _state;
    int _last_state;
    int _debounce;
    unsigned long _last_time;
    String _event_button;
    String _event_button_state_trigger;
};
