#include "ExternalAlarm.h"
#include "Filesystem.h"
#include "Network.h"
#include "Mcp.h"
#include "pins.h"
#include "topics.h"
#include "events.h"

#include "Logger.h" // !

ExternalAlarm* externalAlarm = new ExternalAlarm();

// TODO - when one of the actuators request cancel -> check somehow if others are not still active

ExternalAlarm::ExternalAlarm() {
  _alarm = filesystem->getExternalAlarm();
  _state = filesystem->getExternalAlarmState();
  // _pin = MCP_HW_ALARM_TRIGGER; // TODO
  // mcp->inst()->pinMode(MCP_HW_ALARM_TRIGGER, OUTPUT); // TODO

  // TODO - on boot check
}

int ExternalAlarm::pin() {
  return _pin;
}

bool ExternalAlarm::enabled() {
  return _alarm;
}

void ExternalAlarm::_on() { // TODO
  // mcp->inst()->digitalWrite(MCP_ALARM_TRIGGER, LOW); // ON // TODO
  Log->println("[DEBUG] Alarm - ON"); // TODO
}

void ExternalAlarm::_off() { // TODO
  // mcp->inst()->digitalWrite(MCP_ALARM_TRIGGER, HIGH); // OFF // TODO
  Log->println("[DEBUG] Alarm - ON"); // TODO
}

void ExternalAlarm::_setState(bool alarm) {
  if (alarm != _state) {
    filesystem->setExternalAlarmState(alarm);
    _state = true;
    if (alarm) {
      _on(); // TODO
      network->publishMqttEventMessage(event_external_alarm_state_on, event_external_alarm, false, "", false, "");
      network->publishMqtt(mqtt_topic_external_alarm, mqtt_payload_external_alarm_on, true, /*1*/0);
    } else {
      _off(); // TODO
      network->publishMqttEventMessage(event_external_alarm_state_off, event_external_alarm, false, "", false, "");
      network->publishMqtt(mqtt_topic_external_alarm, mqtt_payload_external_alarm_off, true, /*1*/0);
    }
  }
}

void ExternalAlarm::trigger() {
  if (_alarm) {
    if (!_state) { // check if state change (if alarm is not already on)
      _setState(true);
    }
  }
}

void ExternalAlarm::cancel() { // TODO
  if (_alarm) {
    if (_state) { // check if state change (if alarm is not already off)
      _setState(false);
    }
  }
}

void ExternalAlarm::publishMqttInit() {
  if (_alarm) {
    if (_state) {
      network->publishMqtt(mqtt_topic_external_alarm, mqtt_payload_external_alarm_on, true, /*1*/0);
    } else {
      network->publishMqtt(mqtt_topic_external_alarm, mqtt_payload_external_alarm_off, true, /*1*/0);
    }
  } else {
    network->publishMqtt(mqtt_topic_external_alarm, mqtt_payload_external_alarm_na, true, /*1*/0);
  }
}

void ExternalAlarm::check() { // TODO
  Log->println("[DEBUG] Alarm - Boot Check"); // TODO
}
