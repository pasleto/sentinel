#include "FireSensor.h"
#include "Filesystem.h"
#include "Network.h"
#include "Mcp.h"
#include "pins.h"
#include "prefs.h"
#include "topics.h"
#include "events.h"
#include "defaults.h"

FireSensor::FireSensor() {
  _fire_sensor = filesystem->getFireSensor();
  _fire_sensor_invert = filesystem->getFireSensorInvert();
  _fire_sensor_restore = filesystem->getFireSensorRestore();
  _fire_sensor_alarm = filesystem->getFireSensorAlarm();
  _fire_sensor_pin = MCP_FIRE_SENSOR;
  mcp->inst()->pinMode(MCP_FIRE_SENSOR, INPUT); // external pull-up resistor
  mcp->inst()->setupInterruptPin(MCP_FIRE_SENSOR, CHANGE);
  _last_time = 0;
  _trigger = true;
  _check = true;
  _debounce = DEFAULT_FIRE_SENSOR_DEBOUNCE;
}

void FireSensor::trigger() {
  if (_fire_sensor) {
    if (!_trigger) _trigger = true;
  }
}

void FireSensor::_logFireSensor(bool alarm) { // ?
  if (alarm != _fire_sensor_alarm) { // !
    if (alarm) {
      network->publishMqttEventMessage(event_fire_sensor_state_alarm, event_fire_sensor_current, false, "", false, "");
      network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_alarm, true, /*1*/0);
      if (!_fire_sensor_alarm) { // ? - not needed ??
        filesystem->setFireSensorAlarm(alarm);
        _fire_sensor_alarm = alarm;
        network->publishMqttEventMessage(event_fire_sensor_state_alarm, event_fire_sensor_saved, false, "", false, "");
        network->publishMqtt(mqtt_topic_fire_sensor_saved, mqtt_payload_fire_sensor_alarm, true, /*1*/0);
      }
    } else {
      network->publishMqttEventMessage(event_fire_sensor_state_normal, event_fire_sensor_current, false, "", false, "");
      network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_normal, true, /*1*/0);
      if (_fire_sensor_restore) {
        if (_fire_sensor_alarm) { // ? - not needed ??
          filesystem->setFireSensorAlarm(alarm);
          _fire_sensor_alarm = alarm;
          network->publishMqttEventMessage(event_fire_sensor_state_normal, event_fire_sensor_saved, false, "", false, "");
          network->publishMqtt(mqtt_topic_fire_sensor_saved, mqtt_payload_fire_sensor_normal, true, /*1*/0);
        }
      }
    }
  }
}

void FireSensor::handle() {
  if (_fire_sensor) {
    if (_trigger) {
      int current_state = mcp->inst()->digitalRead(_fire_sensor_pin);
      if (current_state != _last_state) {
        _last_time = millis();
      }
      if (((millis() - _last_time) >= _debounce) || _check) {
        if ((current_state != _state) || _check) {
          _state = current_state;
          if (_fire_sensor_invert) {
            if (_state == LOW) {
              _logFireSensor(false);
            }
            if (_state == HIGH) {
              _logFireSensor(true);
            }
          } else {
            if (_state == LOW) {
              _logFireSensor(true);
            }
            if (_state == HIGH) {
              _logFireSensor(false);
            }
          }
        }
        _trigger = false;
        _check = false;
      }
      _last_state = current_state;
    }
  }
}

void FireSensor::publishMqttInit() {
  if (_fire_sensor) {
    int fire_sensor_current_state = mcp->inst()->digitalRead(_fire_sensor_pin);
    if (_fire_sensor_invert) {
      if (fire_sensor_current_state == LOW) {
        network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_normal, true, /*1*/0);
      }
      if (fire_sensor_current_state == HIGH) {
        network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_alarm, true, /*1*/0);
      }
    } else {
      if (fire_sensor_current_state == LOW) {
        network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_alarm, true, /*1*/0);
      }
      if (fire_sensor_current_state == HIGH) {
        network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_normal, true, /*1*/0);
      }
    }
    if (_fire_sensor_alarm) {
      network->publishMqtt(mqtt_topic_fire_sensor_saved, mqtt_payload_fire_sensor_alarm, true, /*1*/0);
    } else {
      network->publishMqtt(mqtt_topic_fire_sensor_saved, mqtt_payload_fire_sensor_normal, true, /*1*/0);
    }
  } else {
    network->publishMqtt(mqtt_topic_fire_sensor_current, mqtt_payload_fire_sensor_na, true, /*1*/0);
    network->publishMqtt(mqtt_topic_fire_sensor_saved, mqtt_payload_fire_sensor_na, true, /*1*/0);
  }
}
