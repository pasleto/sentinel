#pragma once

#include <Preferences.h>
#include <sqlite3.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

struct Event { // TODO
  int64_t id;
  uint32_t timestamp;
  String state;
  String initiator;
  double measurement;
  String status;
};

class Filesystem {
  public:
    Filesystem();
    void init();
    int openDatabase();
    void closeDatabase();
    const char* execPrep(const char *sql);
    const char* handleConfigJson(DynamicJsonDocument json_doc); // TODO
    void addEvent(String state, String initiator, bool has_measurement, double measurement, bool has_status, String status); // TODO - return int
    void eventAction(String job_id, String action, String sql); // TODO - return int
    int getLogsCount();
    Event getPagedEvent(int count, int page, int i);
    Preferences* preferences();
    void logBoot(bool configMode);
    void logReboot();
    void logUpdate(String currentVersion, String newVersion);
    void logConfigChange(String status);
    bool configMode();
    void setConfigMode(bool value);
    void switchToConfigMode();
    bool keepWebserverOutsideConfigMode();
  private:
    void _initPreferences();
    void _initDatabase();
    void _update(); // TODO
    int _getLogsCount(const char *sql);
    Event _getPagedEvent(const char *sql);
    void _addEvent(String sql);
    Preferences* _preferences = nullptr;
    sqlite3* _fs_db;
    sqlite3_stmt* _db_res;
    const char* _db_tail;
    const char* _db_path;
    int _db_rc;
};

extern Filesystem* filesystem;
