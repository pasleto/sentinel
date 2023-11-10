#pragma once

#include <Preferences.h>
#include <sqlite3.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class Filesystem {
  public:
    Filesystem();
    void init();
    int openDatabase();
    void closeDatabase();
    const char* execPrep(const char *sql);
    const char* handleConfigJson(DynamicJsonDocument json_doc);
    Preferences* preferences();
    void logBoot(bool configMode);
    void logReboot();
    void logUpdate();
    void logConfigChange(String status);
    bool configMode();
    void setConfigMode(bool value);
    void switchToConfigMode();
    bool hwRebootRequest();
    void setHwRebootRequest(bool value);
  private:
    void _initPreferences();
    void _initDatabase();
    void _update(); // TODO
    Preferences* _preferences = nullptr;
    sqlite3* _fs_db;
    sqlite3_stmt* _db_res;
    const char* _db_tail;
    const char* _db_path;
    int _db_rc;
};

extern Filesystem* filesystem;
