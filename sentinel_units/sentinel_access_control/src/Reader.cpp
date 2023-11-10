#include "Reader.h"
#include "Filesystem.h"
#include "Network.h"
#include "Mcp.h"
#include "Lock.h" // !!!
#include "pins.h"
#include "prefs.h"
#include "topics.h"
#include "events.h"
#include "defaults.h"

#include "Logger.h" // ?

Reader::Reader(int id) : _id(id) {
  switch(_id) {
    case 1:
      _connected = false;
      _halt = true;
      _preference_reader_tamper_alarm = preference_reader_1_tamper_alarm;
      _enabled = filesystem->getReader1(); // !
      _keypad = filesystem->getReader1Keypad(); // !
      _silent = filesystem->getReader1Silent();
      _reverse = filesystem->getReader1Reverse();
      _timeout = filesystem->getReader1Timeout();
      _buzzer_timeout = filesystem->getReader1BuzzerTimeout();
      _tamper_sensor = filesystem->getReader1Tamper();
      _tamper_invert = filesystem->getReader1TamperInvert();
      _tamper_restore = filesystem->getReader1TamperRestore();
      _tamper_alarm = filesystem->getReader1TamperAlarm();
      _mqtt_topic_reader_status = mqtt_topic_reader_1_status;
      _mqtt_payload_reader_connected = mqtt_payload_reader_1_connected;
      _mqtt_payload_reader_disconnected = mqtt_payload_reader_1_disconnected;
      _mqtt_topic_reader_tamper_current = mqtt_topic_reader_1_tamper_current;
      _mqtt_topic_reader_tamper_saved = mqtt_topic_reader_1_tamper_saved;
      _mqtt_payload_reader_tamper_normal = mqtt_payload_reader_1_tamper_normal;
      _mqtt_payload_reader_tamper_alarm = mqtt_payload_reader_1_tamper_alarm;
      _mqtt_payload_reader_tamper_na = mqtt_payload_reader_1_tamper_na;
      _event_reader_tamper_current = event_reader_1_tamper_current;
      _event_reader_tamper_saved = event_reader_1_tamper_saved;
      _event_reader_tamper_state_normal = event_reader_1_tamper_state_normal;
      _event_reader_tamper_state_alarm = event_reader_1_tamper_state_alarm;
      _error_timeout = DEFAULT_READER_ERROR_TIMEOUT;
      _error_sequence_counter = 0;
      _panic_timeout = DEFAULT_READER_PANIC_TIMEOUT;
      _d0_pin = ESP_READER_1_D0;
      _d1_pin = ESP_READER_1_D1;
      pinMode(ESP_READER_1_D0, INPUT); // external pull-down resistor
      pinMode(ESP_READER_1_D1, INPUT); // external pull-down resistor
      _buzzer_pin = MCP_READER_1_BUZZER;
      _led_red_pin = MCP_READER_1_RED_LED;
      _led_green_pin = MCP_READER_1_GREEN_LED;
      mcp->inst()->pinMode(MCP_READER_1_GREEN_LED, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_1_GREEN_LED, HIGH); // OFF
      mcp->inst()->pinMode(MCP_READER_1_RED_LED, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_1_RED_LED, LOW); // ON
      mcp->inst()->pinMode(MCP_READER_1_BUZZER, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_1_BUZZER, HIGH); // OFF
      _tamper_pin = MCP_READER_1_TAMPER;
      mcp->inst()->pinMode(MCP_READER_1_TAMPER, INPUT); // external pull-up resistor
      mcp->inst()->setupInterruptPin(MCP_READER_1_TAMPER, CHANGE);
      _last_time = 0;
      _trigger = true;
      _check = true;
      _debounce = DEFAULT_READER_TAMPER_DEBOUNCE;
      _wiegand = new Wiegand();
      _ticker.attach_ms(500, +[](Reader* reader) { // ? - is it needed ?? <- failover if no init called ?
        mcp->inst()->digitalWrite(reader->greenLedPin(), !mcp->inst()->digitalRead(reader->greenLedPin()));
      }, this);
      break;
    case 2:
      _connected = false;
      _halt = true;
      _preference_reader_tamper_alarm = preference_reader_2_tamper_alarm;
      _enabled = filesystem->getReader2(); // !
      _keypad = filesystem->getReader2Keypad(); // !
      _silent = filesystem->getReader2Silent();
      _reverse = filesystem->getReader2Reverse();
      _timeout = filesystem->getReader2Timeout();
      _buzzer_timeout = filesystem->getReader2BuzzerTimeout();
      _tamper_sensor = filesystem->getReader2Tamper();
      _tamper_invert = filesystem->getReader2TamperInvert();
      _tamper_restore = filesystem->getReader2TamperRestore();
      _tamper_alarm = filesystem->getReader2TamperAlarm();
      _mqtt_topic_reader_status = mqtt_topic_reader_2_status;
      _mqtt_payload_reader_connected = mqtt_payload_reader_2_connected;
      _mqtt_payload_reader_disconnected = mqtt_payload_reader_2_disconnected;
      _mqtt_topic_reader_tamper_current = mqtt_topic_reader_2_tamper_current;
      _mqtt_topic_reader_tamper_saved = mqtt_topic_reader_2_tamper_saved;
      _mqtt_payload_reader_tamper_normal = mqtt_payload_reader_2_tamper_normal;
      _mqtt_payload_reader_tamper_alarm = mqtt_payload_reader_2_tamper_alarm;
      _mqtt_payload_reader_tamper_na = mqtt_payload_reader_2_tamper_na;
      _event_reader_tamper_current = event_reader_2_tamper_current;
      _event_reader_tamper_saved = event_reader_2_tamper_saved;
      _event_reader_tamper_state_normal = event_reader_2_tamper_state_normal;
      _event_reader_tamper_state_alarm = event_reader_2_tamper_state_alarm;
      _error_timeout = DEFAULT_READER_ERROR_TIMEOUT;
      _error_sequence_counter = 0;
      _panic_timeout = DEFAULT_READER_PANIC_TIMEOUT;
      _d0_pin = ESP_READER_2_D0;
      _d1_pin = ESP_READER_2_D1;
      pinMode(ESP_READER_2_D0, INPUT); // external pull-down resistor
      pinMode(ESP_READER_2_D1, INPUT); // external pull-down resistor
      _buzzer_pin = MCP_READER_2_BUZZER;
      _led_red_pin = MCP_READER_2_RED_LED;
      _led_green_pin = MCP_READER_2_GREEN_LED;
      mcp->inst()->pinMode(MCP_READER_2_GREEN_LED, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_2_GREEN_LED, HIGH); // OFF
      mcp->inst()->pinMode(MCP_READER_2_RED_LED, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_2_RED_LED, LOW); // ON
      mcp->inst()->pinMode(MCP_READER_2_BUZZER, OUTPUT);
      mcp->inst()->digitalWrite(MCP_READER_2_BUZZER, HIGH); // OFF
      _tamper_pin = MCP_READER_2_TAMPER;
      mcp->inst()->pinMode(MCP_READER_2_TAMPER, INPUT); // external pull-up resistor
      mcp->inst()->setupInterruptPin(MCP_READER_2_TAMPER, CHANGE);
      _last_time = 0;
      _trigger = true;
      _check = true;
      _debounce = DEFAULT_READER_TAMPER_DEBOUNCE;
      _wiegand = new Wiegand();
      _ticker.attach_ms(500, +[](Reader* reader) { // ? - is it needed ?? <- failover if no init called ?
        mcp->inst()->digitalWrite(reader->greenLedPin(), !mcp->inst()->digitalRead(reader->greenLedPin()));
      }, this);
      break;
  }
}

Wiegand* Reader::wiegand() {
  return _wiegand;
}

bool Reader::reversed() {
  return _reverse;
}

void Reader::processError(Wiegand::DataError error, uint8_t* data, uint8_t bits) {
  if (bits > 3/*2*/) { // sometimes gets 1bits / 00 -> filter this out
    panic();
  }
}

String Reader::processSuccess(uint8_t* data, uint8_t bits) {
  buzzerOn();
  uint8_t bytes = (bits+7)/8;
  String out;
  if (_reverse) {
    for (int i=bytes; i>0; i--) {
      out.concat(String(data[i-1] >> 4, 16));
      out.concat(String(data[i-1] & 0xF, 16));
    }
  } else {
    for (int i=0; i<bytes; i++) {
      out.concat(String(data[i] >> 4, 16));
      out.concat(String(data[i] & 0xF, 16));
    }
  }
  out.toUpperCase();
  return out;
}

void Reader::buzzerOn() { // TODO
  if (!_silent) {
    _buzzer_ticker.detach();
    mcp->inst()->digitalWrite(_buzzer_pin, LOW); // ON
    _buzzer_ticker.once_ms(_buzzer_timeout, +[](Reader* reader) { // TODO - this is sometimes hanged
      reader->buzzerOff();
    }, this);
  }
  // Log->print("[DEBUG] Buzzer "); // ?
  // Log->print(_id); // ?
  // Log->println(" ON"); // ?
}

void Reader::buzzerOnOverride(int timeoutOverride) {
  _buzzer_ticker.detach();
  mcp->inst()->digitalWrite(_buzzer_pin, LOW); // ON
  _buzzer_ticker.once_ms(timeoutOverride, +[](Reader* reader) {
    reader->buzzerOff();
  }, this);
}

void Reader::buzzerOff() {
  mcp->inst()->digitalWrite(_buzzer_pin, HIGH); // OFF
  // _buzzer_ticker.detach(); // ? - ???
  // Log->print("[DEBUG] Buzzer "); // ?
  // Log->print(_id); // ?
  // Log->println(" OFF"); // ?
}

void Reader::greenLedOn() {
  mcp->inst()->digitalWrite(_led_green_pin, LOW); // ON
}

void Reader::greenLedOff() {
  mcp->inst()->digitalWrite(_led_green_pin, HIGH); // OFF
}

void Reader::redLedOn() {
  mcp->inst()->digitalWrite(_led_red_pin, LOW); // ON
}

void Reader::redLedOff() {
  mcp->inst()->digitalWrite(_led_red_pin, HIGH); // OFF
}

int Reader::greenLedPin() {
  return _led_green_pin;
}

int Reader::redLedPin() {
  return _led_red_pin;
}

int Reader::buzzerPin() {
  return _buzzer_pin;
}

void Reader::init() { // TODO - check switch for bypass, block, normal -> outside of here
  if (_tamper_alarm) {
    block();
  } else {
    _ticker.detach();
    _buzzer_ticker.detach();
    buzzerOff();
    redLedOff();
    greenLedOff();
    _halt = false;
  }
  _wiegand->begin(Wiegand::LENGTH_ANY, true); // _wiegand->begin(Wiegand::LENGTH_ANY, false); -> disable built in parity checks -> implement yourself
}

void Reader::error() {
  _ticker.detach();
  _halt = true;
  greenLedOff();
  redLedOn();
  _ticker.once_ms(_error_timeout, +[](Reader* reader) {
    reader->greenLedOff();
    reader->redLedOff();
    reader->setHalt(false);
  }, this);
}

void Reader::errorSequence() {
  _ticker.detach();
  _halt = true;
  greenLedOff();
  redLedOn();
  _error_sequence_counter = 5;
  _ticker.once_ms(150, +[](Reader* reader) {
    reader->_errorSequence();
  }, this);
}

void Reader::_errorSequence() {
  mcp->inst()->digitalWrite(_led_green_pin, !mcp->inst()->digitalRead(_led_green_pin));
  if (_error_sequence_counter > 0) {
    _error_sequence_counter = _error_sequence_counter - 1;
    _ticker.once_ms(150, +[](Reader* reader) {
      reader->_errorSequence();
    }, this);
  } else {
    _error_sequence_counter = 0;
    _ticker.once_ms(150, +[](Reader* reader) {
      reader->greenLedOff();
      reader->redLedOff();
      reader->setHalt(false);
    }, this);
  }
}

void Reader::configSequence() {
  // _halt = true; // ?
  // buzzerOff(); // ?
  // greenLedOff(); // ?
  // redLedOff(); // ?
  _buzzer_ticker.detach();
  _ticker.detach();
  _configSequence1();
}

void Reader::configSequenceEnd() {
  // _buzzer_ticker.detach();
  _ticker.detach();
  // buzzerOff();
  greenLedOff();
  redLedOn();
  buzzerOnOverride(100);
}

void Reader::_configSequence1() {
  greenLedOn();
  redLedOn();
  _ticker.once_ms(500, +[](Reader* reader) {
    reader->_configSequence2();
  }, this);
}

void Reader::_configSequence2() {
  greenLedOn();
  redLedOff();
  _ticker.once_ms(500, +[](Reader* reader) {
    reader->_configSequence3();
  }, this);
}

void Reader::_configSequence3() {
  greenLedOff();
  redLedOn();
  _ticker.once_ms(500, +[](Reader* reader) {
    reader->_configSequence4();
  }, this);
}

void Reader::_configSequence4() {
  greenLedOff();
  redLedOff();
  _ticker.once_ms(500, +[](Reader* reader) {
    reader->_configSequence1();
  }, this);
}

void Reader::bypass() { // ? - allowed with tamper ???
  _ticker.detach();
  _buzzer_ticker.detach();
  _halt = true;
  buzzerOff();
  redLedOff();
  greenLedOn();
}

void Reader::block() { // ? - allowed with tamper ???
  _ticker.detach();
  _buzzer_ticker.detach();
  _halt = true;
  buzzerOff();
  greenLedOff();
  redLedOn();
}

void Reader::normal() { // TODO - when returning from bypass, block to normal -> check if tamper alarm before returning to normal
  if (_tamper_alarm) {
    block();
  } else {
    _ticker.detach();
    _buzzer_ticker.detach();
    _halt = false;
    buzzerOff();
    greenLedOff();
    redLedOff();
  }
}

void Reader::panic() {
  _ticker.detach();
  _halt = true;
  redLedOn();
  greenLedOn();
  _ticker.once_ms(_panic_timeout, +[](Reader* reader) {
    reader->redLedOff();
    reader->greenLedOff();
    reader->setHalt(false);
  }, this);
}

void Reader::success(bool callOthers, int timeoutOverride, Lock* lock, Reader* secondReader) { // ?
  if (!_tamper_alarm) { // ?
    _ticker.detach();
    _halt = true;
    int timeout = _timeout;
    redLedOff();
    greenLedOn();
    if (callOthers) {
      secondReader->success(false, _timeout, lock, this);
      lock->timedOpen(_timeout);
    } else {
      timeout = timeoutOverride;
    }
    _ticker.once_ms(timeout, +[](Reader* reader) {
      reader->redLedOff();
      reader->greenLedOff();
      reader->setHalt(false);
    }, this);
  }
}

// void Reader::_tamperReset() { // TODO - call from somewhere else, where Reader* is not available -> same logic as with mqtt_lock_command -> mqtt_sensor_command ??
//   _setReaderTamperAlarm(false);
//   _tamper_alarm = false;
//   _trigger = true;
//   _check = true;
// }

void Reader::_tamperCheck() { // TODO - what is different from normal() ??
  if (_tamper_alarm) {
    block();
  } else { // TODO - here check for bypass, block
    _ticker.detach();
    _buzzer_ticker.detach();
    _halt = false;
    buzzerOff();
    greenLedOff();
    redLedOff();
  }
}

int Reader::getId() {
  return _id;
}

bool Reader::getHalt() {
  return _halt;
}

void Reader::setHalt(bool halt) {
  _halt = halt;
}

bool Reader::isConnected() {
  return _connected;
}

void Reader::setConnectionState(bool connected) {
  _connected = connected;
}

Ticker Reader::getTicker() {
  return _ticker;
}

Ticker Reader::getBuzzerTicker() {
  return _buzzer_ticker;
}

bool Reader::hasTamperSensor() {
  return _tamper_sensor;
}

bool Reader::isTamperSensorInverted() {
  return _tamper_invert;
}

int Reader::tamperPin() {
  return _tamper_pin;
}

void Reader::trigger() {
  if (_tamper_sensor) {
    if (!_trigger) _trigger = true;
  }
}

// TODO - this is logging each state on boot if _check = true -> do we want this -> do we want only some parts ? or only some parts while other condition is met ?
void Reader::_logTamper(bool alarm) {
  if (alarm) {
    // network->publishMqttEventMessage(_event_reader_tamper_state_alarm, _event_reader_tamper_current, false, "", false, "", false, "", event_level_warning, event_type_device);
    network->publishMqttEventMessage(_event_reader_tamper_state_alarm, _event_reader_tamper_current, "", "", "", event_level_warning, event_type_device);
    network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_alarm, true, /*1*/0);
    if (!_tamper_alarm) {
      filesystem->setBoolPreference(_preference_reader_tamper_alarm, alarm);
      _tamper_alarm = alarm;
      // network->publishMqttEventMessage(_event_reader_tamper_state_alarm, _event_reader_tamper_saved, false, "", false, "", false, "", event_level_warning, event_type_device);
      network->publishMqttEventMessage(_event_reader_tamper_state_alarm, _event_reader_tamper_saved, "", "", "", event_level_warning, event_type_device);
      network->publishMqtt(_mqtt_topic_reader_tamper_saved, _mqtt_payload_reader_tamper_alarm, true, /*1*/0);
      _tamperCheck();
    }
  } else {
    // network->publishMqttEventMessage(_event_reader_tamper_state_normal, _event_reader_tamper_current, false, "", false, "", false, "", event_level_warning, event_type_device);
    network->publishMqttEventMessage(_event_reader_tamper_state_normal, _event_reader_tamper_current, "", "", "", event_level_warning, event_type_device);
    network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_normal, true, /*1*/0);
    if (_tamper_alarm) {
      if (_tamper_restore) {
        filesystem->setBoolPreference(_preference_reader_tamper_alarm, alarm);
        _tamper_alarm = alarm;
        // network->publishMqttEventMessage(_event_reader_tamper_state_normal, _event_reader_tamper_saved, false, "", false, "", false, "", event_level_warning, event_type_device);
        network->publishMqttEventMessage(_event_reader_tamper_state_normal, _event_reader_tamper_saved, "", "", "", event_level_warning, event_type_device);
        network->publishMqtt(_mqtt_topic_reader_tamper_saved, _mqtt_payload_reader_tamper_normal, true, /*1*/0);
        _tamperCheck();
      }
    }
  }
}

bool Reader::_readerTamperAlarm() {
  return _tamper_alarm;
  // return filesystem->getBoolPreference(_preference_reader_tamper_alarm); // ! ?
}

void Reader::handle() {
  if (!_connected || (!_halt && _connected)) {
    _wiegand->flush(); // Checks for pending messages
    _wiegand->setPin0State(digitalRead(_d0_pin)); // Check for changes on the the wiegand input pins
    _wiegand->setPin1State(digitalRead(_d1_pin));
  }
  if (_tamper_sensor) {
    if (_trigger) {
      int current_state = mcp->inst()->digitalRead(_tamper_pin);
      if (current_state != _last_state) {
        _last_time = millis();
      }
      if (((millis() - _last_time) >= _debounce) || _check) {
        if ((current_state != _state) || _check) {
          _state = current_state;
          if (_tamper_invert) {
            if (_state == LOW) {
              _logTamper(false);
            }
            if (_state == HIGH) {
              _logTamper(true);
            }
          } else {
            if (_state == LOW) {
              _logTamper(true);
            }
            if (_state == HIGH) {
              _logTamper(false);
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

void Reader::publishMqttInit() {
  if (_connected) {
    network->publishMqtt(_mqtt_topic_reader_status, _mqtt_payload_reader_connected, true, /*1*/0);
  } else {
    network->publishMqtt(_mqtt_topic_reader_status, _mqtt_payload_reader_disconnected, true, /*1*/0);
  }
  if (_tamper_sensor) {
    int reader_tamper_current_state = mcp->inst()->digitalRead(_tamper_pin);
    if (_tamper_invert) {
      if (reader_tamper_current_state == LOW) {
        network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_normal, true, /*1*/0);
      }
      if (reader_tamper_current_state == HIGH) {
        network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_alarm, true, /*1*/0);
      }
    } else {
      if (reader_tamper_current_state == LOW) {
        network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_alarm, true, /*1*/0);
      }
      if (reader_tamper_current_state == HIGH) {
        network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_normal, true, /*1*/0);
      }
    }
    if (_tamper_alarm) {
      network->publishMqtt(_mqtt_topic_reader_tamper_saved, _mqtt_payload_reader_tamper_alarm, true, /*1*/0);
    } else {
      network->publishMqtt(_mqtt_topic_reader_tamper_saved, _mqtt_payload_reader_tamper_normal, true, /*1*/0);
    }
  } else {
    network->publishMqtt(_mqtt_topic_reader_tamper_current, _mqtt_payload_reader_tamper_na, true, /*1*/0);
    network->publishMqtt(_mqtt_topic_reader_tamper_saved, _mqtt_payload_reader_tamper_na, true, /*1*/0);
  }
}
