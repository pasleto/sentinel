#pragma once

#include <Preferences.h>
#include <sqlite3.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

struct Card { // ! change check -
  bool success = false; // bool success;
  uint32_t current_time;
  String id;
  String value;
  int can_enter;
  int can_exit;
  int has_validity;
  uint32_t valid_since;
  uint32_t valid_until;
};

struct Event { // ! change check -
  int64_t rowid = -1; // int64_t rowid;
  uint32_t timestamp;
  String state;
  String initiator;
  String value;
  String id;
  String status;
  String level;      // info | warning | error
  String type;     // device | access | ...
};

class Filesystem {
  public:
    Filesystem();
    void init();
    bool rsaKeyPairExist();
    bool generateRsaKeypair(); // ? - return value
    const char* handleConfigJson(DynamicJsonDocument json_doc);
    // const char* addEvent(String state, String initiator, bool has_value, String value, bool has_id, String id, bool has_status, String status, String level, String type); // ? - return value
    const char* addEvent(String state, String initiator, String value, String id, String status, String level, String type); // ? - return value
    const char* cardAction(String job_id, String action, String sql); // ? - return value
    const char* eventAction(String job_id, String action, String sql); // ? - return value
    int cardExistByValue(String cardValue);
    int cardExistById(String cardId);
    int getAllCardsCount();
    int getLogsCount();
    Card getCardByValue(String cardValue); // ?
    Card getCardById(String cardId); // ?
    Card getPagedCard(int count, int page, int card);
    Event getPagedEvent(int count, int page, int card);
    void logBoot(bool configMode);
    void logReboot();
    void logUpdate(String currentVersion, String newVersion);
    void logConfigChange(String status);
    void switchToConfigMode();
    String getConfigJsonString();
    DynamicJsonDocument getConfigJson();
      bool isKeyPreference(const char * key);
      bool getBoolPreference(const char * key);
      bool getBoolPreferenceWithDefault(const char * key, bool defVal);
      String getStringPreference(const char * key);
      String getStringPreferenceWithDefault(const char * key, String defVal);
      int getIntPreference(const char * key);
      int getIntPreferenceWithDefault(const char * key, int defVal);
      void setBoolPreference(const char * key, bool value);
      void setStringPreference(const char * key, String value);
      void setIntPreference(const char * key, int value);
      void setBoolPreferenceOnlyIfNotExist(const char * key, bool value);
      void setStringPreferenceOnlyIfNotExist(const char * key, String value);
      void setIntPreferenceOnlyIfNotExist(const char * key, int value);
    // ? - add default values to return ? (or not needed, when default value set if key not exist)
    bool getConfigMode();
    void setConfigMode(bool value);
    bool getKeepWebserverOutsideConfigMode();
    void setKeepWebserverOutsideConfigMode(bool value);
    bool getWasConfigured();
    void setWasConfigured(bool value);
    bool getMqttSslUse();
    void setMqttSslUse(bool value);
    bool getMqttCredentialsUse();
    void setMqttCredentialsUse(bool value);
    bool getMqttValidateClientCert();
    void setMqttValidateClientCert(bool value);
    bool getIpStatic();
    void setIpStatic(bool value);
    bool getDoorSensor();
    void setDoorSensor(bool value);
    bool getDoorSensorState();
    void setDoorSensorState(bool value);
    bool getDoorAlarm(); // TODO
    void setDoorAlarm(bool value); // TODO
    bool getDoorAlarmState(); // TODO
    void setDoorAlarmState(bool value); // TODO
    int getDoorAlarmTimeout(); // TODO
    void setDoorAlarmTimeout(int value); // TODO
    bool getDoorAlarmRestore(); // TODO
    void setDoorAlarmRestore(bool value); // TODO
    bool getExternalAlarm(); // TODO
    void setExternalAlarm(bool value); // TODO
    bool getExternalAlarmState(); // TODO
    void setExternalAlarmState(bool value); // TODO
    bool getButton1();
    void setButton1(bool value);
    bool getButton2();
    void setButton2(bool value);
    bool getReader1();
    void setReader1(bool value);
    bool getReader1Keypad();
    void setReader1Keypad(bool value);
    bool getReader1Reverse();
    void setReader1Reverse(bool value);
    bool getReader1Silent();
    void setReader1Silent(bool value);
    bool getReader1Tamper();
    void setReader1Tamper(bool value);
    bool getReader1TamperInvert();
    void setReader1TamperInvert(bool value);
    bool getReader1TamperRestore();
    void setReader1TamperRestore(bool value);
    bool getReader1TamperAlarm();
    void setReader1TamperAlarm(bool value);
    bool getReader2();
    void setReader2(bool value);
    bool getReader2Keypad();
    void setReader2Keypad(bool value);
    bool getReader2Reverse();
    void setReader2Reverse(bool value);
    bool getReader2Silent();
    void setReader2Silent(bool value);
    bool getReader2Tamper();
    void setReader2Tamper(bool value);
    bool getReader2TamperInvert();
    void setReader2TamperInvert(bool value);
    bool getReader2TamperRestore();
    void setReader2TamperRestore(bool value);
    bool getReader2TamperAlarm();
    void setReader2TamperAlarm(bool value);
    bool getEnclosureTamper();
    void setEnclosureTamper(bool value);
    bool getEnclosureTamperInvert();
    void setEnclosureTamperInvert(bool value);
    bool getEnclosureTamperRestore();
    void setEnclosureTamperRestore(bool value);
    bool getEnclosureTamperAlarm();
    void setEnclosureTamperAlarm(bool value);
    bool getFireSensor();
    void setFireSensor(bool value);
    bool getFireSensorInvert();
    void setFireSensorInvert(bool value);
    bool getFireSensorRestore();
    void setFireSensorRestore(bool value);
    bool getFireSensorAlarm();
    void setFireSensorAlarm(bool value);
    bool getRebootNeeded();
    void setRebootNeeded(bool value);
    String getHostname();
    void setHostname(String value);
    String getIpAddress();
    void setIpAddress(String value);
    String getFirmwareVersion();
    void setFirmwareVersion(String value);
    String getHardwareVersion();
    void setHardwareVersion(String value);
    String getRsaPubKey();
    void setRsaPubKey(String value);
    String getRsaPrivKey();
    void setRsaPrivKey(String value);
    String getMqttUsername();
    void setMqttUsername(String value);
    String getMqttPassword();
    void setMqttPassword(String value);
    String getDeviceUsername();
    void setDeviceUsername(String value);
    String getDevicePassword();
    void setDevicePassword(String value);
    String getMqttRootCertificate();
    void setMqttRootCertificate(String value);
    String getMqttClientCertificate();
    void setMqttClientCertificate(String value);
    String getMqttClientPrivateKey();
    void setMqttClientPrivateKey(String value);
    String getNtpServer();
    void setNtpServer(String value);
    String getMqttServer();
    void setMqttServer(String value);
    String getTimezone();
    void setTimezone(String value);
    String getTimezoneName();
    void setTimezoneName(String value);
    String getIpStaticIp();
    void setIpStaticIp(String value);
    String getIpStaticMask();
    void setIpStaticMask(String value);
    String getIpStaticGateway();
    void setIpStaticGateway(String value);
    String getIpStaticDns();
    void setIpStaticDns(String value);
    String getLastBootTimeTimestamp();
    void setLastBootTimeTimestamp(String value);
    int getLastBootTimeUnix();
    void setLastBootTimeUnix(int value);
    int getMqttPort();
    void setMqttPort(int value);
    int getButton1Timeout();
    void setButton1Timeout(int value);
    int getButton2Timeout();
    void setButton2Timeout(int value);
    int getReader1BuzzerTimeout();
    void setReader1BuzzerTimeout(int value);
    int getReader2BuzzerTimeout();
    void setReader2BuzzerTimeout(int value);
    int getReader1Timeout();
    void setReader1Timeout(int value);
    int getReader2Timeout();
    void setReader2Timeout(int value);
    int getRebootNeededTimestamp();
    void setRebootNeededTimestamp(int value);
  private:
    void _initPreferences();
    void _initDatabase();
    const char* /*int*/ _openDatabase(); // ?
    void _closeDatabase();
    const char* _execPrep(const char *sql);
    void _update(); // TODO
    int _cardExist(const char *sql);
    int _getAllCardsCount(const char *sql);
    int _getLogsCount(const char *sql);
    Card _getCard(const char *sql);
    Event _getPagedEvent(const char *sql);
    const char* _addEvent(String sql); // ? - return value
    Preferences* _preferences = nullptr;
    sqlite3* _fs_db;
    sqlite3_stmt* _db_res;
    const char* _db_tail;
    const char* _db_path;
    const char* _db_file; // ?
    int _db_rc;
};

extern Filesystem* filesystem;
