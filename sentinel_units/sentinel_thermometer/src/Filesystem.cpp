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

void Filesystem::init() { // TODO
  _db_path = "/littlefs/database.db";
  if (!LittleFS.begin(FORMAT_FS_IF_FAILED)) {
    Log->println(F("[FS] File System Mount Failed"));
    while(1);
  } else {
    // LittleFS.remove("/database.db"); // !
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
    _update();
    if (utils->rtcBatteryWarning()) { // ?
      addEvent("ERROR", "RTC", false, -1, true, "RTC Lost Power - Battery Error"); // TODO
    }
  }
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
    logUpdate(current_firmware, MODULE_FIRMWARE_VERSION);
    logReboot(); // ?
    utils->printRestart();
    utils->swReboot();
  }
}

Preferences* Filesystem::preferences() {
  return _preferences;
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

bool Filesystem::keepWebserverOutsideConfigMode() {
  return _preferences->getBool(preference_keep_webserver);
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
    closeDatabase();
    return err;
  }
  if (sqlite3_step(_db_res) != SQLITE_DONE) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] ERROR executing stmt ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(err);
    closeDatabase();
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
  if (!_preferences->isKey(preference_firmware_version)) {
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
  if (!_preferences->isKey(preference_ip_static)) {
    _preferences->putBool(preference_ip_static, DEFAULT_DHCP_STATIC);
  }
  if (!_preferences->isKey(preference_ip_static_ip)) {
    _preferences->putString(preference_ip_static_ip, DEFAULT_DHCP_IP);
  }
  if (!_preferences->isKey(preference_ip_static_mask)) {
    _preferences->putString(preference_ip_static_mask, DEFAULT_DHCP_MASK);
  }
  if (!_preferences->isKey(preference_ip_static_gw)) {
    _preferences->putString(preference_ip_static_gw, DEFAULT_DHCP_GW);
  }
  if (!_preferences->isKey(preference_ip_static_dns)) {
    _preferences->putString(preference_ip_static_dns, DEFAULT_DHCP_DNS);
  }
  if (!_preferences->isKey(preference_ntp_server)) {
    _preferences->putString(preference_ntp_server, DEFAULT_NTP_SERVER);
  }
  if (!_preferences->isKey(preference_timezone)) {
    _preferences->putString(preference_timezone, DEFAULT_TIMEZONE);
  }
  if (!_preferences->isKey(preference_timezone_name)) {
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
  if (!_preferences->isKey(preference_keep_webserver)) {
    _preferences->putBool(preference_keep_webserver, DEFAULT_KEEP_WEBSERVER);
  }
  if (!_preferences->isKey(preference_thermometer_1_temperature)) { // ?
    _preferences->putFloat(preference_thermometer_1_temperature, DEFAULT_THERMOMETER_TEMPERATURE);
  }
  if (!_preferences->isKey(preference_thermometer_1_humidity)) { // ?
    _preferences->putFloat(preference_thermometer_1_humidity, DEFAULT_THERMOMETER_HUMIDITY);
  }
  if (!_preferences->isKey(preference_thermometer_2_temperature)) { // ?
    _preferences->putFloat(preference_thermometer_2_temperature, DEFAULT_THERMOMETER_TEMPERATURE);
  }
  if (!_preferences->isKey(preference_thermometer_2_humidity)) { // ?
    _preferences->putFloat(preference_thermometer_2_humidity, DEFAULT_THERMOMETER_HUMIDITY);
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
    "PRAGMA auto_vacuum = 2;",
    // "CREATE TABLE IF NOT EXISTS cards (id TEXT PRIMARY KEY, value TEXT NOT NULL UNIQUE, has_validity BOOLEAN NOT NULL DEFAULT 0, valid_since NUMERIC DEFAULT 946684800, valid_until NUMERIC DEFAULT 946684800) WITHOUT ROWID;",
      // "CREATE TABLE IF NOT EXISTS events (timestamp NUMERIC NOT NULL DEFAULT (cast(strftime('%s','now') as int)), state TEXT NOT NULL, initiator TEXT NOT NULL, card_value TEXT, status TEXT);",
    "CREATE TABLE IF NOT EXISTS events (timestamp NUMERIC NOT NULL DEFAULT (cast(strftime('%s','now') as int)), state TEXT NOT NULL, initiator TEXT NOT NULL, measurement NUMERIC, status TEXT);",
    // "CREATE TRIGGER IF NOT EXISTS cards_insert_trigger BEFORE INSERT ON cards BEGIN SELECT CASE WHEN (NEW.has_validity = 1 AND (NEW.valid_since ISNULL OR NEW.valid_until ISNULL OR NEW.valid_until <= NEW.valid_since)) THEN RAISE(ABORT, 'Invalid or missing parameter/s') END; END;",
      // "CREATE TRIGGER IF NOT EXISTS events_insert_trigger BEFORE INSERT ON events BEGIN SELECT CASE WHEN (NEW.initiator LIKE 'READER@__' ESCAPE '@' AND (NEW.card_value ISNULL OR NEW.status ISNULL)) THEN RAISE(ABORT, 'Invalid or missing parameter/s') END; END;",
    // "CREATE TRIGGER IF NOT EXISTS events_insert_trigger BEFORE INSERT ON events BEGIN SELECT CASE WHEN (NEW.initiator LIKE 'PROBE@__' ESCAPE '@' AND NEW.measurement ISNULL) THEN RAISE(ABORT, 'Invalid or missing parameter/s') END; END;",
    "PRAGMA incremental_vacuum;",
  };
  if (openDatabase())
    return;
  for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
    execPrep(sqlArray[i].c_str());
  }
  closeDatabase();
}

const char* Filesystem::handleConfigJson(DynamicJsonDocument json_doc) { // TODO
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
    if (json_doc.containsKey("keep_webserver")) {
      bool json_keep_webserver = json_doc["keep_webserver"]; // default to false
      if (json_keep_webserver) {
        if (!_preferences->getBool(preference_keep_webserver)) {
          _preferences->putBool(preference_keep_webserver, true);
        }
      } else {
        if (_preferences->getBool(preference_keep_webserver)) {
          _preferences->putBool(preference_keep_webserver, false);
        }
      }
    }
    if (json_doc.containsKey("device_password_change")) { // value is provided
      bool json_device_password_change = json_doc["device_password_change"]; // default to false
      if (json_device_password_change) {
        if (json_doc.containsKey("device_password")) { // value is provided
          String json_device_password = json_doc["device_password"]; // default to ""
          json_device_password.trim();
          if (json_device_password.length() > 0) { // value is not default
            if (json_device_password.length() >= 8) { // ?
              if (json_device_password != _preferences->getString(preference_device_password)) {
                _preferences->putString(preference_device_password, json_device_password);
              }
            } else {
              return "Device password change was requested, but the value of device password field must be atleast 8 characters long!";
            }
          } else {
            return "Device password change was requested, but the value of device password field is empty!";
          }
        } else {
          return "Device password change was requested, but the device password field is missing!";
        }
      }
    }
    if (json_doc.containsKey("static_ip")) { // value is provided
      bool json_static_ip = json_doc["static_ip"]; // default to false
      if (json_static_ip) {
        if (json_doc.containsKey("ip")) {
          String json_ip = json_doc["ip"]; // default to ""
          json_ip.trim();
          if (json_ip.length() > 0) {
            if (json_doc.containsKey("mask")) {
              String json_mask = json_doc["mask"]; // default to ""
              json_mask.trim();
              if (json_mask.length() > 0) {
                if (json_doc.containsKey("gateway")) {
                  String json_gateway = json_doc["gateway"]; // default to ""
                  json_gateway.trim();
                  if (json_gateway.length() > 0) {
                    if (json_doc.containsKey("dns")) {
                      String json_dns = json_doc["dns"]; // default to ""
                      json_dns.trim();
                      if (json_dns.length() > 0) {
                        // ? - add validation for correct formats
                        if (!_preferences->getBool(preference_ip_static)) {
                          _preferences->putBool(preference_ip_static, true);
                        }
                        if (json_ip != _preferences->getString(preference_ip_static_ip)) {
                          _preferences->putString(preference_ip_static_ip, json_ip);
                        }
                        if (json_mask != _preferences->getString(preference_ip_static_mask)) {
                          _preferences->putString(preference_ip_static_mask, json_mask);
                        }
                        if (json_gateway != _preferences->getString(preference_ip_static_gw)) {
                          _preferences->putString(preference_ip_static_gw, json_gateway);
                        }
                        if (json_dns != _preferences->getString(preference_ip_static_dns)) {
                          _preferences->putString(preference_ip_static_dns, json_dns);
                        }
                      } else {
                        return "Set of static ip configuration was requested, but the value of dns server field is invalid!";
                      }
                    } else {
                      return "Set of static ip configuration was requested, but the dns server field is missing!";
                    }
                  } else {
                    return "Set of static ip configuration was requested, but the value of gateway field is invalid!";
                  }
                } else {
                  return "Set of static ip configuration was requested, but the gateway field is missing!";
                }
              } else {
                return "Set of static ip configuration was requested, but the value of subnet mask field is invalid!";
              }
            } else {
              return "Set of static ip configuration was requested, but the subnet mask field is missing!";
            }
          } else {
            return "Set of static ip configuration was requested, but the value of ip address field is invalid!";
          }
        } else {
          return "Set of static ip configuration was requested, but the ip address field is missing!";
        }
      } else {
        if (_preferences->getBool(preference_ip_static)) {
          _preferences->putBool(preference_ip_static, false);
        }
        if (String(DEFAULT_DHCP_IP) != _preferences->getString(preference_ip_static_ip)) {
          _preferences->putString(preference_ip_static_ip, DEFAULT_DHCP_IP);
        }
        if (String(DEFAULT_DHCP_MASK) != _preferences->getString(preference_ip_static_mask)) {
          _preferences->putString(preference_ip_static_mask, DEFAULT_DHCP_MASK);
        }
        if (String(DEFAULT_DHCP_GW) != _preferences->getString(preference_ip_static_gw)) {
          _preferences->putString(preference_ip_static_gw, DEFAULT_DHCP_GW);
        }
        if (String(DEFAULT_DHCP_DNS) != _preferences->getString(preference_ip_static_dns)) {
          _preferences->putString(preference_ip_static_dns, DEFAULT_DHCP_DNS);
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
    if (json_doc.containsKey("ntp")) { // value is provided
      String json_ntp = json_doc["ntp"]; // default to ""
      json_ntp.trim();
      if (json_ntp.length() > 0) { // value is not default
        if (json_ntp != _preferences->getString(preference_ntp_server)) {
          _preferences->putString(preference_ntp_server, json_ntp);
        }
      } else {
        return "Change of ntp server was requested, but the value is invalid!";
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
    if (!_preferences->getBool(preference_was_configured)) {
      _preferences->putBool(preference_was_configured, true);
    }
    setConfigMode(false);
    return "OK";
  } else {
    return "Json with empty body provided!";
  }
}

void Filesystem::_addEvent(String sql) {
  if (openDatabase())
    return;
  execPrep(sql.c_str());
  closeDatabase();
}

void Filesystem::addEvent(String state, String initiator, bool has_measurement, double measurement, bool has_status, String status) {
  if (has_measurement) {
    if (has_status) {
      String insert_sql = "INSERT INTO events(timestamp, state, initiator, measurement, status) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + measurement + "', '" + status + "');";
      _addEvent(insert_sql);
    } else {
      String insert_sql = "INSERT INTO events(timestamp, state, initiator, measurement, status) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + measurement + "', NULL);";
      _addEvent(insert_sql);
    }
  } else {
    if (has_status) {
      String insert_sql = "INSERT INTO events(timestamp, state, initiator, measurement, status) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, '" + status + "');";
      _addEvent(insert_sql);
    } else {
      String insert_sql = "INSERT INTO events(timestamp, state, initiator, measurement, status) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, NULL);";
      _addEvent(insert_sql);
    }
  }
}

void Filesystem::eventAction(String job_id, String action, String sql) { // TODO - switch for mqtt response | callback function for network functions
  if (openDatabase())
    return;
  const char * res = execPrep(sql.c_str());
  closeDatabase();
  if (res != "OK") {
    // db_card_mqtt_response(job_id.c_str(), action.c_str(), true, res); // TODO
  } else {
    // db_card_mqtt_response(job_id.c_str(), action.c_str(), false, nullptr); // TODO
  }
}

int Filesystem::_getLogsCount(const char *sql) {
  int records_count = -1;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("Failed to fetch data: "));
    Log->println(sqlite3_errmsg(_fs_db));
    closeDatabase();
    return records_count;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    records_count = sqlite3_column_int(_db_res, 0);
  }
  // sqlite3_clear_bindings(_db_res); // ?
  // _db_rc = sqlite3_reset(_db_res); // ?
  // if (_db_rc != SQLITE_OK) { // ?
  //   sqlite3_close(_fs_db); // ?
  //   return false; // ?
  // } // ?
  sqlite3_finalize(_db_res);
  return records_count;
}

int Filesystem::getLogsCount() {
  String sql = "SELECT count(*) FROM events;";
  if (openDatabase())
    return -1;
  int logs_count = _getLogsCount(sql.c_str());
  closeDatabase();
  return logs_count;
}

Event Filesystem::_getPagedEvent(const char *sql) {
  int64_t id = -1;
  uint32_t timestamp = 0;
  String state = "";
  String initiator = "";
  double measurement = -1;
  String status = "";
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("Failed to fetch data: "));
    Log->println(sqlite3_errmsg(_fs_db));
    closeDatabase();
    return { id, timestamp, state, initiator, measurement, status };
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    id = sqlite3_column_int64(_db_res, 0);
    timestamp = sqlite3_column_int(_db_res, 1);
    state = (const char *) sqlite3_column_text(_db_res, 2);
    initiator = (const char *) sqlite3_column_text(_db_res, 3);
    measurement = sqlite3_column_double(_db_res, 4);
    status = (const char *) sqlite3_column_text(_db_res, 5);
  }
  // sqlite3_clear_bindings(_db_res); // ?
  // _db_rc = sqlite3_reset(_db_res); // ?
  // if (_db_rc != SQLITE_OK) { // ?
  //   sqlite3_close(_fs_db); // ?
  //   return false; // ?
  // } // ?
  sqlite3_finalize(_db_res);
  return { id, timestamp, state, initiator, measurement, status };
}

Event Filesystem::getPagedEvent(int count, int page, int i) {
  int limit = 1;
  int offset = ((page - 1) * count) + i;
  String sql = "SELECT rowid, timestamp, state, initiator, measurement, status FROM events ORDER BY rowid LIMIT " + String(limit) + " OFFSET " + String(offset) + ";";
  if (openDatabase())
    return { -1, 0, "", "", -1, "" };
  Event out = _getPagedEvent(sql.c_str());
  closeDatabase();
  return out;
}

void Filesystem::logBoot(bool configMode) {
  addEvent("BOOT", "POWER", false, -1, false, "");
  _preferences->putInt(preference_last_boot_time, rtc->getUnixtime());
  utils->printBoot(configMode);
}

void Filesystem::logReboot() {
  addEvent("REBOOT", "POWER", false, -1, false, "");
}

void Filesystem::logUpdate(String currentVersion, String newVersion) {
  addEvent("UPDATE", "FIRMWARE", false, -1, true, currentVersion + " to " + newVersion);
}

void Filesystem::logConfigChange(String status) {
  addEvent("CHANGE", "CONFIG", false, -1, true, status);
}
