#pragma once

#include <Arduino.h>
#include <Ticker.h>
#include <Wiegand.h>
#include "Lock.h" // !!!

class Reader {
  public:
    Reader(int id);
    void init();
    void buzzerOn();
    void buzzerOnOverride(int timeoutOverride);
    void buzzerOff();
    void greenLedOn();
    void greenLedOff();
    void redLedOn();
    void redLedOff();
    Ticker getTicker();
    Ticker getBuzzerTicker();
    int getId();
    bool getHalt();
    void setHalt(bool halt);
    bool isConnected();
    void setConnectionState(bool connected);
    void error();
    void errorSequence();
    void configSequence();
    void configSequenceEnd();
    void bypass();
    void block();
    void normal();
    void panic();
    void success(bool callOthers, int timeoutOverride, Lock* lock, Reader* secondReader);
    bool hasTamperSensor();
    bool isTamperSensorInverted();
    int tamperPin();
    void handle();
    void trigger();
    int greenLedPin();
    int redLedPin();
    int buzzerPin();
    void publishMqttInit();
    Wiegand* wiegand();
    bool reversed();
    void processError(Wiegand::DataError error, uint8_t* data, uint8_t bits);
    String processSuccess(uint8_t* data, uint8_t bits);
  private:
    void _errorSequence();
    void _configSequence1();
    void _configSequence2();
    void _configSequence3();
    void _configSequence4();
    bool _readerTamperAlarm();
    void _tamperCheck();
    void _logTamper(bool alarm);
    Wiegand* _wiegand = nullptr;
    Ticker _ticker;
    Ticker _buzzer_ticker;
    bool _enabled; // !
    bool _keypad; // !
    int _id;
    int _error_timeout;
    int _error_sequence_counter;
    int _panic_timeout;
    bool _silent;
    bool _reverse;
    int _timeout;
    int _buzzer_timeout;
    bool _tamper_sensor;
    bool _tamper_invert;
    bool _tamper_restore;
    bool _tamper_alarm;
    bool _halt;
    bool _connected;
    int _d0_pin;
    int _d1_pin;
    int _buzzer_pin;
    int _led_red_pin;
    int _led_green_pin;
    int _tamper_pin;
    bool _trigger;
    int _state;
    int _last_state;
    unsigned long _last_time;
    bool _check;
    int _debounce;
    const char* _preference_reader_tamper_alarm;
    const char* _mqtt_topic_reader_status;
    const char* _mqtt_payload_reader_connected;
    const char* _mqtt_payload_reader_disconnected;
    const char* _mqtt_topic_reader_tamper_current;
    const char* _mqtt_topic_reader_tamper_saved;
    const char* _mqtt_payload_reader_tamper_normal;
    const char* _mqtt_payload_reader_tamper_alarm;
    const char* _mqtt_payload_reader_tamper_na;
    String _event_reader_tamper_current;
    String _event_reader_tamper_saved;
    String _event_reader_tamper_state_normal;
    String _event_reader_tamper_state_alarm;

    // TODO - has_keypad
    // TODO - keypad_mode (virtual card || char by char)
    // TODO - require key_code -> add keycode to cards database
};
