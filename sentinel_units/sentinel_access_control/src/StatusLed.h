#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

class StatusLed {
  public:
    StatusLed();
    void init();
    void setBrightness(uint8_t scale);
    void off();
    void setBooting(bool value);
    void setUpdating(bool value);
    void setNetworkConnected(bool value);
    void setMqttConnected(bool value);
    void setConfigMode(bool value);
    void setApConnected(bool value);
  private:
    void _update();
    void _cyan();
    void _orange();
    void _white();
    void _red();
    void _green();
    void _blue();
    void _magenta();
    void _yellow();

    bool _getColorBlink();
    void _setColorBlink(bool value);

    void _blinkBooting(int ms);
    void _blinkUpdating(int ms);
    void _blinkOff();
    bool _color_blink;

    bool _is_booting;
    bool _is_updating;
    bool _network_connected;
    bool _mqtt_connected;
    bool _config_mode;
    bool _ap_connected;

    Adafruit_NeoPixel* _leds;
    uint8_t _brightness;

    Ticker _ticker;
};

extern StatusLed* statusLed;
