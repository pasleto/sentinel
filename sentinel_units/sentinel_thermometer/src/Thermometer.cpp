#include "Thermometer.h"
#include "Filesystem.h"
#include "Network.h"
#include "Utils.h"
#include "pref.h"
#include "topics.h"
#include "events.h"
#include "defaults.h"

#include "Logger.h"

Thermometer::Thermometer(int id) : _id(id) {
  switch (_id) {
    case 1:
      _sensor = new SHT31();
      _temperature = filesystem->preferences()->getFloat(preference_thermometer_1_temperature, DEFAULT_THERMOMETER_TEMPERATURE);
      _humidity = filesystem->preferences()->getFloat(preference_thermometer_1_humidity, DEFAULT_THERMOMETER_HUMIDITY);

      _mqtt_topic_status = mqtt_topic_thermometer_1_status;
      _mqtt_payload_status_connected = mqtt_topic_thermometer_1_connected;
      _mqtt_payload_status_disconnected =  mqtt_topic_thermometer_1_disconnected;
      _mqtt_topic_temperature = mqtt_topic_thermometer_1_temperature;
      _mqtt_topic_humidity = mqtt_topic_thermometer_1_humidity;
      _mqtt_payload_temperature_na = mqtt_payload_thermometer_1_temperature_na;
      _mqtt_payload_humidity_na = mqtt_payload_thermometer_1_humidity_na;

      _event_temperature = event_thermometer_1_temperature;
      _event_humidity = event_thermometer_1_humidity;

      _check_last_time = 0;
      _check_period = 5000;

      _sensor->begin(SHT31_ADDRESS, &Wire);
      _connected = _sensor->isConnected();
      break;
    case 2:
      _sensor = new SHT31();
      _temperature = filesystem->preferences()->getFloat(preference_thermometer_2_temperature, DEFAULT_THERMOMETER_TEMPERATURE);
      _humidity = filesystem->preferences()->getFloat(preference_thermometer_2_humidity, DEFAULT_THERMOMETER_HUMIDITY);

      _mqtt_topic_status = mqtt_topic_thermometer_2_status;
      _mqtt_payload_status_connected = mqtt_topic_thermometer_2_connected;
      _mqtt_payload_status_disconnected =  mqtt_topic_thermometer_2_disconnected;
      _mqtt_topic_temperature = mqtt_topic_thermometer_2_temperature;
      _mqtt_topic_humidity = mqtt_topic_thermometer_2_humidity;
      _mqtt_payload_temperature_na = mqtt_payload_thermometer_2_temperature_na;
      _mqtt_payload_humidity_na = mqtt_payload_thermometer_2_humidity_na;

      _event_temperature = event_thermometer_2_temperature;
      _event_humidity = event_thermometer_2_humidity;

      _check_last_time = 0;
      _check_period = 5000;

      _sensor->begin(SHT31_ADDRESS, &Wire1);
      _connected = _sensor->isConnected();
      break;
  }
}

String Thermometer::_formatValue(float value) {
  return String(utils->round2(value));
}

bool Thermometer::isConnected() {
  return _connected;
}

void Thermometer::handle() { // TODO - saving values into preferences ???
  if (_connected) {
    if (millis() - _check_last_time >= _check_period) {
      _check_last_time = millis();
      _sensor->read();
      float temperature = _sensor->getTemperature();
      float humidity = _sensor->getHumidity();
      if (temperature != _temperature) {
        _temperature = temperature;
        network->publishMqttEventMessage("°C", _event_temperature, true, utils->round2(temperature), false, "");
        network->publishMqtt(_mqtt_topic_temperature, _formatValue(temperature).c_str(), true, /*1*/0);
      }
      if (humidity != _humidity) {
        _humidity = humidity;
        network->publishMqttEventMessage("%", _event_humidity, true, utils->round2(humidity), false, "");
        network->publishMqtt(_mqtt_topic_humidity, _formatValue(humidity).c_str(), true, /*1*/0);
      }
    }
  }
}

void Thermometer::publishMqttInit() {
  if (_connected) {
    _sensor->read();
    network->publishMqtt(_mqtt_topic_status, _mqtt_payload_status_connected, true, /*1*/0);
    network->publishMqtt(_mqtt_topic_temperature, _formatValue(_sensor->getTemperature()).c_str(), true, /*1*/0);
    network->publishMqtt(_mqtt_topic_humidity, _formatValue(_sensor->getHumidity()).c_str(), true, /*1*/0);
  } else {
    network->publishMqtt(_mqtt_topic_status, _mqtt_payload_status_disconnected, true, /*1*/0);
    network->publishMqtt(_mqtt_topic_temperature, _mqtt_payload_temperature_na, true, /*1*/0);
    network->publishMqtt(_mqtt_topic_humidity, _mqtt_payload_humidity_na, true, /*1*/0);
  }
}

