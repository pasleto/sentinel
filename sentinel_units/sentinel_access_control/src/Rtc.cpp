#include "Rtc.h"
#include "Utils.h"
#include "Logger.h"

Rtc* rtc = new Rtc();

Rtc::Rtc() {
  _rtc = new RTC_DS3231();
}

void Rtc::init() {
  if (!_rtc->begin()) {
    Log->println(F("[RTC] Error"));
    while (1);
  }
  if (_rtc->lostPower()) {
    Log->println(F("[RTC] Lost power, setting default date (1/1/2000 00:00:00)"));
    _rtc->adjust(DateTime(2000, 1, 1, 0, 0, 0));
    utils->setRtcBatteryWarning(true);
  }
  // log->print(F("[RTC] Temperature is: "));
  // log->print(_rtc.getTemperature());
  // log->println(F(" °C"));
}

uint32_t Rtc::getUnixtime() {
  return _rtc->now().unixtime();
}

String Rtc::getTimestamp() {
  return _rtc->now().timestamp();
}

void Rtc::updateFromNTP() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    _rtc->adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    Log->print(F("[RTC] Updated from NTP - "));
    Log->println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}
