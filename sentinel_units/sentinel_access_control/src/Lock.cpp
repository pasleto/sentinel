#include "Lock.h"
#include "Filesystem.h"
#include "Network.h"
#include "Mcp.h"
#include "pins.h"
// #include "prefs.h"
#include "topics.h"
#include "events.h"
#include "defaults.h"

// #include "Alarm.h"

// TODO - when door closing and it is within timer -> cancel -> if timer runs out -> trigger alarm
// TODO - bypass switch for door alarm

// Lock* lock = new Lock(); // ! single instance

Lock::Lock() {
  mcp->inst()->pinMode(MCP_LOCK_TRIGGER, OUTPUT);
  mcp->inst()->digitalWrite(MCP_LOCK_TRIGGER, HIGH); // OFF
  _door_sensor = filesystem->getDoorSensor();
  _door_sensor_state = filesystem->getDoorSensorState();
  _door_sensor_pin = MCP_DOOR_SENSOR;
  _door_alarm = filesystem->getDoorAlarm(); // TODO
  _door_alarm_state = filesystem->getDoorAlarmState(); // TODO
  _door_alarm_timeout = filesystem->getDoorAlarmTimeout(); // TODO
  _door_alarm_restore = filesystem->getDoorAlarmRestore(); // TODO
  mcp->inst()->pinMode(MCP_DOOR_SENSOR, INPUT); // external pull-up resistor
  mcp->inst()->setupInterruptPin(MCP_DOOR_SENSOR, CHANGE);
  _last_time = 0;
  _trigger = true;
  _check = true;
  _debounce = DEFAULT_DOOR_SENSOR_DEBOUNCE;
}

void Lock::init() { // ! single instance

}

void Lock::timedOpen(int timeout) {
  _ticker.detach();
  open();
  _ticker.once_ms(timeout, +[](Lock* lock) {
    lock->close();
  }, this);
}

void Lock::open() {
  mcp->inst()->digitalWrite(MCP_LOCK_TRIGGER, LOW); // ON
}

void Lock::close() {
  mcp->inst()->digitalWrite(MCP_LOCK_TRIGGER, HIGH); // OFF
}

bool Lock::hasDoorSensor() {
  return _door_sensor;
}

int Lock::doorSensorPin() {
  return _door_sensor_pin;
}

Ticker Lock::getTicker() {
  return _ticker;
}

void Lock::trigger() {
  if (_door_sensor) {
    if (!_trigger) _trigger = true;
  }
}

void Lock::bypass() {
  _ticker.detach();
  open();
}

void Lock::block() {
  _ticker.detach();
  close();
}

void Lock::normal() {
  _ticker.detach();
  close();
}

void Lock::_logDoorSensor(bool closed) {
  if (closed != _door_sensor_state) {
    if (closed) {
      // network->publishMqttEventMessage(event_door_sensor_state_closed, event_door_sensor, false, "", false, "", false, "", event_level_info, event_type_access);
      network->publishMqttEventMessage(event_door_sensor_state_closed, event_door_sensor, "", "", "", event_level_info, event_type_access);
      network->publishMqtt(mqtt_topic_door_sensor, mqtt_payload_door_sensor_closed, true, /*1*/0);
      filesystem->setDoorSensorState(closed);
      _door_sensor_state = closed;
    } else {
      // network->publishMqttEventMessage(event_door_sensor_state_open, event_door_sensor, false, "", false, "", false, "", event_level_info, event_type_access);
      network->publishMqttEventMessage(event_door_sensor_state_open, event_door_sensor, "", "", "", event_level_info, event_type_access);
      network->publishMqtt(mqtt_topic_door_sensor, mqtt_payload_door_sensor_open, true, /*1*/0);
      filesystem->setDoorSensorState(closed);
      _door_sensor_state = closed;
    }
  }
}

// TODO - this is logging each state on boot if _check = true -> do we want this -> do we want only some parts ? or only some parts while other condition is met ?
void Lock::_logDoorAlarm(bool alarm) { // TODO
  if (alarm) {
    // network->publishMqttEventMessage(event_door_alarm_state_on, event_door_alarm_current, false, "", false, "", false, "", event_level_warning, event_type_device);
    network->publishMqttEventMessage(event_door_alarm_state_on, event_door_alarm_current, "", "", "", event_level_warning, event_type_device);
    network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_on, true, /*1*/0);
    // ? publish current
    if (!_door_alarm_state) {
      // if (!_check) {

      // }
      filesystem->setDoorAlarmState(alarm);
      _door_alarm_state = alarm;
      // network->publishMqttEventMessage(event_door_alarm_state_on, event_door_alarm_saved, false, "", false, "", false, "", event_level_warning, event_type_device);
      network->publishMqttEventMessage(event_door_alarm_state_on, event_door_alarm_saved, "", "", "", event_level_warning, event_type_device);
      network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_on, true, /*1*/0);
    }
    // else {
    //   if (_check) {

    //   }
    // }
  } else {
    // network->publishMqttEventMessage(event_door_alarm_state_off, event_door_alarm_current, false, "", false, "", false, "", event_level_warning, event_type_device);
    network->publishMqttEventMessage(event_door_alarm_state_off, event_door_alarm_current, "", "", "", event_level_warning, event_type_device);
    network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_off, true, /*1*/0);
    // ? publish current
    if (_door_alarm_state) {
      if (_door_alarm_restore) {
        filesystem->setDoorAlarmState(alarm);
        _door_alarm_state = alarm;
        // network->publishMqttEventMessage(event_door_alarm_state_off, event_door_alarm_saved, false, "", false, "", false, "", event_level_warning, event_type_device);
        network->publishMqttEventMessage(event_door_alarm_state_off, event_door_alarm_saved, "", "", "", event_level_warning, event_type_device);
        network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_off, true, /*1*/0);
      }
    }
  }
}

void Lock::_alarmTrigger() { // TODO - add bool, if this should trigger HW alarm
  if (_door_alarm) {
    _alarm_ticker.detach();
    _alarm_ticker.once_ms(_door_alarm_timeout, +[](Lock* lock) { // start timer
      lock->_logDoorAlarm(true);
    }, this);
  }
}

void Lock::_alarmCancel() { // TODO - add bool, if this should cancel HW alarm
  if (_door_alarm) {
    if (_alarm_ticker.active()) { // still running -> door closed withing timer
      _alarm_ticker.detach();
    } else { // not running -> door closed after timer
      _alarm_ticker.detach();
      _logDoorAlarm(false);
    }
  }
}

void Lock::handleDoorSensor() {
  if (_door_sensor) {
    if (_trigger) {
      int current_state = mcp->inst()->digitalRead(_door_sensor_pin);
      if (current_state != _last_state) {
        _last_time = millis();
      }
      if (((millis() - _last_time) >= _debounce) || _check) {
        if ((current_state != _state) || _check) {
          _state = current_state;
          if (_state == LOW) { // closed
            _logDoorSensor(true);
            _alarmCancel();
          }
          if (_state == HIGH) { // open
            _logDoorSensor(false);
            _alarmTrigger();
          }
        }
        _trigger = false;
        _check = false;
      }
      _last_state = current_state;
    }
  }
}

void Lock::publishMqttInit() {
  if (_door_sensor) {
    if (_door_sensor_state) {
      network->publishMqtt(mqtt_topic_door_sensor, mqtt_payload_door_sensor_closed, true, /*1*/0);
    } else {
      network->publishMqtt(mqtt_topic_door_sensor, mqtt_payload_door_sensor_open, true, /*1*/0);
    }
    if (_door_alarm) {
      // if (_door_alarm_curr_state) { // ? how to get state of current ?
      //   network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_on, true, /*1*/0);
      // } else {
      //   network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_off, true, /*1*/0);
      // }
      if (_door_alarm_state) {
        network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_on, true, /*1*/0);
      } else {
        network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_off, true, /*1*/0);
      }
    } else {
      network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_na, true, /*1*/0);
      network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_na, true, /*1*/0);
    }
  } else {
    network->publishMqtt(mqtt_topic_door_sensor, mqtt_payload_door_sensor_na, true, /*1*/0);
    network->publishMqtt(mqtt_topic_door_alarm_current, mqtt_payload_door_alarm_na, true, /*1*/0);
    network->publishMqtt(mqtt_topic_door_alarm_saved, mqtt_payload_door_alarm_na, true, /*1*/0);
  }
}
