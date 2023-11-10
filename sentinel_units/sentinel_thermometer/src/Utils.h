#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class Utils {
  public:
    Utils();
    void printInit();
    void printBoot(bool configMode);
    void printRestart();
    void printSwitchToConfigMode();
    void printCaptivePortal(String ipAddress);
    String getMac();
    String getMacNag();
    String defaultHostname();
    String getTopicBase();
    double round2(float value);
    double spiffsPercentageUsage();
    double spiffsUsed();
    double spiffsFree();
    double spiffsTotal();
    String filesystemUsageJson();
    void swReboot();
    bool rtcBatteryWarning();
    void setRtcBatteryWarning(bool value);
  private:
    String _macToString(byte ar[]);
    bool _rtcBatteryWarning;
};

extern Utils* utils;
