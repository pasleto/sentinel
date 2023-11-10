#include "Filesystem.h"
#include "Utils.h"
#include "Rtc.h"
#include "Logger.h"
#include "config.h"
#include "pref.h"
#include "defaults.h"

Filesystem* filesystem = new Filesystem();

Filesystem::Filesystem() {
  _preferences = new Preferences();
}

void Filesystem::init() {
  _db_path = "/littlefs/database.db";
  if (!LittleFS.begin(FORMAT_FS_IF_FAILED)) {
    Log->println(F("[FS] File System Mount Failed"));
    while(1);
  } else {
    Log->print(F("[FS] Usage: "));
    Log->print(utils->spiffsPercentageUsage());
    Log->print(F(" % ("));
    Log->print(utils->spiffsUsed());
    Log->print(F(" / "));
    Log->print(utils->spiffsTotal());
    Log->println(F(" bytes)"));
    _initPreferences();
    sqlite3_initialize();
    _initDatabase();
  }
  _update(); // TODO
}

void Filesystem::_update() { // TODO
  String current_firmware = _preferences->getString(preference_firmware_version);
  if (current_firmware != MODULE_FIRMWARE_VERSION) { // ? - some older firmare version found
    Log->println(F("[FS] Updating"));
    // if (current_firmware == "1.4.1") { // ?
    //   // ?
    // }
    // if (current_firmware == "1.4.2") { // ?
    //   // ?
    // }
    _preferences->putString(preference_firmware_version, MODULE_FIRMWARE_VERSION); // ?
    logUpdate();
    logReboot(); // ?
    utils->printRestart();
    utils->swReboot();
  }
}

Preferences* Filesystem::preferences() {
  return _preferences;
}

bool Filesystem::hwRebootRequest() {
  return _preferences->getBool(preference_hardware_reboot);
}

void Filesystem::setHwRebootRequest(bool value) { // ? - invert value logic
  _preferences->putBool(preference_hardware_reboot, value);
}

bool Filesystem::configMode() {
  return _preferences->getBool(preference_config_mode);
}

void Filesystem::setConfigMode(bool value) {
  _preferences->putBool(preference_config_mode, value);
}

void Filesystem::switchToConfigMode() {
  utils->printSwitchToConfigMode();
  setConfigMode(true);
  utils->swReboot();
}

int Filesystem::openDatabase() {
  _db_rc = sqlite3_open(_db_path, &_fs_db);
  if (_db_rc) {
    Log->print(F("[DB] Can't open database: "));
    Log->println(sqlite3_errmsg(_fs_db));
    return _db_rc;
  }
  return _db_rc;
}

void Filesystem::closeDatabase() {
  sqlite3_close(_fs_db);
}

const char* Filesystem::execPrep(const char *sql) { // ? - cleanup
  Log->print(F("[DB] ")); // ?
  Log->println(sql); // ?
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(err);
    closeDatabase(); // sqlite3_close(_fs_db);
    return err;
  }
  if (sqlite3_step(_db_res) != SQLITE_DONE) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] ERROR executing stmt ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(err);
    closeDatabase(); // sqlite3_close(_fs_db);
    return err;
  }
  // sqlite3_clear_bindings(_db_res); // ?
  // _db_rc = sqlite3_reset(_db_res); // ?
  // if (_db_rc != SQLITE_OK) { // ?
  //   sqlite3_close(_fs_db); // ?
  //   return false; // ?
  // } // ?
  sqlite3_finalize(_db_res);
  return "OK";
}

void Filesystem::_initPreferences() { // ?
  _preferences->begin("config", false);
  if (!_preferences->isKey(preference_firmware_version)) { // ?
    _preferences->putString(preference_firmware_version, MODULE_FIRMWARE_VERSION);
  }
  if (!_preferences->isKey(preference_hostname)) {
    _preferences->putString(preference_hostname, utils->defaultHostname());
  }
  if (!_preferences->isKey(preference_device_username)) {
    _preferences->putString(preference_device_username, DEFAULT_DEVICE_ADMIN_USERNAME);
  }
  if (!_preferences->isKey(preference_device_password)) {
    _preferences->putString(preference_device_password, DEFAULT_DEVICE_ADMIN_PASSWORD);
  }
  if (!_preferences->isKey(preference_ntp_server)) { // ?
    _preferences->putString(preference_ntp_server, DEFAULT_NTP_SERVER);
  }
  if (!_preferences->isKey(preference_timezone)) { // ?
    _preferences->putString(preference_timezone, DEFAULT_TIMEZONE);
  }
  if (!_preferences->isKey(preference_timezone_name)) { // ?
    _preferences->putString(preference_timezone_name, DEFAULT_TIMEZONE_NAME);
  }
  if (!_preferences->isKey(preference_mqtt_server)) {
    _preferences->putString(preference_mqtt_server, DEFAULT_MQTT_SERVER);
  }
  if (!_preferences->isKey(preference_mqtt_port)) {
    _preferences->putInt(preference_mqtt_port, DEFAULT_MQTT_PORT);
  }
  if (!_preferences->isKey(preference_mqtt_credentials_set)) {
    _preferences->putBool(preference_mqtt_credentials_set, DEFAULT_MQTT_CREDENTIALS);
  }
  if (!_preferences->isKey(preference_mqtt_username)) {
    _preferences->putString(preference_mqtt_username, DEFAULT_MQTT_USER);
  }
  if (!_preferences->isKey(preference_mqtt_password)) {
    _preferences->putString(preference_mqtt_password, DEFAULT_MQTT_PASS);
  }
  if (!_preferences->isKey(preference_ssl_use)) {
    _preferences->putBool(preference_ssl_use, DEFAULT_SSL_USE);
  }
  if (!_preferences->isKey(preference_ssl_root_ca)) {
    _preferences->putString(preference_ssl_root_ca, DEFAULT_SSL_CA_CERTIFICATE);
  }
  if (!_preferences->isKey(preference_ssl_validate_client)) { // ?
    _preferences->putBool(preference_ssl_validate_client, DEFAULT_SSL_VALIDATE_CLIENT);
  }
  if (!_preferences->isKey(preference_ssl_client_certificate)) { // ?
    _preferences->putString(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
  }
  if (!_preferences->isKey(preference_ssl_client_private_key)) { // ?
    _preferences->putString(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
  }
  if (!_preferences->isKey(preference_gnss_mode)) {
    _preferences->putInt(preference_gnss_mode, DEFAULT_GNSS_MODE);
  }
  if (!_preferences->isKey(preference_gprs_apn)) {
    _preferences->putString(preference_gprs_apn, DEFAULT_GPRS_APN);
  }
  if (!_preferences->isKey(preference_gprs_credentials_set)) {
    _preferences->putBool(preference_gprs_credentials_set, DEFAULT_GPRS_CREDENTIALS);
  }
  if (!_preferences->isKey(preference_gprs_username)) {
    _preferences->putString(preference_gprs_username, DEFAULT_GPRS_USER);
  }
  if (!_preferences->isKey(preference_gprs_password)) {
    _preferences->putString(preference_gprs_password, DEFAULT_GPRS_PASS);
  }
  if (!_preferences->isKey(preference_last_lat)) {
    _preferences->putFloat(preference_last_lat, 0.0);
  }
  if (!_preferences->isKey(preference_last_lon)) {
    _preferences->putFloat(preference_last_lon, 0.0);
  }
  if (!_preferences->isKey(preference_last_alt)) {
    _preferences->putFloat(preference_last_alt, 0.0);
  }
  if (!_preferences->isKey(preference_last_speed)) {
    _preferences->putFloat(preference_last_speed, 0.0);
  }
  if (!_preferences->isKey(preference_last_accuracy)) {
    _preferences->putFloat(preference_last_accuracy, 0.0);
  }
  if (!_preferences->isKey(preference_last_datetime)) {
    _preferences->putString(preference_last_datetime, DateTime(2000,1,1,0,0,0).timestamp());
  }
  if (!_preferences->isKey(preference_hardware_reboot)) {
    _preferences->putBool(preference_hardware_reboot, DEFAULT_HW_REBOOT);
  }
  if (!_preferences->isKey(preference_was_configured)) {
    _preferences->putBool(preference_was_configured, DEFAULT_WAS_CONFIGURED);
  }
  if (!_preferences->isKey(preference_config_mode)) {
    _preferences->putBool(preference_config_mode, DEFAULT_CONFIG_MODE);
  }
  if (!_preferences->isKey(preference_last_boot_time)) {
    _preferences->putInt(preference_last_boot_time, 0);
  }
}

void Filesystem::_initDatabase() { // TODO
  String sqlArray[] = {

  };
  if (openDatabase())
    return;
  for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
    execPrep(sqlArray[i].c_str());
  }
  closeDatabase(); // sqlite3_close(_fs_db);
}

const char* Filesystem::handleConfigJson(DynamicJsonDocument json_doc) { // ?
  if (json_doc.size() > 0) {
    if (json_doc.containsKey("hostname")) { // value is provided
      String json_hostname = json_doc["hostname"]; // default to ""
      json_hostname.trim();
      if (json_hostname.length() > 0) { // value is not default
        // ? - add validation for correct format
        if (json_hostname != _preferences->getString(preference_hostname)) {
          _preferences->putString(preference_hostname, json_hostname);
        }
      } else {
        return "Change of device hostname was requested, but the value of hostname field is invalid!";
      }
    }
    if (json_doc.containsKey("device_password_change")) { // value is provided
      bool json_device_password_change = json_doc["device_password_change"]; // default to false
      if (json_device_password_change) {
        if (json_doc.containsKey("device_password")) { // value is provided
          String json_device_password = json_doc["device_password"]; // default to ""
          json_device_password.trim();
          if (json_device_password.length() > 0) { // value is not default
            // ? - add validation for correct format
            if (json_device_password != _preferences->getString(preference_device_password)) {
              _preferences->putString(preference_device_password, json_device_password);
            }
          } else {
            return "Device password change was requested, but the value of device password field is invalid!";
          }
        } else {
          return "Device password change was requested, but the device password field is missing!";
        }
      }
    }
    if (json_doc.containsKey("mqtt_server")) { // value is provided
      String json_mqtt_server = json_doc["mqtt_server"]; // default to ""
      json_mqtt_server.trim();
      if (json_mqtt_server.length() > 0) { // value is not default
        if (json_mqtt_server != _preferences->getString(preference_mqtt_server)) {
          _preferences->putString(preference_mqtt_server, json_mqtt_server);
        }
      } else {
        return "Change of mqtt server was requested, but the value of mqtt server field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_port")) { // value is provided
      int json_mqtt_port = json_doc["mqtt_port"]; // default to 0
      if (json_mqtt_port > 0) { // value is not default
        // ? - add validation for correct format
        if (json_mqtt_port != _preferences->getInt(preference_mqtt_port)) {
          _preferences->putInt(preference_mqtt_port, json_mqtt_port);
        }
      } else {
        return "Change of mqtt port was requested, but the value of mqtt port field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_credentials")) { // value is provided
      bool json_mqtt_credentials = json_doc["mqtt_credentials"]; // default to false
      if (json_mqtt_credentials) {
        if (json_doc.containsKey("mqtt_username")) {
          String json_mqtt_username = json_doc["mqtt_username"]; // default to ""
          json_mqtt_username.trim();
          if (json_mqtt_username.length() > 0) {
            if (_preferences->getString(preference_mqtt_password) != DEFAULT_MQTT_PASS && _preferences->getString(preference_mqtt_password).length() > 0) { // some password set -> check if password change was requested
              if (json_doc.containsKey("mqtt_password_change")) {
                bool json_mqtt_password_change = json_doc["mqtt_password_change"]; // default to false
                if (json_mqtt_password_change) {
                  if (json_doc.containsKey("mqtt_password")) {
                    String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                    json_mqtt_password.trim();
                    if (json_mqtt_password.length() > 0) {
                      if (!_preferences->getBool(preference_mqtt_credentials_set)) {
                        _preferences->putBool(preference_mqtt_credentials_set, true);
                      }
                      if (json_mqtt_username != _preferences->getString(preference_mqtt_username)) {
                        _preferences->putString(preference_mqtt_username, json_mqtt_username);
                      }
                      if (json_mqtt_password != _preferences->getString(preference_mqtt_password)) {
                        _preferences->putString(preference_mqtt_password, json_mqtt_password);
                      }
                    } else {
                      return "Mqtt password change was requested, but the value of mqtt password field is invalid!";
                    }
                  } else {
                    return "Mqtt password change was requested, but the mqtt password field is missing!";
                  }
                }
              }
            } else { // password not set -> enforce mqtt_pass
              if (json_doc.containsKey("mqtt_password")) {
                String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                json_mqtt_password.trim();
                if (json_mqtt_password.length() > 0) {
                  if (!_preferences->getBool(preference_mqtt_credentials_set)) {
                    _preferences->putBool(preference_mqtt_credentials_set, true);
                  }
                  if (json_mqtt_username != _preferences->getString(preference_mqtt_username)) {
                    _preferences->putString(preference_mqtt_username, json_mqtt_username);
                  }
                  if (json_mqtt_password != _preferences->getString(preference_mqtt_password)) {
                    _preferences->putString(preference_mqtt_password, json_mqtt_password);
                  }
                } else {
                  return "Set mqtt server credentials was requested, but the value of mqtt password field is invalid!";
                }
              } else {
                return "Set mqtt server credentials was requested, but the mqtt password field is missing!";
              }
            }
          } else {
            return "Set mqtt server credentials was requested, but the value of mqtt username field is invalid!";
          }
        } else {
          return "Set mqtt server credentials was requested, but the mqtt username field is missing!";
        }
      } else {
        if (_preferences->getBool(preference_mqtt_credentials_set)) {
          _preferences->putBool(preference_mqtt_credentials_set, false);
        }
        if (String(DEFAULT_MQTT_USER) != _preferences->getString(preference_mqtt_username)) {
          _preferences->putString(preference_mqtt_username, DEFAULT_MQTT_USER);
        }
        if (String(DEFAULT_MQTT_PASS) != _preferences->getString(preference_mqtt_password)) {
          _preferences->putString(preference_mqtt_password, DEFAULT_MQTT_PASS);
        }
      }
    }
    // if (json_doc.containsKey("ntp")) { // value is provided // ?
    //   String json_ntp = json_doc["ntp"]; // default to ""
    //   json_ntp.trim();
    //   if (json_ntp.length() > 0) { // value is not default
    //     if (json_ntp != _preferences->getString(preference_ntp_server)) {
    //       _preferences->putString(preference_ntp_server, json_ntp);
    //     }
    //   } else {
    //     return "Change of ntp server was requested, but the value is invalid!";
    //   }
    // }
    if (json_doc.containsKey("gnss_mode")) { // value is provided
      int json_gnss_mode = json_doc["gnss_mode"]; // default to 0
      if (json_gnss_mode > 0) { // value is not default
        // ? - add validation for correct format
        if (json_gnss_mode != _preferences->getInt(preference_gnss_mode)) {
          _preferences->putInt(preference_gnss_mode, json_gnss_mode);
        }
      } else {
        return "Change of gnss mode was requested, but the value of mqtt port field is invalid!";
      }
    }
    if (json_doc.containsKey("gprs_apn")) { // value is provided
      String json_gprs_apn = json_doc["gprs_apn"]; // default to ""
      json_gprs_apn.trim();
      if (json_gprs_apn.length() > 0) { // value is not default
        if (json_gprs_apn != _preferences->getString(preference_gprs_apn)) {
          _preferences->putString(preference_gprs_apn, json_gprs_apn);
        }
      } else {
        return "Change of apn was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("gprs_credentials")) { // value is provided
      bool json_gprs_credentials = json_doc["gprs_credentials"]; // default to false
      if (json_gprs_credentials) {
        if (json_doc.containsKey("gprs_username")) {
          String json_gprs_username = json_doc["gprs_username"]; // default to ""
          json_gprs_username.trim();
          if (json_gprs_username.length() > 0) {
            if (_preferences->getString(preference_gprs_password) != DEFAULT_GPRS_PASS && _preferences->getString(preference_gprs_password).length() > 0) { // some password set -> check if password change was requested
              if (json_doc.containsKey("gprs_password_change")) {
                bool json_gprs_password_change = json_doc["gprs_password_change"]; // default to false
                if (json_gprs_password_change) {
                  if (json_doc.containsKey("gprs_password")) {
                    String json_gprs_password = json_doc["gprs_password"]; // default to ""
                    json_gprs_password.trim();
                    if (json_gprs_password.length() > 0) {
                      if (!_preferences->getBool(preference_gprs_credentials_set)) {
                        _preferences->putBool(preference_gprs_credentials_set, true);
                      }
                      if (json_gprs_username != _preferences->getString(preference_gprs_username)) {
                        _preferences->putString(preference_gprs_username, json_gprs_username);
                      }

                      if (json_gprs_password != _preferences->getString(preference_gprs_password)) {
                        _preferences->putString(preference_gprs_password, json_gprs_password);
                      }
                    } else {
                      return "Gprs password change was requested, but the value of gprs password field is invalid!";
                    }
                  } else {
                    return "Gprs password change was requested, but the gprs password field is missing!";
                  }
                }
              }
            } else { // password not set -> enforce gprs_pass
              if (json_doc.containsKey("gprs_password")) {
                String json_gprs_password = json_doc["gprs_password"]; // default to ""
                json_gprs_password.trim();
                if (json_gprs_password.length() > 0) {

                  if (!_preferences->getBool(preference_gprs_credentials_set)) {
                    _preferences->putBool(preference_gprs_credentials_set, true);
                  }
                  if (json_gprs_username != _preferences->getString(preference_gprs_username)) {
                    _preferences->putString(preference_gprs_username, json_gprs_username);
                  }
                  if (json_gprs_password != _preferences->getString(preference_gprs_password)) {
                    _preferences->putString(preference_gprs_password, json_gprs_password);
                  }
                } else {
                  return "Set gprs credentials was requested, but the value of gprs password field is invalid!";
                }
              } else {
                return "Set gprs credentials was requested, but the gprs password field is missing!";
              }
            }
          } else {
            return "Set gprs credentials was requested, but the value of gprs username field is invalid!";
          }
        } else {
          return "Set gprs credentials was requested, but the gprs username field is missing!";
        }
      } else {
        if (_preferences->getBool(preference_gprs_credentials_set)) {
          _preferences->putBool(preference_gprs_credentials_set, false);
        }
        if (String(DEFAULT_GPRS_USER) != _preferences->getString(preference_gprs_username)) {
          _preferences->putString(preference_gprs_username, DEFAULT_GPRS_USER);
        }
        if (String(DEFAULT_GPRS_PASS) != _preferences->getString(preference_gprs_password)) {
          _preferences->putString(preference_gprs_password, DEFAULT_GPRS_PASS);
        }
      }
    }
    if (json_doc.containsKey("ssl_use")) { // value is provided // ? control aswell validation + cert ??
    bool json_ssl_use = json_doc["ssl_use"]; // default to false
      if (json_ssl_use) {
        if (json_doc.containsKey("ssl_root_ca")) { // value is provided
          String json_ssl_root_ca = json_doc["ssl_root_ca"]; // default to ""
          json_ssl_root_ca.trim();
          if (json_ssl_root_ca.length() > 0) { // value is not default
            if (json_doc.containsKey("ssl_validate_client")) {
              bool json_ssl_validate_client = json_doc["ssl_validate_client"]; // default to false
              if (json_ssl_validate_client) {
                if (json_doc.containsKey("ssl_client_certificate")) {
                  String json_ssl_client_certificate = json_doc["ssl_client_certificate"]; // default to ""
                  json_ssl_client_certificate.trim();
                  if (json_ssl_client_certificate.length() > 0) { // value is not default
                    if (json_doc.containsKey("ssl_client_private_key")) {
                      String json_ssl_client_private_key = json_doc["ssl_client_private_key"]; // default to ""
                      json_ssl_client_private_key.trim();
                      if (json_ssl_client_private_key.length() > 0) { // value is not default
                        if (!_preferences->getBool(preference_ssl_use)) {
                          _preferences->putBool(preference_ssl_use, true); // ?
                        }
                        if (json_ssl_root_ca != _preferences->getString(preference_ssl_root_ca)) {
                          _preferences->putString(preference_ssl_root_ca, json_ssl_root_ca); // ?
                        }
                        if (!_preferences->getBool(preference_ssl_validate_client)) {
                          _preferences->putBool(preference_ssl_validate_client, true); // ?
                        }
                        if (json_ssl_client_certificate != _preferences->getString(preference_ssl_client_certificate)) {
                          _preferences->putString(preference_ssl_client_certificate, json_ssl_client_certificate); // ?
                        }
                        if (json_ssl_client_private_key != _preferences->getString(preference_ssl_client_private_key)) {
                          _preferences->putString(preference_ssl_client_private_key, json_ssl_client_private_key); // ?
                        }
                      } else {
                        return "Validation of tls client certificate was requested, but the value of ssl client certificate private key field is invalid!";
                      }
                    } else {
                      return "Validation of tls client certificate was requested, but the ssl client certificate private key field is missing!";
                    }
                  } else {
                    return "Validation of tls client certificate was requested, but the value of ssl client certificate field is invalid!";
                  }
                } else {
                  return "Validation of tls client certificate was requested, but the ssl client certificate field is missing!";
                }
              } else {
                if (!_preferences->getBool(preference_ssl_use)) {
                  _preferences->putBool(preference_ssl_use, true); // ?
                }
                if (json_ssl_root_ca != _preferences->getString(preference_ssl_root_ca)) {
                  _preferences->putString(preference_ssl_root_ca, json_ssl_root_ca); // ?
                }
                if (_preferences->getBool(preference_ssl_validate_client)) { // ?
                  _preferences->putBool(preference_ssl_validate_client, false);
                }
                if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != _preferences->getString(preference_ssl_client_certificate)) { // ?
                  _preferences->putString(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
                }
                if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != _preferences->getString(preference_ssl_client_private_key)) { // ?
                  _preferences->putString(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
                }
              }
            } else {
              // ? - what if field is missing ? -> enable tls, disable client validation || enable tls, do not modify client validation
              if (!_preferences->getBool(preference_ssl_use)) {
                _preferences->putBool(preference_ssl_use, true); // ?
              }
              if (json_ssl_root_ca != _preferences->getString(preference_ssl_root_ca)) {
                _preferences->putString(preference_ssl_root_ca, json_ssl_root_ca); // ?
              }
              if (_preferences->getBool(preference_ssl_validate_client)) { // ?
                _preferences->putBool(preference_ssl_validate_client, false);
              }
              if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != _preferences->getString(preference_ssl_client_certificate)) { // ?
                _preferences->putString(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
              }
              if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != _preferences->getString(preference_ssl_client_private_key)) { // ?
                _preferences->putString(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
              }
            }
          } else {
            return "Tls was requested, but the value of ssl root certificate field is invalid!";
          }
        } else {
          return "Tls was requested, but the ssl root certificate field is missing!";
        }
      } else {
        if (_preferences->getBool(preference_ssl_use)) {
          _preferences->putBool(preference_ssl_use, false); // ?
        }
        if (String(DEFAULT_SSL_CA_CERTIFICATE) != _preferences->getString(preference_ssl_root_ca)) {
          _preferences->putString(preference_ssl_root_ca, DEFAULT_SSL_CA_CERTIFICATE); // ?
        }
        if (_preferences->getBool(preference_ssl_validate_client)) { // ?
          _preferences->putBool(preference_ssl_validate_client, false);
        }
        if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != _preferences->getString(preference_ssl_client_certificate)) { // ?
          _preferences->putString(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
        }
        if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != _preferences->getString(preference_ssl_client_private_key)) { // ?
          _preferences->putString(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
        }
      }
    }
    if (!_preferences->getBool(preference_hardware_reboot)) {
      _preferences->putBool(preference_hardware_reboot, true);
    }
    if (!_preferences->getBool(preference_was_configured)) {
      _preferences->putBool(preference_was_configured, true);
    }
    setConfigMode(false);
    return "OK";
  } else {
    return "Json with empty body provided!";
  }
}

// void Filesystem::_addEvent(String sql) {
//   if (openDatabase())
//     return;
//   execPrep(sql.c_str());
//   closeDatabase(); // sqlite3_close(_fs_db);
// }

// void Filesystem::addEvent(String state, String initiator, bool has_card, String card_value, bool has_status, String status) {
//   if (has_card) {
//     if (has_status) {
//       String insert_sql = "INSERT INTO events(state, initiator, card_value, status) VALUES('" + state + "', '" + initiator + "', '" + card_value + "', '" + status + "');";
//       _addEvent(insert_sql);
//     } else {
//       String insert_sql = "INSERT INTO events(state, initiator, card_value, status) VALUES('" + state + "', '" + initiator + "', '" + card_value + "', NULL);";
//       _addEvent(insert_sql);
//     }
//   } else {
//     if (has_status) {
//       String insert_sql = "INSERT INTO events(state, initiator, card_value, status) VALUES('" + state + "', '" + initiator + "', NULL, '" + status + "');";
//       _addEvent(insert_sql);
//     } else {
//       String insert_sql = "INSERT INTO events(state, initiator, card_value, status) VALUES('" + state + "', '" + initiator + "', NULL, NULL);";
//       _addEvent(insert_sql);
//     }
//   }
// }

void Filesystem::logBoot(bool configMode) {
  // addEvent("BOOT", "POWER", false, "", false, "");
  _preferences->putInt(preference_last_boot_time, rtc->getUnixtime());
  utils->printBoot(configMode); // !
}

void Filesystem::logReboot() {
  // addEvent("REBOOT", "POWER", false, "", false, "");
}

void Filesystem::logUpdate() {
  // addEvent("UPDATE", "FIRMWARE", false, "", false, "");
}

void Filesystem::logConfigChange(String status) {
  // addEvent("CHANGE", "CONFIG", false, "", true, status);
}
