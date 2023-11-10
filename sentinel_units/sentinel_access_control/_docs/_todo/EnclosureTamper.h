#pragma once

class EnclosureTamper {
  public:
    EnclosureTamper();
    void trigger();
    void handle();
    void publishMqttInit();
  private:
    void _logEnclosureTamper(bool alarm);
    bool _enclosure_tamper;
    bool _enclosure_tamper_invert;
    bool _enclosure_tamper_restore;
    bool _enclosure_tamper_alarm;
    bool _trigger;
    int _enclosure_tamper_pin;
    int _state;
    int _last_state;
    unsigned long _last_time;
    bool _check;
    int _debounce;
};
