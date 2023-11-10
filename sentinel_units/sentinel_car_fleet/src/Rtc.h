#pragma once

#include <RTClib.h>

class Rtc {
  public:
    Rtc();
    void init();
    uint32_t getUnixtime();
    String getTimestamp();
    float getTemperature();
    void adjust(DateTime datetime);
  private:
    RTC_DS3231* _rtc = nullptr;
};

extern Rtc* rtc;
