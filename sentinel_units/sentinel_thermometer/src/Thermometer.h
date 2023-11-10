#pragma once

#include <Arduino.h>
#include "Wire.h"
#include "SHT31.h"

#define SHT31_ADDRESS   0x44

class Thermometer {
  public:
    Thermometer(int id);
    void handle();
    void publishMqttInit();
    bool isConnected();
  private:
    String _formatValue(float value);
    SHT31* _sensor;
    int _id;
    int _check_period;
    unsigned long _check_last_time;
    bool _connected;
    float _temperature;
    float _humidity;
    const char * _mqtt_topic_status;
    const char * _mqtt_payload_status_connected;
    const char * _mqtt_payload_status_disconnected;
    const char * _mqtt_topic_temperature;
    const char * _mqtt_topic_humidity;
    const char * _mqtt_payload_temperature_na;
    const char * _mqtt_payload_humidity_na;
    String _event_temperature;
    String _event_humidity;
};
