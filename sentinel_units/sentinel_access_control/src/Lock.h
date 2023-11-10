#pragma once

#include <Ticker.h>

class Lock {
  public:
    Lock();
    void init(); // ! single instance
    void timedOpen(int timeout);
    void open();
    void close();
    bool hasDoorSensor();
    int doorSensorPin();
    void handleDoorSensor();
    void trigger();
    void bypass();
    void block();
    void normal();
    void publishMqttInit();
    Ticker getTicker();
  private:
    void _logDoorSensor(bool closed);
    void _logDoorAlarm(bool alarm); // TODO
    void _alarmTrigger(); // TODO
    void _alarmCancel(); // TODO
    Ticker _alarm_ticker; // TODO
    bool _door_alarm; // TODO
    bool _door_alarm_state; // TODO
    int _door_alarm_timeout; // TODO
    bool _door_alarm_restore; // TODO
    Ticker _ticker;
    bool _door_sensor; // ! ?
    bool _door_sensor_state; // ! ?
    int _door_sensor_pin;
    bool _trigger;
    int _state;
    int _last_state;
    unsigned long _last_time;
    bool _check;
    int _debounce;
};

// extern Lock* lock; // ! single instance
