#include "Rtc.h"
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

float Rtc::getTemperature() {
  return _rtc->getTemperature();
}

void Rtc::adjust(DateTime datetime) {
  _rtc->adjust(datetime);
}
