#include "StatusLed.h"
#include "config.h"
#include "pins.h"

StatusLed* statusLed = new StatusLed();

StatusLed::StatusLed() {
  _brightness = STATUS_LED_BRIGHTNESS;
  _is_booting = false;
  _is_updating = false;
  _network_connected = false;
  _mqtt_connected = false;
  _config_mode = false;
  _ap_connected = false;
  _color_blink = false;
}

void StatusLed::init() {
  Adafruit_NeoPixel pixels(STATUS_LED_COUNT, ESP_STATUS_LED_DATA, STATUS_LED_COLOR_ORDER + STATUS_LED_TYPE);
  _leds = new Adafruit_NeoPixel(pixels);
  _leds->begin();
  setBrightness(_brightness);
  off();
}

void StatusLed::setBrightness(uint8_t scale) {
  _brightness = scale;
  _leds->setBrightness(scale);
}

void StatusLed::off() {
  setBrightness(_brightness);
  _leds->clear();
  _leds->show();
}

void StatusLed::setBooting(bool value) {
  _is_booting = value;
  _update();
}

void StatusLed::setUpdating(bool value) {
  _is_updating = value;
  _update();
}

void StatusLed::setNetworkConnected(bool value) {
  _network_connected = value;
  _update();
}

void StatusLed::setMqttConnected(bool value) {
  _mqtt_connected = value;
  _update();
}

void StatusLed::setConfigMode(bool value) {
  _config_mode = value;
  _update();
}

void StatusLed::setApConnected(bool value) {
  _ap_connected = value;
  _update();
}

bool StatusLed::_getColorBlink() {
  return _color_blink;
}

void StatusLed::_setColorBlink(bool value) {
  _color_blink = value;
}

void StatusLed::_blinkBooting(int ms) {
  _ticker.detach();
  _setColorBlink(true);
  _ticker.attach_ms(ms, +[](StatusLed* led) {
    if (led->_getColorBlink()) {
      led->_setColorBlink(false);
      led->_red();
    } else {
      led->_setColorBlink(true);
      led->off();
    }
  }, this);
}

void StatusLed::_blinkUpdating(int ms) {
  _ticker.detach();
  _setColorBlink(true);
  _ticker.attach_ms(ms, +[](StatusLed* led) {
    if (led->_getColorBlink()) {
      led->_setColorBlink(false);
      led->_magenta();
    } else {
      led->_setColorBlink(true);
      led->_cyan();
    }
  }, this);
}

void StatusLed::_blinkOff() {
  _ticker.detach();
  _setColorBlink(false);
}

void StatusLed::_update() {
  if (_is_booting) {
    _blinkBooting(500);
    if (_is_updating) {
      _blinkUpdating(250);
    }
  } else {
    _blinkOff();
    if (_config_mode) {
      _white();
      if (_ap_connected) {
        _magenta();
      }
    } else {
      _orange();
      if (_network_connected) {
        _blue();
        if (_mqtt_connected) {
          _green();
        }
      }
    }
  }
}

void StatusLed::_cyan() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(0, 255, 255));
  _leds->show();
}

void StatusLed::_orange() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(255, 165, 0));
  _leds->show();
}

void StatusLed::_white() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(255, 255, 255));
  _leds->show();
}

void StatusLed::_red() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(255, 0, 0));
  _leds->show();
}

void StatusLed::_green() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(0, 255, 0));
  _leds->show();
}

void StatusLed::_blue() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(0, 0, 255));
  _leds->show();
}

void StatusLed::_magenta() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(255,0,255));
  _leds->show();
}

void StatusLed::_yellow() {
  setBrightness(_brightness);
  _leds->fill(_leds->Color(255,255,0));
  _leds->show();
}
