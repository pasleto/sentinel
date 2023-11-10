#include "Filesystem.h"
#include "Network.h"
#include "EnclosureTamper.h"
#include "Mcp.h"
#include "pins.h"
#include "prefs.h"
#include "topics.h"
#include "events.h"
#include "defaults.h"

EnclosureTamper::EnclosureTamper() {
  _enclosure_tamper = filesystem->getEnclosureTamper();
  _enclosure_tamper_invert = filesystem->getEnclosureTamperInvert();
  _enclosure_tamper_restore = filesystem->getEnclosureTamperRestore();
  _enclosure_tamper_alarm = filesystem->getEnclosureTamperAlarm();
  _enclosure_tamper_pin = MCP_ENCLOSURE_TAMPER_SENSOR;
  mcp->inst()->pinMode(MCP_ENCLOSURE_TAMPER_SENSOR, INPUT); // external pull-up resistor
  mcp->inst()->setupInterruptPin(MCP_ENCLOSURE_TAMPER_SENSOR, CHANGE);
  _last_time = 0;
  _trigger = true;
  _check = true;
  _debounce = DEFAULT_ENCLOSURE_TAMPER_DEBOUNCE;
}

void EnclosureTamper::trigger() {
  if (_enclosure_tamper) {
    if (!_trigger) _trigger = true;
  }
}

void EnclosureTamper::_logEnclosureTamper(bool alarm) { // ?
  if (alarm != _enclosure_tamper_alarm) { // !
    if (alarm) {
      network->publishMqttEventMessage(event_enclosure_tamper_state_alarm, event_enclosure_tamper_current, false, "", false, "");
      network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_alarm, true, /*1*/0);
      if (!_enclosure_tamper_alarm) { // ? - not needed ??
        filesystem->setEnclosureTamperAlarm(alarm);
        _enclosure_tamper_alarm = alarm;
        network->publishMqttEventMessage(event_enclosure_tamper_state_alarm, event_enclosure_tamper_saved, false, "", false, "");
        network->publishMqtt(mqtt_topic_enclosure_tamper_saved, mqtt_payload_enclosure_tamper_alarm, true, /*1*/0);
      }
    } else {
      network->publishMqttEventMessage(event_enclosure_tamper_state_normal, event_enclosure_tamper_current, false, "", false, "");
      network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_normal, true, /*1*/0);
      if (_enclosure_tamper_restore) {
        if (_enclosure_tamper_alarm) { // ? - not needed ??
          filesystem->setEnclosureTamperAlarm(alarm);
          _enclosure_tamper_alarm = alarm;
          network->publishMqttEventMessage(event_enclosure_tamper_state_normal, event_enclosure_tamper_saved, false, "", false, "");
          network->publishMqtt(mqtt_topic_enclosure_tamper_saved, mqtt_payload_enclosure_tamper_normal, true, /*1*/0);
        }
      }
    }
  }
}

void EnclosureTamper::handle() {
  if (_enclosure_tamper) {
    if (_trigger) {
      int current_state = mcp->inst()->digitalRead(_enclosure_tamper_pin);
      if (current_state != _last_state) {
        _last_time = millis();
      }
      if (((millis() - _last_time) >= _debounce) || _check) {
        if ((current_state != _state) || _check) {
          _state = current_state;
          if (_enclosure_tamper_invert) {
            if (_state == LOW) {
              _logEnclosureTamper(false);
            }
            if (_state == HIGH) {
              _logEnclosureTamper(true);
            }
          } else {
            if (_state == LOW) {
              _logEnclosureTamper(true);
            }
            if (_state == HIGH) {
              _logEnclosureTamper(false);
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

void EnclosureTamper::publishMqttInit() {
  if (_enclosure_tamper) {
    int enclosure_tamper_current_state = mcp->inst()->digitalRead(_enclosure_tamper_pin);
    if (_enclosure_tamper_invert) {
      if (enclosure_tamper_current_state == LOW) {
        network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_normal, true, /*1*/0);
      }
      if (enclosure_tamper_current_state == HIGH) {
        network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_alarm, true, /*1*/0);
      }
    } else {
      if (enclosure_tamper_current_state == LOW) {
        network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_alarm, true, /*1*/0);
      }
      if (enclosure_tamper_current_state == HIGH) {
        network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_normal, true, /*1*/0);
      }
    }
    if (_enclosure_tamper_alarm) {
      network->publishMqtt(mqtt_topic_enclosure_tamper_saved, mqtt_payload_enclosure_tamper_alarm, true, /*1*/0);
    } else {
      network->publishMqtt(mqtt_topic_enclosure_tamper_saved, mqtt_payload_enclosure_tamper_normal, true, /*1*/0);
    }
  } else {
    network->publishMqtt(mqtt_topic_enclosure_tamper_current, mqtt_payload_enclosure_tamper_na, true, /*1*/0);
    network->publishMqtt(mqtt_topic_enclosure_tamper_saved, mqtt_payload_enclosure_tamper_na, true, /*1*/0);
  }
}
