#pragma once

#include <Arduino.h>
#include "Lock.h"
// #include "EnclosureTamper.h"
// #include "FireSensor.h"
#include "Reader.h"
#include "Button.h"

class ConfigButton {
  public:
    ConfigButton();
    void init(Lock* lock, Reader* firstReader, Reader* secondReader, Button* firstButton, Button* secondButton/*, EnclosureTamper* enclosureTamper, FireSensor* fireSensor*/);
    void handleOnBoot(void (*configBoot)());
    void handle(void (*configBoot)());
    void enterConfig(void (*configBoot)());
    void exitConfig(void (*normalBoot)(bool coldBoot));
  private:
    int _debounce;
    int _period;
    bool _pressed;
    bool _active;
    int _last_state;
    unsigned long _last_time;
    Lock* _lock = nullptr;
    Button* _button_1 = nullptr;
    Button* _button_2 = nullptr;
    Reader* _reader_1 = nullptr;
    Reader* _reader_2 = nullptr;
    // EnclosureTamper* _enclosure_tamper = nullptr;
    // FireSensor* _fire_sensor = nullptr;
};

extern ConfigButton* configButton;
