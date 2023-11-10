#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Oled {
  public:
    Oled();
    void init();
    void setup();
    void drawUpdating();
    void setIpAddress(String value);
    void setNetworkConnected(bool value);
    void setMqttConnected(bool value);
  private:
    void _clear();
    void _drawBooting();
    void _draw();
    Adafruit_SSD1306* _display;
    bool _initialized;
    String _moduleType;
    String _macAddress;
    String _ipAddress;
    // bool _networkConnected;
    // bool _mqttConnected;
};

extern Oled* oled;
