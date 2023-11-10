#include "Utils.h"
#include "Logger.h"
#include "StatusLed.h"
#include "config.h"
#include "pins.h"
#include "version.h"
#include <Wire.h>

Utils* utils = new Utils();

Utils::Utils() {
  Wire.setPins(ESP_SDA, ESP_SCL);
  pinMode(ESP_CONFIG_BUTTON, INPUT); // external pull-up
  _rtcBatteryWarning = false;
  _rebootNeededTrigger = false;
}

bool Utils::rtcBatteryWarning() {
  return _rtcBatteryWarning;
}

void Utils::setRtcBatteryWarning(bool value) {
  _rtcBatteryWarning = value;
}

bool Utils::rebootNededTrigger() {
  return _rebootNeededTrigger;
}

void Utils::setRebootNededTrigger(bool value) {
  _rebootNeededTrigger = value;
}

void Utils::printInit() {
  Log->println();
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->print(F("[Device Unit Name] "));
  Log->println(MODULE_NAME);
  Log->print(F("[Device Unit Type] "));
  Log->println(MODULE_TYPE);
  Log->print(F("[Firmware Version] "));
  Log->println(MODULE_FIRMWARE_VERSION);
  Log->print(F("[Hardware Version] "));
  Log->println(MODULE_HW_BOARD_NAME/*MODULE_HARDWARE_VERSION*/);
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printBoot(bool configMode) {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  if (configMode) {
    Log->println(F("[BOOT] Completed (Config Mode)"));
  } else {
    Log->println(F("[BOOT] Completed"));
  }
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printRestart() {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->println(F("[BOOT] Restarting"));
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printSwitchToConfigMode() {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->println(F("[CONFIG] Switching to config mode"));
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printSwitchFromConfigMode() {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->println(F("[CONFIG] Switching out from config mode"));
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printCaptivePortal(String ipAddress) {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->print(F("[AP] Captive portal on IP: "));
  Log->println(ipAddress);
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

String Utils::_macToString(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String Utils::getMac() {
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = _macToString((byte*) &esp_id);
  return mac_address;
}

String Utils::getMacNag() {
  String mac_address = getMac();
  mac_address.replace(":", "");
  // String mac_address_nag = mac_address;
  // return mac_address_nag;
  return mac_address;
}

String Utils::defaultHostname() {
  String mac_address_nag = getMacNag();
  String hostname = MODULE_TYPE + String('-') + mac_address_nag;
  return hostname;
}

String Utils::getTopicBase() {
  String mac_address_nag = getMacNag();
  String topic_base = MQTT_BASE_TOPIC + mac_address_nag;
  return topic_base;
}

double Utils::round2(float value) {
  return (int)(value * 100 + 0.5) / 100.0;
}

double Utils::spiffsPercentageUsage() {
  float calc = ((float)LittleFS.usedBytes() / (float)LittleFS.totalBytes()) * 100;
  return round2(calc);
}

double Utils::spiffsUsed() {
  return LittleFS.usedBytes();
}

double Utils::spiffsFree() {
  return LittleFS.totalBytes() - LittleFS.usedBytes();
}

double Utils::spiffsTotal() {
  return LittleFS.totalBytes();
}

DynamicJsonDocument Utils::filesystemUsageJson() {
  // const size_t capacity = JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(1);
  // StaticJsonDocument<capacity> json_doc;
  StaticJsonDocument<64> json_doc;
  json_doc["percentage"] = spiffsPercentageUsage();
  json_doc["used"] = spiffsUsed();
  json_doc["free"] = spiffsFree();
  json_doc["total"] = spiffsTotal();
  return json_doc;
}

String Utils::filesystemUsageJsonString() {
  String data_out;
  serializeJson(filesystemUsageJson(), data_out);
  return data_out;
}

void Utils::swReboot() {
  statusLed->off();
  ESP.restart();
}
