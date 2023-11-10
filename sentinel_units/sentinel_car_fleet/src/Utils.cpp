#include "Utils.h"
#include "Logger.h"
#include "config.h"
#include "pins.h"
#include <Wire.h>

Utils* utils = new Utils();

Utils::Utils() {
  Wire.setPins(ESP_SDA, ESP_SCL);

  pinMode(ESP_CONFIG_BUTTON, INPUT_PULLUP); // external pull-down || external pull-up

  digitalWrite(ESP_RST, HIGH); // off
  pinMode(ESP_RST, OUTPUT);
  digitalWrite(ESP_PWR_LED, LOW); // on // TODO
  pinMode(ESP_PWR_LED, OUTPUT); // TODO

  // digitalWrite(ESP_BEEPER, HIGH); // !
  // pinMode(ESP_BEEPER, OUTPUT); // !

  tone(ESP_BEEPER, 0, 0);

  // TODO
  pinMode(ESP_CAR_PWR_INT, INPUT); // TODO - external pull resistor
  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_rising, RISING);
  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_falling, FALLING);
  // attachInterrupt(ESP_CAR_PWR_INT, handle_car_pwr_interrupt_change, CHANGE);
}

void Utils::printInit() {
  Log->println();
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  // Log->print(F("[Device Unit Type] "));
  // Log->print(MODULE_NAME);
  // Log->print(F(" ("));
  // Log->print(MODULE_TYPE);
  // Log->println(F(")"));
  // Log->print(F("[Hardware Version] "));
  // Log->printl(MODULE_HARDWARE_VERSION);
  Log->print(F("[Device Unit Name] "));
  Log->println(MODULE_NAME);
  Log->print(F("[Device Unit Type] "));
  Log->println(MODULE_TYPE);
  Log->print(F("[Firmware Version] "));
  Log->println(MODULE_FIRMWARE_VERSION);
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

void Utils::printHardwareRestart() {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->println(F("[BOOT] Performing hardware reboot"));
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printSwitchToConfigMode() {
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->println(F("[CONFIG] Switching to config mode"));
  Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

void Utils::printCaptivePortal(String ipAddress) {
  // Log->println(F("----------------------------------------------------------------------------------------------------------------"));
  Log->print(F("[AP] Captive portal on IP: "));
  Log->println(ipAddress);
  // Log->println(F("----------------------------------------------------------------------------------------------------------------"));
}

String Utils::_macToString(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
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
  String mac_address_nag = mac_address;
  return mac_address_nag;
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

float Utils::nonNegative(float value) {
  // if (value >= 0.0) {
  //   return value;
  // } else {
  //   return 0.00;
  // }
  if (value < 0.0) {
    return 0.00;
  } else {
    return value;
  }
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

String Utils::filesystemUsageJson() {
  const size_t capacity = JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> json_doc;
  json_doc["percentage"] = spiffsPercentageUsage();
  json_doc["used"] = spiffsUsed();
  json_doc["free"] = spiffsFree();
  json_doc["total"] = spiffsTotal();
  String data_out;
  serializeJson(json_doc, data_out);
  return data_out;
}

void Utils::playConfigSaveBeep() {
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
}

void Utils::playConfigEntryBeep() {
  tone(ESP_BEEPER, 5000, 500);
}

void Utils::playHwRebootBeep() {
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
  tone(ESP_BEEPER, 7000, 250);
  delay(500);
}

void Utils::playOnCardBeep() {
  tone(ESP_BEEPER, 3000, 1000);
}

void Utils::hwReboot() {
  digitalWrite(ESP_RST, LOW);
}

void Utils::swReboot() {
  ESP.restart();
}
