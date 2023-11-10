#pragma once

class FireSensor {
  public:
    FireSensor();
    void trigger();
    void handle();
    void publishMqttInit();
  private:
    void _logFireSensor(bool alarm);
    bool _fire_sensor;
    bool _fire_sensor_invert;
    bool _fire_sensor_restore;
    bool _fire_sensor_alarm;
    bool _trigger;
    int _fire_sensor_pin;
    int _state;
    int _last_state;
    unsigned long _last_time;
    bool _check;
    int _debounce;
};
