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
    void printHardwareRestart();
    void printSwitchToConfigMode();
    void printCaptivePortal(String ipAddress);
    String getMac();
    String getMacNag();
    String defaultHostname();
    String getTopicBase();
    double round2(float value);
    float nonNegative(float value);
    double spiffsPercentageUsage();
    double spiffsUsed();
    double spiffsFree();
    double spiffsTotal();
    String filesystemUsageJson();
    void playConfigSaveBeep();
    void playConfigEntryBeep();
    void playHwRebootBeep();
    void playOnCardBeep();
    void hwReboot();
    void swReboot();
  private:
    String _macToString(byte ar[]);
};

extern Utils* utils;
