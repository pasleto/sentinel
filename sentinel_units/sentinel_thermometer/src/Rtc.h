#pragma once

#include <Preferences.h>
#include <RTClib.h>

class Rtc {
  public:
    Rtc();
    void init();
    uint32_t getUnixtime();
    String getTimestamp();
    void updateFromNTP();
  private:
    RTC_DS3231* _rtc = nullptr;
};

extern Rtc* rtc;
