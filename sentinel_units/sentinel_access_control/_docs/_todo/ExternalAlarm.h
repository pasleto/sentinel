#pragma once

class ExternalAlarm {
  public:
    ExternalAlarm();
    int pin();
    bool enabled();
    void trigger();
    void cancel();
    void check(); // TODO - call after boot for state check
    void publishMqttInit();
  private:
    void _setState(bool alarm);
    void _on();
    void _off();
    int _pin;
    bool _alarm;
    bool _state;
};

extern ExternalAlarm* externalAlarm;