#include "ConfigButton.h"
#include "defaults.h"
#include "pins.h"
#include "Web.h"
#include "Utils.h"
#include "Filesystem.h"

ConfigButton* configButton = new ConfigButton();

ConfigButton::ConfigButton() {}

void ConfigButton::init(Lock* lock, Reader* firstReader, Reader* secondReader, Button* firstButton, Button* secondButton/*, EnclosureTamper* enclosureTamper, FireSensor* fireSensor*/) {
  _debounce = DEFAULT_CONFIG_BUTTON_DEBOUNCE;
  _period = 5000;
  _pressed = false;
  _active = false;
  _last_state = 0;
  _last_time = 0;
  _lock = lock;
  _button_1 = firstButton;
  _button_2 = secondButton;
  _reader_1 = firstReader;
  _reader_2 = secondReader;
  // _enclosure_tamper = enclosureTamper;
  // _fire_sensor = fireSensor;
}

void ConfigButton::handleOnBoot(void (*configBoot)()) {
  if (digitalRead(ESP_CONFIG_BUTTON) == LOW) {
    enterConfig(configBoot);
  }
}

void ConfigButton::handle(void (*configBoot)()) { // debounce not working !!!
  int _current_state = digitalRead(ESP_CONFIG_BUTTON);
  if (_current_state != _last_state) {
    if ((millis() - _last_time) >= _debounce) {
      if (_current_state == LOW) {
        if (!_pressed) {
          _pressed = true;
        }
      } else {
        if (_pressed) {
          _active = false;
          _pressed = false;
        }
      }
      _last_state = _current_state;
      _last_time = millis();
    }
  } else {
    if (_current_state == LOW) {
      if  (!_active) {
        if (_pressed) {
          if ((millis() - _last_time) > _period) {
            _active = true;
            enterConfig(configBoot);
          }
        }
      }
    }
  }
}

void ConfigButton::enterConfig(void (*configBoot)()) {
  _lock->block();
  _button_1->block();
  _button_2->block();
  _reader_1->block();
  _reader_2->block();
  _reader_1->buzzerOnOverride(100);
  _reader_2->buzzerOnOverride(100);
  utils->printSwitchToConfigMode();
  filesystem->setConfigMode(true);
  delay(200);
  (*configBoot)();
}

void ConfigButton::exitConfig(void (*normalBoot)(bool coldBoot)) { // ?
  web->setConfigExitTrigger(false);
  // ? check aswell preference_was_configured
  if (filesystem->getConfigMode()) {
    utils->printSwitchFromConfigMode();
    filesystem->setConfigMode(false);
    (*normalBoot)(false);
  }
}
