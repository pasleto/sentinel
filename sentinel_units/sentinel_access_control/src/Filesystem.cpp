#include "Filesystem.h"
#include "Utils.h"
#include "Rtc.h"
#include "Logger.h"
#include "StatusLed.h"
#include "Oled.h"
#include "config.h"
#include "prefs.h"
#include "events.h"
#include "version.h"
#include "defaults.h"
#include "CryptoHandler.h"

Filesystem* filesystem = new Filesystem();

Filesystem::Filesystem() {
  _preferences = new Preferences();
  _db_path = "/littlefs/database.db";
  _db_file = "/database.db";
}

void Filesystem::init() {
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
    // if (getDatabaseFullDelete()) { // !
      // LittleFS.remove(_db_file);
    //   setDatabaseFullDelete(false);
    // }
    _initDatabase();
    _update();
    if (utils->rtcBatteryWarning()) {
      // addEvent(event_error, event_rtc, false, "", false, "", true, event_rtc_message, event_level_error, event_type_device);
      addEvent(event_error, event_rtc, "", "", event_rtc_message, event_level_error, event_type_device);
    }
  }
}

bool Filesystem::rsaKeyPairExist() {
  return (isKeyPreference(preference_rsa_pub) && isKeyPreference(preference_rsa_priv));
}

bool Filesystem::generateRsaKeypair() { // ?
  Log->println("[RSA] Keypair generation started ...");
  RsaGenerateResponse response = cryptoHandler->rsa_keypair_generate();
  if (response.success) {
    setRsaPubKey((char*)response.pubKey);
    setRsaPrivKey((char*)response.privKey);
    Log->println("[RSA] Keypair generated");
  } else {
    Log->println("[RSA] Keypair generation failed!");
  }
  return response.success; // ?
}

void Filesystem::_update() { // TODO - handle changes when upgrading firmware (what if downgrade ???)
  String current_firmware = getFirmwareVersion();
  if (current_firmware != MODULE_FIRMWARE_VERSION) { // ? - some older firmare version found
    statusLed->setUpdating(true);
    oled->drawUpdating();
    Log->println(F("[FS] Updating"));
    // if (current_firmware == "1.4.1") { // ?
    //   // ?
    // }
    // if (current_firmware == "1.4.2") { // ?
    //   // ?
    // }
    setFirmwareVersion(MODULE_FIRMWARE_VERSION);
    logUpdate(current_firmware, MODULE_FIRMWARE_VERSION);
    logReboot(); // ?
    utils->printRestart();
    utils->swReboot();
  }
}

void Filesystem::switchToConfigMode() {
  utils->printSwitchToConfigMode();
  setConfigMode(true);
  utils->swReboot();
}

const char * Filesystem::_openDatabase() {
  _db_rc = sqlite3_open(_db_path, &_fs_db);
  if (_db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] Can't open database: "));
    Log->println(err);
    return err;
  }
  return "OK";
}

void Filesystem::_closeDatabase() {
  sqlite3_close(_fs_db);
}

const char * Filesystem::_execPrep(const char *sql) { // ? - cleanup - Log
  Log->print(F("[DB] ")); // ?
  Log->println(sql); // ?
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(err);
    sqlite3_finalize(_db_res);
    return err;
  }
  if (sqlite3_step(_db_res) != SQLITE_DONE) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] ERROR executing stmt ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(err);
    sqlite3_finalize(_db_res);
    return err;
  }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(err);
    sqlite3_finalize(_db_res);
    return err;
  }
  sqlite3_finalize(_db_res);
  return "OK";
}

void Filesystem::_initPreferences() { // TODO
  _preferences->begin("config", false);
  setStringPreferenceOnlyIfNotExist(preference_firmware_version, MODULE_FIRMWARE_VERSION);
  setStringPreferenceOnlyIfNotExist(preference_hardware_version, MODULE_HW_BOARD_NAME);
  setStringPreferenceOnlyIfNotExist(preference_hostname, utils->defaultHostname());
  setStringPreferenceOnlyIfNotExist(preference_ip_address, "UNKNOWN");
  setStringPreferenceOnlyIfNotExist(preference_device_username, DEFAULT_DEVICE_ADMIN_USERNAME);
  setStringPreferenceOnlyIfNotExist(preference_device_password, DEFAULT_DEVICE_ADMIN_PASSWORD);
  setBoolPreferenceOnlyIfNotExist(preference_ip_static, DEFAULT_DHCP_STATIC);
  setStringPreferenceOnlyIfNotExist(preference_ip_static_ip, DEFAULT_DHCP_IP);
  setStringPreferenceOnlyIfNotExist(preference_ip_static_mask, DEFAULT_DHCP_MASK);
  setStringPreferenceOnlyIfNotExist(preference_ip_static_gw, DEFAULT_DHCP_GW);
  setStringPreferenceOnlyIfNotExist(preference_ip_static_dns, DEFAULT_DHCP_DNS);
  setBoolPreferenceOnlyIfNotExist(preference_button_1, DEFAULT_BUTTON);
  setIntPreferenceOnlyIfNotExist(preference_button_1_timeout, DEFAULT_BUTTON_TIMEOUT);
  setBoolPreferenceOnlyIfNotExist(preference_button_2, DEFAULT_BUTTON);
  setIntPreferenceOnlyIfNotExist(preference_button_2_timeout, DEFAULT_BUTTON_TIMEOUT);
  setBoolPreferenceOnlyIfNotExist(preference_door_sensor, DEFAULT_DOOR_SENSOR);
  setBoolPreferenceOnlyIfNotExist(preference_door_sensor_state, DEFAULT_DOOR_SENSOR_STATE);
  setBoolPreferenceOnlyIfNotExist(preference_door_alarm, DEFAULT_DOOR_ALARM);
  setBoolPreferenceOnlyIfNotExist(preference_door_alarm_state, DEFAULT_DOOR_ALARM_STATE);
  setBoolPreferenceOnlyIfNotExist(preference_door_alarm_restore, DEFAULT_DOOR_ALARM_RESTORE);
  setIntPreferenceOnlyIfNotExist(preference_door_alarm_timeout, DEFAULT_DOOR_ALARM_TIMEOUT);
  setBoolPreferenceOnlyIfNotExist(preference_external_alarm, DEFAULT_EXTERNAL_ALARM); // TODO
  setBoolPreferenceOnlyIfNotExist(preference_external_alarm_state, DEFAULT_EXTERNAL_ALARM_STATE); // TODO
  setBoolPreferenceOnlyIfNotExist(preference_reader_1, DEFAULT_READER);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_keypad, DEFAULT_READER_KEYPAD);
  setIntPreferenceOnlyIfNotExist(preference_reader_1_buzzer_timeout, DEFAULT_READER_BUZZER_TIMEOUT);
  setIntPreferenceOnlyIfNotExist(preference_reader_1_timeout, DEFAULT_READER_TIMEOUT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_reverse, DEFAULT_READER_REVERSE);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_silent, DEFAULT_READER_SILENT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_tamper, DEFAULT_READER_TAMPER_SENSOR);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_tamper_invert, DEFAULT_READER_TAMPER_INVERT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_tamper_restore, DEFAULT_READER_TAMPER_RESTORE);
  setBoolPreferenceOnlyIfNotExist(preference_reader_1_tamper_alarm, DEFAULT_READER_TAMPER_ALARM); // ???
  setBoolPreferenceOnlyIfNotExist(preference_reader_2, DEFAULT_READER);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_keypad, DEFAULT_READER_KEYPAD);
  setIntPreferenceOnlyIfNotExist(preference_reader_2_buzzer_timeout, DEFAULT_READER_BUZZER_TIMEOUT);
  setIntPreferenceOnlyIfNotExist(preference_reader_2_timeout, DEFAULT_READER_TIMEOUT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_reverse, DEFAULT_READER_REVERSE);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_silent, DEFAULT_READER_SILENT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_tamper, DEFAULT_READER_TAMPER_SENSOR);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_tamper_invert, DEFAULT_READER_TAMPER_INVERT);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_tamper_restore, DEFAULT_READER_TAMPER_RESTORE);
  setBoolPreferenceOnlyIfNotExist(preference_reader_2_tamper_alarm, DEFAULT_READER_TAMPER_ALARM); // ???
        setBoolPreferenceOnlyIfNotExist(preference_enclosure_tamper, DEFAULT_ENCLOSURE_TAMPER_SENSOR); // ?
        setBoolPreferenceOnlyIfNotExist(preference_enclosure_tamper_invert, DEFAULT_ENCLOSURE_TAMPER_INVERT); // ?
        setBoolPreferenceOnlyIfNotExist(preference_enclosure_tamper_restore, DEFAULT_ENCLOSURE_TAMPER_RESTORE); // ?
        setBoolPreferenceOnlyIfNotExist(preference_enclosure_tamper_alarm, DEFAULT_ENCLOSURE_TAMPER_ALARM); // ?
        setBoolPreferenceOnlyIfNotExist(preference_fire_sensor, DEFAULT_FIRE_SENSOR); // ?
        setBoolPreferenceOnlyIfNotExist(preference_fire_sensor_invert, DEFAULT_FIRE_SENSOR_INVERT); // ?
        setBoolPreferenceOnlyIfNotExist(preference_fire_sensor_restore, DEFAULT_FIRE_SENSOR_RESTORE); // ?
        setBoolPreferenceOnlyIfNotExist(preference_fire_sensor_alarm, DEFAULT_FIRE_SENSOR_ALARM); // ?
  setStringPreferenceOnlyIfNotExist(preference_ntp_server, DEFAULT_NTP_SERVER);
  setStringPreferenceOnlyIfNotExist(preference_timezone, DEFAULT_TIMEZONE);
  setStringPreferenceOnlyIfNotExist(preference_timezone_name, DEFAULT_TIMEZONE_NAME);
  setStringPreferenceOnlyIfNotExist(preference_mqtt_server, DEFAULT_MQTT_SERVER);
  setIntPreferenceOnlyIfNotExist(preference_mqtt_port, DEFAULT_MQTT_PORT);
  setBoolPreferenceOnlyIfNotExist(preference_mqtt_credentials_set, DEFAULT_MQTT_CREDENTIALS);
  setStringPreferenceOnlyIfNotExist(preference_mqtt_username, DEFAULT_MQTT_USER);
  setStringPreferenceOnlyIfNotExist(preference_mqtt_password, DEFAULT_MQTT_PASS);
  setBoolPreferenceOnlyIfNotExist(preference_ssl_use, DEFAULT_SSL_USE);
  setStringPreferenceOnlyIfNotExist(preference_ssl_root_ca, DEFAULT_SSL_CA_CERTIFICATE);
  setBoolPreferenceOnlyIfNotExist(preference_ssl_validate_client, DEFAULT_SSL_VALIDATE_CLIENT);
  setStringPreferenceOnlyIfNotExist(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
  setStringPreferenceOnlyIfNotExist(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
  setBoolPreferenceOnlyIfNotExist(preference_keep_webserver, DEFAULT_KEEP_WEBSERVER);
  setBoolPreferenceOnlyIfNotExist(preference_was_configured, DEFAULT_WAS_CONFIGURED);
  setBoolPreferenceOnlyIfNotExist(preference_config_mode, DEFAULT_CONFIG_MODE);
  setIntPreferenceOnlyIfNotExist(preference_last_boot_time_unix, 0);
  setStringPreferenceOnlyIfNotExist(preference_last_boot_time_timestamp, "");
  // call on every boot
  setBoolPreference(preference_reboot_needed, false);
  setIntPreference(preference_reboot_needed_timestamp, 0);
}

void Filesystem::_initDatabase() { // TODO - events -> add initiator_id column, add packet_id column || card -> sync_mark // ? - cleanup
  String sqlArray[] = {
    // "PRAGMA auto_vacuum = 0;", // ? NONE | auto-vacuum is disabled -> database connection can be changed between "full" and "incremental" mode at any time -> changing from "none" to "full" or "incremental" can only occur when the database is new
    // "PRAGMA auto_vacuum = 1;", // ? FULL | freelist pages are moved to the end of the database file and the database file is truncated to remove the freelist pages at every transaction commit
    "PRAGMA auto_vacuum = 2;", // ? INCREMENTAL | auto-vacuuming does not occur automatically at each commit -> separate incremental_vacuum pragma must be invoked to cause the auto-vacuum to occur
    "CREATE TABLE IF NOT EXISTS cards (id TEXT PRIMARY KEY, value TEXT NOT NULL UNIQUE, can_enter BOOLEAN NOT NULL DEFAULT 0, can_exit BOOLEAN NOT NULL DEFAULT 0, has_validity BOOLEAN NOT NULL DEFAULT 0, valid_since NUMERIC DEFAULT 946684800, valid_until NUMERIC DEFAULT 946684800) WITHOUT ROWID;",
    "CREATE TABLE IF NOT EXISTS events (timestamp NUMERIC NOT NULL DEFAULT (cast(strftime('%s','now') as int)), state TEXT NOT NULL, initiator TEXT NOT NULL, value TEXT, id TEXT, status TEXT, level TEXT NOT NULL, type TEXT NOT NULL);",
    "CREATE TRIGGER IF NOT EXISTS cards_insert_trigger_validity BEFORE INSERT ON cards BEGIN SELECT CASE WHEN (NEW.has_validity = 1 AND (NEW.valid_since ISNULL OR NEW.valid_until ISNULL OR NEW.valid_until <= NEW.valid_since)) THEN RAISE(ABORT, 'Invalid or missing parameter/s') END; END;",
    "CREATE TRIGGER IF NOT EXISTS events_insert_trigger_readers BEFORE INSERT ON events BEGIN SELECT CASE WHEN (NEW.initiator LIKE 'READER@__' ESCAPE '@' AND (NEW.value ISNULL OR NEW.id ISNULL OR NEW.status ISNULL)) THEN RAISE(ABORT, 'Invalid or missing parameter/s') END; END;",
    // "PRAGMA incremental_vacuum;",
    "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('A4DCCC1D', 'A4DCCC1D', 0, 1, 1, 946684800, 1735689600);", // can_exit | 00:00 01/01/2000 - 00:00 01/01/2025 // ? - cleanup
    "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('F4EE851D', 'F4EE851D', 1, 0, 1, 946684800, 1672531200);", // can_enter | 00:00 01/01/2000 - 00:00 01/01/2023 // ? - cleanup
    "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('94C3D01D', '94C3D01D', 0, 1, 1, 1735689600, 1767225600);", // can_exit | 00:00 01/01/2025 - 00:00 01/01/2026 // ? - cleanup
    "REPLACE INTO cards(id, value, can_enter, can_exit) VALUES('F4A4B501', 'F4A4B501', 1, 0);", // can_enter // ? - cleanup
    "REPLACE INTO cards(id, value, can_enter, can_exit) VALUES('04C2B401', '04C2B401', 1, 1);", // no restrictions // ? - cleanup
  };
  for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
    if (_openDatabase() != "OK") return;
    _execPrep(sqlArray[i].c_str());
    _closeDatabase();
  }
}

const char * Filesystem::handleConfigJson(DynamicJsonDocument json_doc) { // TODO - reset tamper alarms states ???
  bool change = false; // ! - determine what need reboot when changing or what can be handled by recalling
  if (json_doc.size() > 0) {
    if (json_doc.containsKey("hostname")) { // value is provided
      String json_hostname = json_doc["hostname"]; // default to ""
      json_hostname.trim();
      if (json_hostname.length() > 0) { // value is not default
        // ? - add validation for correct format
        if (json_hostname != getHostname()) {
          setHostname(json_hostname);
          change = true;
        }
      } else {
        return "Change of device hostname was requested, but the value of hostname field is invalid!";
      }
    }
    if (json_doc.containsKey("keep_webserver")) {
      bool json_keep_webserver = json_doc["keep_webserver"]; // default to false
      if (json_keep_webserver) {
        if (!getKeepWebserverOutsideConfigMode()) {
          setKeepWebserverOutsideConfigMode(true);
          change = true;
        }
      } else {
        if (getKeepWebserverOutsideConfigMode()) {
          setKeepWebserverOutsideConfigMode(false);
          change = true;
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
              if (json_device_password != getDevicePassword()) {
                setDevicePassword(json_device_password);
                change = true;
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
                        if (!getIpStatic()) {
                          setIpStatic(true);
                          change = true;
                        }
                        if (json_ip != getIpStaticIp()) {
                          setIpStaticIp(json_ip);
                          change = true;
                        }
                        if (json_mask != getIpStaticMask()) {
                          setIpStaticMask(json_mask);
                          change = true;
                        }
                        if (json_gateway != getIpStaticGateway()) {
                          setIpStaticGateway(json_gateway);
                          change = true;
                        }
                        if (json_dns != getIpStaticDns()) {
                          setIpStaticDns(json_dns);
                          change = true;
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
        if (getIpStatic()) {
          setIpStatic(false);
          change = true;
        }
        if (String(DEFAULT_DHCP_IP) != getIpStaticIp()) {
          setIpStaticIp(DEFAULT_DHCP_IP);
          change = true;
        }
        if (String(DEFAULT_DHCP_MASK) != getIpStaticMask()) {
          setIpStaticMask(DEFAULT_DHCP_MASK);
          change = true;
        }
        if (String(DEFAULT_DHCP_GW) != getIpStaticGateway()) {
          setIpStaticGateway(DEFAULT_DHCP_GW);
          change = true;
        }
        if (String(DEFAULT_DHCP_DNS) != getIpStaticDns()) {
          setIpStaticDns(DEFAULT_DHCP_DNS);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("mqtt_server")) { // value is provided
      String json_mqtt_server = json_doc["mqtt_server"]; // default to ""
      json_mqtt_server.trim();
      if (json_mqtt_server.length() > 0) { // value is not default
        if (json_mqtt_server != getMqttServer()) {
          setMqttServer(json_mqtt_server);
          change = true;
        }
      } else {
        return "Change of mqtt server was requested, but the value of mqtt server field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_port")) { // value is provided
      int json_mqtt_port = json_doc["mqtt_port"]; // default to 0
      if (json_mqtt_port > 0) { // value is not default
        // ? - add validation for correct format
        if (json_mqtt_port != getMqttPort()) {
          setMqttPort(json_mqtt_port);
          change = true;
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
             if (getMqttPassword() != DEFAULT_MQTT_PASS && getMqttPassword().length() > 0) {// some password set -> check if password change was requested
              if (json_doc.containsKey("mqtt_password_change")) {
                bool json_mqtt_password_change = json_doc["mqtt_password_change"]; // default to false
                if (json_mqtt_password_change) {
                  if (json_doc.containsKey("mqtt_password")) {
                    String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                    json_mqtt_password.trim();
                    if (json_mqtt_password.length() > 0) {
                      if (!getMqttCredentialsUse()) {
                        setMqttCredentialsUse(true);
                        change = true;
                      }
                      if (json_mqtt_username != getMqttUsername()) {
                        setMqttUsername(json_mqtt_username);
                        change = true;
                      }
                      if (json_mqtt_password != getMqttPassword()) {
                        setMqttPassword(json_mqtt_password);
                        change = true;
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
                  if (!getMqttCredentialsUse()) {
                    setMqttCredentialsUse(true);
                    change = true;
                  }
                  if (json_mqtt_username != getMqttUsername()) {
                    setMqttUsername(json_mqtt_username);
                    change = true;
                  }
                  if (json_mqtt_password != getMqttPassword()) {
                    setMqttPassword(json_mqtt_password);
                    change = true;
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
        if (getMqttCredentialsUse()) {
          setMqttCredentialsUse(false);
          change = true;
        }
        if (String(DEFAULT_MQTT_USER) != getMqttUsername()) {
          setMqttUsername(DEFAULT_MQTT_USER);
          change = true;
        }
        if (String(DEFAULT_MQTT_PASS) != getMqttPassword()) {
          setMqttPassword(DEFAULT_MQTT_PASS);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("ntp")) { // value is provided
      String json_ntp = json_doc["ntp"]; // default to ""
      json_ntp.trim();
      if (json_ntp.length() > 0) { // value is not default
        if (json_ntp != getNtpServer()) {
          setNtpServer(json_ntp);
          change = true;
        }
      } else {
        return "Change of ntp server was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("button_1")) {
      bool json_button_1 = json_doc["button_1"]; // default to false
      if (json_button_1) {
        if (!getButton1()) {
          setButton1(true);
          change = true;
        }
      } else {
        if (getButton1()) {
          setButton1(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("button_2")) {
      bool json_button_2 = json_doc["button_2"]; // default to false
      if (json_button_2) {
        if (!getButton2()) {
          setButton2(true);
          change = true;
        }
      } else {
        if (getButton2()) {
          setButton2(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("button_1_timeout")) { // value is provided
      int json_button_1_timeout = json_doc["button_1_timeout"]; // default to 0
      if (json_button_1_timeout > 0) { // value is not default
        // ? - add validation for correct format
        if (json_button_1_timeout != getButton1Timeout()) {
          setButton1Timeout(json_button_1_timeout);
          change = true;
        }
      } else {
        return "Change of button 1 timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("button_2_timeout")) { // value is provided
      int json_button_2_timeout = json_doc["button_2_timeout"]; // default to 0
      if (json_button_2_timeout > 0) { // value is not default
        // ? - add validation for correct format
        if (json_button_2_timeout != getButton2Timeout()) {
          setButton2Timeout(json_button_2_timeout);
          change = true;
        }
      } else {
        return "Change of button 2 timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("door_sensor")) { // value is provided
      bool json_door_sensor = json_doc["door_sensor"]; // default to false
      if (json_door_sensor) {
        if (!getDoorSensor()) {
          setDoorSensor(true);
          change = true;
        }
        if (json_doc.containsKey("door_alarm")) {
          bool json_door_alarm = json_doc["door_alarm"]; // default to false
          if (json_door_alarm) {
            if (!getDoorAlarm()) {
              setDoorAlarm(true);
              change = true;
            }
            if (json_doc.containsKey("door_alarm_restore")) {
              bool json_door_alarm_restore = json_doc["door_alarm_restore"]; // default to false
              if (json_door_alarm_restore) {
                if (!getDoorAlarmRestore()) {
                  setDoorAlarmRestore(true);
                  change = true;
                }
              } else {
                if (getDoorAlarmRestore()) {
                  setDoorAlarmRestore(false);
                  change = true;
                }
              }
            }
            if (json_doc.containsKey("door_alarm_timeout")) {
              int json_door_alarm_timeout = json_doc["door_alarm_timeout"]; // default to 0
              if (json_door_alarm_timeout > 0) { // value is not default
                // ? - add validation for correct format
                if (json_door_alarm_timeout != getDoorAlarmTimeout()) {
                  setDoorAlarmTimeout(json_door_alarm_timeout);
                  change = true;
                }
              } else {
                return "Change of door alarm timeout was requested, but the value is invalid!";
              }
            }
          } else {
            if (getDoorAlarm()) {
              setDoorAlarm(false);
              change = true;
            }
            if (DEFAULT_DOOR_ALARM_STATE != getDoorAlarmState()) {
              setDoorAlarmState(DEFAULT_DOOR_ALARM_STATE);
              change = true;
            }
            if (DEFAULT_DOOR_ALARM_RESTORE != getDoorAlarmRestore()) {
              setDoorAlarmRestore(DEFAULT_DOOR_ALARM_RESTORE);
              change = true;
            }
            if (DEFAULT_DOOR_ALARM_TIMEOUT != getDoorAlarmTimeout()) {
              setDoorAlarmTimeout(DEFAULT_DOOR_ALARM_TIMEOUT);
              change = true;
            }
          }
        }
      } else {
        if (getDoorSensor()) {
          setDoorSensor(false);
          change = true;
        }
        if (getDoorAlarm()) {
          setDoorAlarm(false);
          change = true;
        }
        if (DEFAULT_DOOR_ALARM_STATE != getDoorAlarmState()) {
          setDoorAlarmState(DEFAULT_DOOR_ALARM_STATE);
          change = true;
        }
        if (DEFAULT_DOOR_ALARM_RESTORE != getDoorAlarmRestore()) {
          setDoorAlarmRestore(DEFAULT_DOOR_ALARM_RESTORE);
          change = true;
        }
        if (DEFAULT_DOOR_ALARM_TIMEOUT != getDoorAlarmTimeout()) {
          setDoorAlarmTimeout(DEFAULT_DOOR_ALARM_TIMEOUT);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_1")) {
      bool json_reader_1 = json_doc["reader_1"]; // default to false
      if (json_reader_1) {
        if (!getReader1()) {
          setReader1(true);
          change = true;
        }
      } else {
        if (getReader1()) {
          setReader1(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_2")) {
      bool json_reader_2 = json_doc["reader_2"]; // default to false
      if (json_reader_2) {
        if (!getReader2()) {
          setReader2(true);
          change = true;
        }
      } else {
        if (getReader2()) {
          setReader2(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_1_keypad")) {
      bool json_reader_1_keypad = json_doc["reader_1_keypad"]; // default to false
      if (json_reader_1_keypad) {
        if (!getReader1Keypad()) {
          setReader1Keypad(true);
          change = true;
        }
      } else {
        if (getReader1Keypad()) {
          setReader1Keypad(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_2_keypad")) {
      bool json_reader_2_keypad = json_doc["reader_2_keypad"]; // default to false
      if (json_reader_2_keypad) {
        if (!getReader2Keypad()) {
          setReader2Keypad(true);
          change = true;
        }
      } else {
        if (getReader2Keypad()) {
          setReader2Keypad(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_1_buzzer")) { // value is provided
      int json_reader_1_buzzer = json_doc["reader_1_buzzer"]; // default to 0
      if (json_reader_1_buzzer > 0) { // value is not default
        // ? - add validation for correct format
        if (json_reader_1_buzzer != getReader1BuzzerTimeout()) {
          setReader1BuzzerTimeout(json_reader_1_buzzer);
          change = true;
        }
      } else {
        return "Change of reader 1 buzzer timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("reader_2_buzzer")) { // value is provided
      int json_reader_2_buzzer = json_doc["reader_2_buzzer"]; // default to 0
      if (json_reader_2_buzzer > 0) { // value is not default
        // ? - add validation for correct format
        if (json_reader_2_buzzer != getReader2BuzzerTimeout()) {
          setReader2BuzzerTimeout(json_reader_2_buzzer);
          change = true;
        }
      } else {
        return "Change of reader 2 buzzer timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("reader_1_timeout")) { // value is provided
      int json_reader_1_timeout = json_doc["reader_1_timeout"]; // default to 0
      if (json_reader_1_timeout > 0) { // value is not default
        // ? - add validation for correct format
        if (json_reader_1_timeout != getReader1Timeout()) {
          setReader1Timeout(json_reader_1_timeout);
          change = true;
        }
      } else {
        return "Change of reader 1 lock timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("reader_2_timeout")) { // value is provided
      int json_reader_2_timeout = json_doc["reader_2_timeout"]; // default to 0
      if (json_reader_2_timeout > 0) { // value is not default
        // ? - add validation for correct format
        if (json_reader_2_timeout != getReader2Timeout()) {
          setReader2Timeout(json_reader_2_timeout);
          change = true;
        }
      } else {
        return "Change of reader 2 lock timeout was requested, but the value is invalid!";
      }
    }
    if (json_doc.containsKey("reader_1_reverse")) { // value is provided
      bool json_reader_1_reverse = json_doc["reader_1_reverse"]; // default to false
      if (json_reader_1_reverse) {
        if (!getReader1Reverse()) {
          setReader1Reverse(true);
          change = true;
        }
      } else {
        if (getReader1Reverse()) {
          setReader1Reverse(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_2_reverse")) { // value is provided
      bool json_reader_2_reverse = json_doc["reader_2_reverse"]; // default to false
      if (json_reader_2_reverse) {
        if (!getReader2Reverse()) {
          setReader2Reverse(true);
          change = true;
        }
      } else {
        if (getReader2Reverse()) {
          setReader2Reverse(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_1_silent")) { // value is provided
      bool json_reader_1_silent = json_doc["reader_1_silent"]; // default to false
      if (json_reader_1_silent) {
        if (!getReader1Silent()) {
          setReader1Silent(true);
          change = true;
        }
      } else {
        if (getReader1Silent()) {
          setReader1Silent(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_2_silent")) { // value is provided
      bool json_reader_2_silent = json_doc["reader_2_silent"]; // default to false
      if (json_reader_2_silent) {
        if (!getReader2Silent()) {
          setReader2Silent(true);
          change = true;
        }
      } else {
        if (getReader2Silent()) {
          setReader2Silent(false);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_1_tamper_sensor")) {
      bool json_reader_1_tamper_sensor = json_doc["reader_1_tamper_sensor"]; // default to false
      if (json_reader_1_tamper_sensor) {
        if (!getReader1Tamper()) {
          setReader1Tamper(true);
          change = true;
        }
        if (json_doc.containsKey("reader_1_tamper_invert")) {
          bool json_reader_1_tamper_invert = json_doc["reader_1_tamper_invert"]; // default to false
          if (json_reader_1_tamper_invert) {
            if (!getReader1TamperInvert()) {
              setReader1TamperInvert(true);
              change = true;
            }
          } else {
            if (getReader1TamperInvert()) {
              setReader1TamperInvert(false);
              change = true;
            }
          }
        }
        if (json_doc.containsKey("reader_1_tamper_restore")) {
          bool json_reader_1_tamper_restore = json_doc["reader_1_tamper_restore"]; // default to false
          if (json_reader_1_tamper_restore) {
            if (!getReader1TamperRestore()) {
              setReader1TamperRestore(true);
              change = true;
            }
          } else {
            if (getReader1TamperRestore()) {
              setReader1TamperRestore(false);
              change = true;
            }
          }
        }
      } else {
        if (getReader1Tamper()) {
          setReader1Tamper(false);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_INVERT != getReader1TamperInvert()) {
          setReader1TamperInvert(DEFAULT_READER_TAMPER_INVERT);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_RESTORE != getReader1TamperRestore()) {
          setReader1TamperRestore(DEFAULT_READER_TAMPER_RESTORE);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_ALARM != getReader1TamperAlarm()) {
          setReader1TamperAlarm(DEFAULT_READER_TAMPER_ALARM);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("reader_2_tamper_sensor")) {
      bool json_reader_2_tamper_sensor = json_doc["reader_2_tamper_sensor"]; // default to false
      if (json_reader_2_tamper_sensor) {
        if (!getReader2Tamper()) {
          setReader2Tamper(true);
          change = true;
        }
        if (json_doc.containsKey("reader_2_tamper_invert")) {
          bool json_reader_2_tamper_invert = json_doc["reader_2_tamper_invert"]; // default to false
          if (json_reader_2_tamper_invert) {
            if (!getReader2TamperInvert()) {
              setReader2TamperInvert(true);
              change = true;
            }
          } else {
            if (getReader2TamperInvert()) {
              setReader2TamperInvert(false);
              change = true;
            }
          }
        }
        if (json_doc.containsKey("reader_2_tamper_restore")) {
          bool json_reader_2_tamper_restore = json_doc["reader_2_tamper_restore"]; // default to false
          if (json_reader_2_tamper_restore) {
            if (!getReader2TamperRestore()) {
              setReader2TamperRestore(true);
              change = true;
            }
          } else {
            if (getReader2TamperRestore()) {
              setReader2TamperRestore(false);
              change = true;
            }
          }
        }
      } else {
        if (getReader2Tamper()) {
          setReader2Tamper(false);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_INVERT != getReader2TamperInvert()) {
          setReader2TamperInvert(DEFAULT_READER_TAMPER_INVERT);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_RESTORE != getReader2TamperRestore()) {
          setReader2TamperRestore(DEFAULT_READER_TAMPER_RESTORE);
          change = true;
        }
        if (DEFAULT_READER_TAMPER_ALARM != getReader2TamperAlarm()) {
          setReader2TamperAlarm(DEFAULT_READER_TAMPER_ALARM);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("enclosure_tamper_sensor")) {
      bool json_enclosure_tamper_sensor = json_doc["enclosure_tamper_sensor"]; // default to false
      if (json_enclosure_tamper_sensor) {
        if (!getEnclosureTamper()) {
          setEnclosureTamper(true);
          change = true;
        }
        if (json_doc.containsKey("enclosure_tamper_invert")) {
          bool json_enclosure_tamper_invert = json_doc["enclosure_tamper_invert"]; // default to false
          if (json_enclosure_tamper_invert) {
            if (!getEnclosureTamperInvert()) {
              setEnclosureTamperInvert(true);
              change = true;
            }
          } else {
            if (getEnclosureTamperInvert()) {
              setEnclosureTamperInvert(false);
              change = true;
            }
          }
        }
        if (json_doc.containsKey("enclosure_tamper_restore")) {
          bool json_enclosure_tamper_restore = json_doc["enclosure_tamper_restore"]; // default to false
          if (json_enclosure_tamper_restore) {
            if (!getEnclosureTamperRestore()) {
              setEnclosureTamperRestore(true);
              change = true;
            }
          } else {
            if (getEnclosureTamperRestore()) {
              setEnclosureTamperRestore(false);
              change = true;
            }
          }
        }
      } else {
        if (getEnclosureTamper()) {
          setEnclosureTamper(false);
          change = true;
        }
        if (DEFAULT_ENCLOSURE_TAMPER_INVERT != getEnclosureTamperInvert()) {
          setEnclosureTamperInvert(DEFAULT_ENCLOSURE_TAMPER_INVERT);
          change = true;
        }
        if (DEFAULT_ENCLOSURE_TAMPER_RESTORE != getEnclosureTamperRestore()) {
          setEnclosureTamperRestore(DEFAULT_ENCLOSURE_TAMPER_RESTORE);
          change = true;
        }
        if (DEFAULT_ENCLOSURE_TAMPER_ALARM != getEnclosureTamperAlarm()) {
          setEnclosureTamperAlarm(DEFAULT_ENCLOSURE_TAMPER_ALARM);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("fire_sensor")) {
      bool json_fire_sensor = json_doc["fire_sensor"]; // default to false
      if (json_fire_sensor) {
        if (!getFireSensor()) {
          setFireSensor(true);
          change = true;
        }
        if (json_doc.containsKey("fire_sensor_invert")) {
          bool json_fire_sensor_invert = json_doc["fire_sensor_invert"]; // default to false
          if (json_fire_sensor_invert) {
            if (!getFireSensorInvert()) {
              setFireSensorInvert(true);
              change = true;
            }
          } else {
            if (getFireSensorInvert()) {
              setFireSensorInvert(false);
              change = true;
            }
          }
        }
        if (json_doc.containsKey("fire_sensor_restore")) {
          bool json_fire_sensor_restore = json_doc["fire_sensor_restore"]; // default to false
          if (json_fire_sensor_restore) {
            if (!getFireSensorRestore()) {
              setFireSensorRestore(true);
              change = true;
            }
          } else {
            if (getFireSensorRestore()) {
              setFireSensorRestore(false);
              change = true;
            }
          }
        }
      } else {
        if (getFireSensor()) {
          setFireSensor(false);
          change = true;
        }
        if (DEFAULT_FIRE_SENSOR_INVERT != getFireSensorInvert()) {
          setFireSensorInvert(DEFAULT_FIRE_SENSOR_INVERT);
          change = true;
        }
        if (DEFAULT_FIRE_SENSOR_RESTORE != getFireSensorRestore()) {
          setFireSensorRestore(DEFAULT_FIRE_SENSOR_RESTORE);
          change = true;
        }
        if (DEFAULT_FIRE_SENSOR_ALARM != getFireSensorAlarm()) {
          setFireSensorAlarm(DEFAULT_FIRE_SENSOR_ALARM);
          change = true;
        }
      }
    }
    if (json_doc.containsKey("external_alarm")) { // TODO
      bool json_external_alarm = json_doc["external_alarm"]; // default to false
      if (json_external_alarm) {
        if (!getExternalAlarm()) {
          setExternalAlarm(true);
          change = true;
        }
      } else {
        if (getExternalAlarm()) {
          setExternalAlarm(false);
          change = true;
        }
        if (DEFAULT_EXTERNAL_ALARM_STATE != getExternalAlarmState()) {
          setExternalAlarmState(DEFAULT_EXTERNAL_ALARM_STATE);
          change = true;
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
                        if (!getMqttSslUse()) {
                          setMqttSslUse(true);
                          change = true;
                        }
                        if (json_ssl_root_ca != getMqttRootCertificate()) {
                          setMqttRootCertificate(json_ssl_root_ca);
                          change = true;
                        }
                        if (!getMqttValidateClientCert()) {
                          setMqttValidateClientCert(true);
                          change = true;
                        }
                        if (json_ssl_client_certificate != getMqttClientCertificate()) {
                          setMqttClientCertificate(json_ssl_client_certificate);
                          change = true;
                        }
                        if (json_ssl_client_private_key != getMqttClientPrivateKey()) {
                          setMqttClientPrivateKey(json_ssl_client_private_key);
                          change = true;
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
                if (!getMqttSslUse()) {
                  setMqttSslUse(true);
                  change = true;
                }
                if (json_ssl_root_ca != getMqttRootCertificate()) {
                  setMqttRootCertificate(json_ssl_root_ca);
                  change = true;
                }
                if (getMqttValidateClientCert()) {
                  setMqttValidateClientCert(false);
                  change = true;
                }
                if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != getMqttClientCertificate()) {
                  setMqttClientCertificate(DEFAULT_SSL_CLIENT_CERTIFICATE);
                  change = true;
                }
                if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != getMqttClientPrivateKey()) {
                  setMqttClientPrivateKey(DEFAULT_SSL_CLIENT_PRIVATE_KEY);
                  change = true;
                }
              }
            } else {
              // ? - what if field is missing ? -> enable tls, disable client validation || enable tls, do not modify client validation
              if (!getMqttSslUse()) {
                setMqttSslUse(true);
                change = true;
              }
              if (json_ssl_root_ca != getMqttRootCertificate()) {
                setMqttRootCertificate(json_ssl_root_ca);
                change = true;
              }
              if (getMqttValidateClientCert()) {
                setMqttValidateClientCert(false);
                change = true;
              }
              if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != getMqttClientCertificate()) {
                setMqttClientCertificate(DEFAULT_SSL_CLIENT_CERTIFICATE);
                change = true;
              }
              if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != getMqttClientPrivateKey()) {
                setMqttClientPrivateKey(DEFAULT_SSL_CLIENT_PRIVATE_KEY);
                change = true;
              }
            }
          } else {
            return "Tls was requested, but the value of ssl root certificate field is invalid!";
          }
        } else {
          return "Tls was requested, but the ssl root certificate field is missing!";
        }
      } else {
        if (getMqttSslUse()) {
          setMqttSslUse(false);
          change = true;
        }
        if (String(DEFAULT_SSL_CA_CERTIFICATE) != getMqttRootCertificate()) {
          setMqttRootCertificate(DEFAULT_SSL_CA_CERTIFICATE);
          change = true;
        }
        if (getMqttValidateClientCert()) {
          setMqttValidateClientCert(false);
          change = true;
        }
        if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != getMqttClientCertificate()) {
          setMqttClientCertificate(DEFAULT_SSL_CLIENT_CERTIFICATE);
          change = true;
        }
        if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != getMqttClientPrivateKey()) {
          setMqttClientPrivateKey(DEFAULT_SSL_CLIENT_PRIVATE_KEY);
          change = true;
        }
      }
    }
    if (!getWasConfigured()) {
      setWasConfigured(true);
      change = true;
    }
    // setConfigMode(false); // call from outside
    if (change) {
      return "OK";
    } else {
      return "EXIT";
    }
  } else {
    return "Json with empty body provided!";
  }
}

const char * Filesystem::_addEvent(String sql) {
  const char * dbOpenRes = _openDatabase();
  if (dbOpenRes != "OK") return dbOpenRes;
  const char * execRes = _execPrep(sql.c_str());
  _closeDatabase();
  return execRes; // ?
}

// const char * Filesystem::addEvent(String state, String initiator, bool has_value, String value, bool has_id, String id, bool has_status, String status, String level, String type) { // ! change check -
const char * Filesystem::addEvent(String state, String initiator, String value, String id, String status, String level, String type) { // ! change check -
  // if (has_value) {
  //   if (has_id) {
  //     if (has_status) { //            -> value, id, status
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + value + "', '" + id + "', '" + status + "', '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     } else { //                     -> value, id
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + value + "', '" + id + "', NULL, '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     }
  //   } else {
  //     if (has_status) { //            -> value, status
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + value + "', NULL, '" + status + "', '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     } else { //                     -> value
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', '" + value + "', NULL, NULL, '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     }
  //   }
  // } else {
  //   if (has_id) {
  //     if (has_status) { //            -> id, status
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, '" + id + "', '" + status + "', '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     } else { //                     -> id
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, '" + id + "', NULL, '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     }
  //   } else {
  //     if (has_status) { //            -> status
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, NULL, '" + status + "', '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     } else { //                     -> _
  //       String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', NULL, NULL, NULL, '" + level + "', '" + type + "');";
  //       return _addEvent(insert_sql);
  //     }
  //   }
  // }

  value = value == nullptr ? "NULL" : "'" + value + "'";    // NULLable values in database
  id = id == nullptr ? "NULL" : "'" + id + "'";             // NULLable values in database
  status = status == nullptr ? "NULL" : "'" + status + "'"; // NULLable values in database

  String insert_sql = "INSERT INTO events(timestamp, state, initiator, value, id, status, level, type) VALUES(" + String(rtc->getUnixtime()) + ", '" + state + "', '" + initiator + "', " + value + ", " + id + ", " + status + ", '" + level + "', '" + type + "');";
  return _addEvent(insert_sql);
}

const char * Filesystem::cardAction(String job_id, String action, String sql) { // TODO - switch for mqtt response | callback function for network functions
  const char * dbOpenRes = _openDatabase();
  if (dbOpenRes != "OK") return dbOpenRes;
  const char * execRes = _execPrep(sql.c_str());
  _closeDatabase();
  return execRes; // ?
  // if (res != "OK") {
  //   // db_card_mqtt_response(job_id.c_str(), action.c_str(), true, res); // TODO
  // } else {
  //   // db_card_mqtt_response(job_id.c_str(), action.c_str(), false, nullptr); // TODO
  // }
}

const char * Filesystem::eventAction(String job_id, String action, String sql) { // TODO - switch for mqtt response | callback function for network functions
  const char * dbOpenRes = _openDatabase();
  if (dbOpenRes != "OK") return dbOpenRes;
  const char * execRes = _execPrep(sql.c_str());
  _closeDatabase();
  return execRes; // ?
  // if (res != "OK") {
  //   // db_card_mqtt_response(job_id.c_str(), action.c_str(), true, res); // TODO
  // } else {
  //   // db_card_mqtt_response(job_id.c_str(), action.c_str(), false, nullptr); // TODO
  // }
}

int Filesystem::_cardExist(const char *sql) {
  int records_count = -1;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(_fs_db);
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    records_count = sqlite3_column_int(_db_res, 0);
  }
  // if (sqlite3_step(_db_res) != SQLITE_DONE) {
  //   Log->print(F("[DB] ERROR executing stmt ("));
  //   Log->print(sqlite3_step(_db_res));
  //   Log->print(F("): "));
  //   Log->println(sqlite3_errmsg(_fs_db));
  //   sqlite3_finalize(_db_res);
  //   return records_count;
  // }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  sqlite3_finalize(_db_res);
  if (records_count > 0) {
    return 1;
  } else {
    return 0;
  }
}

int Filesystem::cardExistByValue(String cardValue) {
  String sql = "SELECT count(*) FROM cards WHERE value = '" + cardValue + "';";
  if (_openDatabase() != "OK") return -1;
  int card_exist = _cardExist(sql.c_str());
  _closeDatabase();
  return card_exist;
}

int Filesystem::cardExistById(String cardId) {
  String sql = "SELECT count(*) FROM cards WHERE id = '" + cardId + "';";
  if (_openDatabase() != "OK") return -1;
  int card_exist = _cardExist(sql.c_str());
  _closeDatabase();
  return card_exist;
}

int Filesystem::_getLogsCount(const char *sql) {
  int records_count = -1;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    records_count = sqlite3_column_int(_db_res, 0);
  }
  // if (sqlite3_step(_db_res) != SQLITE_DONE) {
  //   Log->print(F("[DB] ERROR executing stmt ("));
  //   Log->print(sqlite3_step(_db_res));
  //   Log->print(F("): "));
  //   Log->println(sqlite3_errmsg(_fs_db));
  //   sqlite3_finalize(_db_res);
  //   return records_count;
  // }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  sqlite3_finalize(_db_res);
  return records_count;
}

int Filesystem::getLogsCount() {
  String sql = "SELECT count(*) FROM events;";
  if (_openDatabase() != "OK") return -1;
  int logs_count = _getLogsCount(sql.c_str());
  _closeDatabase();
  return logs_count;
}

Event Filesystem::_getPagedEvent(const char *sql) { // ! change check -
  // int64_t rowid = -1;
  // uint32_t timestamp = 0;
  // String state = "";
  // String initiator = "";
  // String value = "";
  // String id = "";
  // String status = "";
  // String level = "";
  // String type = "";
  Event out;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    // return { rowid, timestamp, state, initiator, value, id, status, level, type };
    return out;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    out.rowid = sqlite3_column_int64(_db_res, 0);
    out.timestamp = sqlite3_column_int(_db_res, 1);
    out.state = (const char *) sqlite3_column_text(_db_res, 2);
    out.initiator = (const char *) sqlite3_column_text(_db_res, 3);
    out.value = (const char *) sqlite3_column_text(_db_res, 4);
    out.id = (const char *) sqlite3_column_text(_db_res, 5);
    out.status = (const char *) sqlite3_column_text(_db_res, 6);
    out.level = (const char *) sqlite3_column_text(_db_res, 7);
    out.type = (const char *) sqlite3_column_text(_db_res, 8);
  }
  // if (sqlite3_step(_db_res) != SQLITE_DONE) {
  //   Log->print(F("[DB] ERROR executing stmt ("));
  //   Log->print(sqlite3_step(_db_res));
  //   Log->print(F("): "));
  //   Log->println(sqlite3_errmsg(_fs_db));
  //   sqlite3_finalize(_db_res);
  // //   return { rowid, timestamp, state, initiator, value, id, status, level, type };
  //   return out;
  // }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    // return { rowid, timestamp, state, initiator, value, id, status, level, type };
    return out;
  }
  sqlite3_finalize(_db_res);
  // return { rowid, timestamp, state, initiator, value, id, status, level, type };
  return out;
}

Event Filesystem::getPagedEvent(int count, int page, int card) { // ! change check -
  int limit = 1;
  int offset = ((page - 1) * count) + card;
  String sql = "SELECT rowid, timestamp, state, initiator, value, id, status, level, type FROM events ORDER BY rowid LIMIT " + String(limit) + " OFFSET " + String(offset) + ";";
  if (_openDatabase() != "OK") return Event();
  Event out = _getPagedEvent(sql.c_str());
  _closeDatabase();
  return out;
}

int Filesystem::_getAllCardsCount(const char *sql) {
  int records_count = -1;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    records_count = sqlite3_column_int(_db_res, 0);
  }
  // if (sqlite3_step(_db_res) != SQLITE_DONE) {
  //   Log->print(F("[DB] ERROR executing stmt ("));
  //   Log->print(sqlite3_step(_db_res));
  //   Log->print(F("): "));
  //   Log->println(sqlite3_errmsg(_fs_db));
  //   sqlite3_finalize(_db_res);
  //   return records_count;
  // }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    return records_count;
  }
  sqlite3_finalize(_db_res);
  return records_count;
}

int Filesystem::getAllCardsCount() {
  String sql = "SELECT count(*) FROM cards;";
  if (_openDatabase() != "OK") return -1;
  int cards_count = _getAllCardsCount(sql.c_str());
  _closeDatabase();
  return cards_count;
}

Card Filesystem::_getCard(const char *sql) { // ! change check -
  // bool success = false;
  // uint32_t current_time = rtc->getUnixtime();
  // String id = "";
  // String value = "";
  // int can_enter = -1;
  // int can_exit = -1;
  // int has_validity = -1;
  // uint32_t valid_since = 0;
  // uint32_t valid_until = 0;
  Card out;
  _db_rc = sqlite3_prepare_v2(_fs_db, sql, strlen(sql), &_db_res, &_db_tail);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed preparing sql: "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    // return { success, current_time, id, value, can_enter, can_exit, has_validity, valid_since, valid_until };
    return out;
  }
  while (sqlite3_step(_db_res) == SQLITE_ROW) {
    out.id = (const char *) sqlite3_column_text(_db_res, 0);
    out.value = (const char *) sqlite3_column_text(_db_res, 1);
    out.can_enter = sqlite3_column_int(_db_res, 2);
    out.can_exit = sqlite3_column_int(_db_res, 3);
    out.has_validity = sqlite3_column_int(_db_res, 4);
    out.valid_since = sqlite3_column_int(_db_res, 5);
    out.valid_until = sqlite3_column_int(_db_res, 6);
  }
  // if (sqlite3_step(_db_res) != SQLITE_DONE) {
  //   Log->print(F("[DB] ERROR executing stmt ("));
  //   Log->print(sqlite3_step(_db_res));
  //   Log->print(F("): "));
  //   Log->println(sqlite3_errmsg(_fs_db));
  //   sqlite3_finalize(_db_res);
  // //   return { success, current_time, id, value, can_enter, can_exit, has_validity, valid_since, valid_until };
  //   return out;
  // }
  sqlite3_clear_bindings(_db_res);
  _db_rc = sqlite3_reset(_db_res);
  if (_db_rc != SQLITE_OK) {
    Log->print(F("[DB] Failed reset ("));
    Log->print(sqlite3_step(_db_res));
    Log->print(F("): "));
    Log->println(sqlite3_errmsg(_fs_db));
    sqlite3_finalize(_db_res);
    // return { success, current_time, id, value, can_enter, can_exit, has_validity, valid_since, valid_until };
    return out;
  }
  sqlite3_finalize(_db_res);
  out.success = true;
  out.current_time = rtc->getUnixtime();
  // return { success, current_time, id, value, can_enter, can_exit, has_validity, valid_since, valid_until };
  return out;
}

Card Filesystem::getPagedCard(int count, int page, int card) { // ! change check -
  int limit = 1;
  int offset = ((page - 1) * count) + card;
  String sql = "SELECT id, value, can_enter, can_exit, has_validity, valid_since, valid_until FROM cards ORDER BY id LIMIT " + String(limit) + " OFFSET " + String(offset) + ";";
  if (_openDatabase() != "OK") return Card();
  Card out = _getCard(sql.c_str());
  _closeDatabase();
  return out;
}

Card Filesystem::getCardByValue(String cardValue) { // ! change check -
  String sql = "SELECT id, value, can_enter, can_exit, has_validity, valid_since, valid_until FROM cards WHERE value = '" + cardValue + "';";
  if (_openDatabase() != "OK") return Card();
  Card out = _getCard(sql.c_str());
  _closeDatabase();
  return out;
}

Card Filesystem::getCardById(String cardId) { // ! change check -
  String sql = "SELECT id, value, can_enter, can_exit, has_validity, valid_since, valid_until FROM cards WHERE id = '" + cardId + "';";
  if (_openDatabase() != "OK") return Card();
  Card out = _getCard(sql.c_str());
  _closeDatabase();
  return out;
}

void Filesystem::logBoot(bool configMode) { // ? - set new times when rtc is in warning -> default time will be set
  // addEvent(event_boot, event_power, false, "", false, "", false, "", event_level_info, event_type_device);
  addEvent(event_boot, event_power, "", "", "", event_level_info, event_type_device);
  setLastBootTimeUnix(rtc->getUnixtime());
  setLastBootTimeTimestamp(rtc->getTimestamp());
  utils->printBoot(configMode);
}

void Filesystem::logReboot() {
  // addEvent(event_reboot, event_power, false, "", false, "", true, event_reboot_initiated, event_level_info, event_type_device);
  addEvent(event_reboot, event_power, "", "", event_reboot_initiated, event_level_info, event_type_device);
}

void Filesystem::logUpdate(String currentVersion, String newVersion) {
  // addEvent(event_update, event_firmware, false, "", false, "", true, currentVersion + " to " + newVersion, event_level_info, event_type_device);
  addEvent(event_update, event_firmware, "", "", currentVersion + " to " + newVersion, event_level_info, event_type_device);
}

void Filesystem::logConfigChange(String status) {
  // addEvent(event_change, event_config, false, "", false, "", true, status, event_level_info, event_type_device);
  addEvent(event_change, event_config, "", "", status, event_level_info, event_type_device);
}





DynamicJsonDocument Filesystem::getConfigJson() {
  DynamicJsonDocument json_doc(12288); // ! - how to allocate size
  json_doc["config_mode"] = getConfigMode();
  json_doc["was_configured"] = getWasConfigured();
  json_doc["device_password_default"] = (getDevicePassword() == DEFAULT_DEVICE_ADMIN_PASSWORD); // ?
  json_doc["module_type"] = MODULE_TYPE;
  json_doc["firmware"] = getFirmwareVersion();
  json_doc["hardware"] = getHardwareVersion();
  json_doc["unit_id"] = utils->getMacNag();
  json_doc["mac_address"] = utils->getMac();
  json_doc["ip_address"] = getIpAddress();
  json_doc["hostname"] = getHostname();
  json_doc["keep_webserver"] = getKeepWebserverOutsideConfigMode();
  json_doc["mqtt_server"] = getMqttServer();
  json_doc["mqtt_port"] = getMqttPort();
  json_doc["mqtt_credentials"] = getMqttCredentialsUse();
  json_doc["mqtt_user"] = getMqttUsername();
  json_doc["mqtt_pass_set"] = (getMqttPassword() != DEFAULT_MQTT_PASS && getMqttPassword().length() > 0);
  json_doc["ssl_use"] = getMqttSslUse();
  json_doc["ssl_root_ca"] = getMqttRootCertificate();
  json_doc["ssl_validate_client"] = getMqttValidateClientCert();
  json_doc["ssl_client_certificate"] = getMqttClientCertificate();
  json_doc["ssl_client_private_key"] = getMqttClientPrivateKey();
  json_doc["ntp"] = getNtpServer();
  json_doc["ip_static"] = getIpStatic();
  json_doc["ip_static_ip"] = getIpStaticIp();
  json_doc["ip_static_mask"] = getIpStaticMask();
  json_doc["ip_static_gw"] = getIpStaticGateway();
  json_doc["ip_static_dns"] = getIpStaticDns();
  json_doc["button_1"] = getButton1();
  json_doc["button_1_timeout"] = getButton1Timeout();
  json_doc["button_2"] = getButton2();
  json_doc["button_2_timeout"] = getButton2Timeout();
  json_doc["door_sensor"] = getDoorSensor();
  json_doc["door_alarm"] = getDoorAlarm(); // TODO
  json_doc["door_alarm_state"] = getDoorAlarmState(); // TODO ???
  json_doc["door_alarm_restore"] = getDoorAlarmRestore(); // TODO
  json_doc["door_alarm_timeout"] = getDoorAlarmTimeout(); // TODO
  json_doc["external_alarm"] = getExternalAlarm(); // TODO
  json_doc["external_alarm_state"] = getExternalAlarmState(); // TODO ???
  json_doc["reader_1"] = getReader1();
  json_doc["reader_1_keypad"] = getReader1Keypad();
  json_doc["reader_1_buzzer_timeout"] = getReader1BuzzerTimeout();
  json_doc["reader_1_timeout"] = getReader1Timeout();
  json_doc["reader_1_reverse"] = getReader1Reverse();
  json_doc["reader_1_silent"] = getReader1Silent();
  json_doc["reader_1_tamper_sensor"] = getReader1Tamper();
  json_doc["reader_1_tamper_invert"] = getReader1TamperInvert();
  json_doc["reader_1_tamper_restore"] = getReader1TamperRestore();
  json_doc["reader_1_tamper_alarm"] = getReader1TamperAlarm();
  json_doc["reader_2"] = getReader2();
  json_doc["reader_2_keypad"] = getReader2Keypad();
  json_doc["reader_2_buzzer_timeout"] = getReader2BuzzerTimeout();
  json_doc["reader_2_timeout"] = getReader2Timeout();
  json_doc["reader_2_reverse"] = getReader2Reverse();
  json_doc["reader_2_silent"] = getReader2Silent();
  json_doc["reader_2_tamper_sensor"] = getReader2Tamper();
  json_doc["reader_2_tamper_invert"] = getReader2TamperInvert();
  json_doc["reader_2_tamper_restore"] = getReader2TamperRestore();
  json_doc["reader_2_tamper_alarm"] = getReader2TamperAlarm();
  json_doc["enclosure_tamper_sensor"] = getEnclosureTamper();
  json_doc["enclosure_tamper_invert"] = getEnclosureTamperInvert();
  json_doc["enclosure_tamper_restore"] = getEnclosureTamperRestore();
  json_doc["enclosure_tamper_alarm"] = getEnclosureTamperAlarm();
  json_doc["fire_sensor"] = getFireSensor();
  json_doc["fire_sensor_invert"] = getFireSensorInvert();
  json_doc["fire_sensor_restore"] = getFireSensorRestore();
  json_doc["fire_sensor_alarm"] = getFireSensorAlarm();
  json_doc["storage_usage"] = utils->spiffsPercentageUsage();
  json_doc["rtc_battery_warning"] = utils->rtcBatteryWarning();
  json_doc["reboot_needed"] = getRebootNeeded(); // ?
  // size_t json_size = measureJson(json_doc); // !
  // Log->print("[DEBUG] getConfigJson - measureJson(json_doc): "); // !
  // Log->println(json_size); // !
  return json_doc;
}

String Filesystem::getConfigJsonString() {
  String data_out;
  serializeJson(getConfigJson(), data_out);
  return data_out;
}

bool Filesystem::isKeyPreference(const char * key) {
  return _preferences->isKey(key);
}

bool Filesystem::getBoolPreference(const char * key) {
  return _preferences->getBool(key);
}

bool Filesystem::getBoolPreferenceWithDefault(const char * key, bool defVal) {
  return _preferences->getBool(key, defVal);
}

String Filesystem::getStringPreference(const char * key) {
  return _preferences->getString(key);
}

String Filesystem::getStringPreferenceWithDefault(const char * key, String defVal) {
  return _preferences->getString(key, defVal);
}

int Filesystem::getIntPreference(const char * key) {
  return _preferences->getInt(key);
}

int Filesystem::getIntPreferenceWithDefault(const char * key, int defVal) {
  return _preferences->getInt(key, defVal);
}

void Filesystem::setBoolPreference(const char * key, bool value) {
  _preferences->putBool(key, value);
}

void Filesystem::setStringPreference(const char * key, String value) {
  _preferences->putString(key, value);
}

void Filesystem::setIntPreference(const char * key, int value) {
  _preferences->putInt(key, value);
}

void Filesystem::setBoolPreferenceOnlyIfNotExist(const char * key, bool value) {
  if (!_preferences->isKey(key)) {
    _preferences->putBool(key, value);
  }
}

void Filesystem::setStringPreferenceOnlyIfNotExist(const char * key, String value) {
  if (!_preferences->isKey(key)) {
    _preferences->putString(key, value);
  }
}

void Filesystem::setIntPreferenceOnlyIfNotExist(const char * key, int value) {
  if (!_preferences->isKey(key)) {
    _preferences->putInt(key, value);
  }
}

bool Filesystem::getConfigMode() {
  return getBoolPreference(preference_config_mode);
}

void Filesystem::setConfigMode(bool value) {
  setBoolPreference(preference_config_mode, value);
}

bool Filesystem::getKeepWebserverOutsideConfigMode() {
  return getBoolPreference(preference_keep_webserver);
}

void Filesystem::setKeepWebserverOutsideConfigMode(bool value) {
  setBoolPreference(preference_keep_webserver, value);
}

bool Filesystem::getWasConfigured() {
  return getBoolPreference(preference_was_configured);
}

void Filesystem::setWasConfigured(bool value) {
  setBoolPreference(preference_was_configured, value);
}

bool Filesystem::getMqttSslUse() {
  return getBoolPreference(preference_ssl_use);
}

void Filesystem::setMqttSslUse(bool value) {
  setBoolPreference(preference_ssl_use, value);
}

bool Filesystem::getMqttCredentialsUse() {
  return getBoolPreference(preference_mqtt_credentials_set);
}

void Filesystem::setMqttCredentialsUse(bool value) {
  setBoolPreference(preference_mqtt_credentials_set, value);
}

bool Filesystem::getMqttValidateClientCert() {
  return getBoolPreference(preference_ssl_validate_client);
}

void Filesystem::setMqttValidateClientCert(bool value) {
  setBoolPreference(preference_ssl_validate_client, value);
}

bool Filesystem::getIpStatic() {
  return getBoolPreference(preference_ip_static);
}

void Filesystem::setIpStatic(bool value) {
  setBoolPreference(preference_ip_static, value);
}

bool Filesystem::getDoorSensor() {
  return getBoolPreference(preference_door_sensor);
}

void Filesystem::setDoorSensor(bool value) {
  setBoolPreference(preference_door_sensor, value);
}

bool Filesystem::getDoorSensorState() {
  return getBoolPreference(preference_door_sensor_state);
}

void Filesystem::setDoorSensorState(bool value) {
  setBoolPreference(preference_door_sensor_state, value);
}

bool Filesystem::getDoorAlarm() { // TODO
  return getBoolPreference(preference_door_alarm);
}

void Filesystem::setDoorAlarm(bool value) { // TODO
  setBoolPreference(preference_door_alarm, value);
}

bool Filesystem::getDoorAlarmState() { // TODO
  return getBoolPreference(preference_door_alarm_state);
}

void Filesystem::setDoorAlarmState(bool value) { // TODO
  setBoolPreference(preference_door_alarm_state, value);
}

int Filesystem::getDoorAlarmTimeout() { // TODO
  return getIntPreference(preference_door_alarm_timeout);
}

void Filesystem::setDoorAlarmTimeout(int value) { // TODO
  setIntPreference(preference_door_alarm_timeout, value);
}

bool Filesystem::getDoorAlarmRestore() { // TODO
  return getBoolPreference(preference_door_alarm_restore);
}

void Filesystem::setDoorAlarmRestore(bool value) { // TODO
  setBoolPreference(preference_door_alarm_restore, value);
}

bool Filesystem::getExternalAlarm() { // TODO
  return getBoolPreference(preference_external_alarm);
}

void Filesystem::setExternalAlarm(bool value) { // TODO
  setBoolPreference(preference_external_alarm, value);
}

bool Filesystem::getExternalAlarmState() { // TODO
  return getBoolPreference(preference_external_alarm_state);
}

void Filesystem::setExternalAlarmState(bool value) { // TODO
  setBoolPreference(preference_external_alarm_state, value);
}

bool Filesystem::getButton1() {
  return getBoolPreference(preference_button_1);
}

void Filesystem::setButton1(bool value) {
  setBoolPreference(preference_button_1, value);
}

bool Filesystem::getButton2() {
  return getBoolPreference(preference_button_2);
}

void Filesystem::setButton2(bool value) {
  setBoolPreference(preference_button_2, value);
}

bool Filesystem::getReader1() {
  return getBoolPreference(preference_reader_1);
}

void Filesystem::setReader1(bool value) {
  setBoolPreference(preference_reader_1, value);
}

bool Filesystem::getReader1Keypad() {
  return getBoolPreference(preference_reader_1_keypad);
}

void Filesystem::setReader1Keypad(bool value) {
  setBoolPreference(preference_reader_1_keypad, value);
}

bool Filesystem::getReader1Reverse() {
  return getBoolPreference(preference_reader_1_reverse);
}

void Filesystem::setReader1Reverse(bool value) {
  setBoolPreference(preference_reader_1_reverse, value);
}

bool Filesystem::getReader1Silent() {
  return getBoolPreference(preference_reader_1_silent);
}

void Filesystem::setReader1Silent(bool value) {
  setBoolPreference(preference_reader_1_silent, value);
}

bool Filesystem::getReader1Tamper() {
  return getBoolPreference(preference_reader_1_tamper);
}

void Filesystem::setReader1Tamper(bool value) {
  setBoolPreference(preference_reader_1_tamper, value);
}

bool Filesystem::getReader1TamperInvert() {
  return getBoolPreference(preference_reader_1_tamper_invert);
}

void Filesystem::setReader1TamperInvert(bool value) {
  setBoolPreference(preference_reader_1_tamper_invert, value);
}

bool Filesystem::getReader1TamperRestore() {
  return getBoolPreference(preference_reader_1_tamper_restore);
}

void Filesystem::setReader1TamperRestore(bool value) {
  setBoolPreference(preference_reader_1_tamper_restore, value);
}

bool Filesystem::getReader1TamperAlarm() {
  return getBoolPreference(preference_reader_1_tamper_alarm);
}

void Filesystem::setReader1TamperAlarm(bool value) {
  setBoolPreference(preference_reader_1_tamper_alarm, value);
}

bool Filesystem::getReader2() {
  return getBoolPreference(preference_reader_2);
}

void Filesystem::setReader2(bool value) {
  setBoolPreference(preference_reader_2, value);
}

bool Filesystem::getReader2Keypad() {
  return getBoolPreference(preference_reader_2_keypad);
}

void Filesystem::setReader2Keypad(bool value) {
  setBoolPreference(preference_reader_2_keypad, value);
}

bool Filesystem::getReader2Reverse() {
  return getBoolPreference(preference_reader_2_reverse);
}

void Filesystem::setReader2Reverse(bool value) {
  setBoolPreference(preference_reader_2_reverse, value);
}

bool Filesystem::getReader2Silent() {
  return getBoolPreference(preference_reader_2_silent);
}

void Filesystem::setReader2Silent(bool value) {
  setBoolPreference(preference_reader_2_silent, value);
}

bool Filesystem::getReader2Tamper() {
  return getBoolPreference(preference_reader_2_tamper);
}

void Filesystem::setReader2Tamper(bool value) {
  setBoolPreference(preference_reader_2_tamper, value);
}

bool Filesystem::getReader2TamperInvert() {
  return getBoolPreference(preference_reader_2_tamper_invert);
}

void Filesystem::setReader2TamperInvert(bool value) {
  setBoolPreference(preference_reader_2_tamper_invert, value);
}

bool Filesystem::getReader2TamperRestore() {
  return getBoolPreference(preference_reader_2_tamper_restore);
}

void Filesystem::setReader2TamperRestore(bool value) {
  setBoolPreference(preference_reader_2_tamper_restore, value);
}

bool Filesystem::getReader2TamperAlarm() {
  return getBoolPreference(preference_reader_2_tamper_alarm);
}

void Filesystem::setReader2TamperAlarm(bool value) {
  setBoolPreference(preference_reader_2_tamper_alarm, value);
}

bool Filesystem::getEnclosureTamper() {
  return getBoolPreference(preference_enclosure_tamper);
}

void Filesystem::setEnclosureTamper(bool value) {
  setBoolPreference(preference_enclosure_tamper, value);
}

bool Filesystem::getEnclosureTamperInvert() {
  return getBoolPreference(preference_enclosure_tamper_invert);
}

void Filesystem::setEnclosureTamperInvert(bool value) {
  setBoolPreference(preference_enclosure_tamper_invert, value);
}

bool Filesystem::getEnclosureTamperRestore() {
  return getBoolPreference(preference_enclosure_tamper_restore);
}

void Filesystem::setEnclosureTamperRestore(bool value) {
  setBoolPreference(preference_enclosure_tamper_restore, value);
}

bool Filesystem::getEnclosureTamperAlarm() {
  return getBoolPreference(preference_enclosure_tamper_alarm);
}

void Filesystem::setEnclosureTamperAlarm(bool value) {
  setBoolPreference(preference_enclosure_tamper_alarm, value);
}

bool Filesystem::getFireSensor() {
  return getBoolPreference(preference_fire_sensor);
}

void Filesystem::setFireSensor(bool value) {
  setBoolPreference(preference_fire_sensor, value);
}

bool Filesystem::getFireSensorInvert() {
  return getBoolPreference(preference_fire_sensor_invert);
}

void Filesystem::setFireSensorInvert(bool value) {
  setBoolPreference(preference_fire_sensor_invert, value);
}

bool Filesystem::getFireSensorRestore() {
  return getBoolPreference(preference_fire_sensor_restore);
}

void Filesystem::setFireSensorRestore(bool value) {
  setBoolPreference(preference_fire_sensor_restore, value);
}

bool Filesystem::getFireSensorAlarm() {
  return getBoolPreference(preference_fire_sensor_alarm);
}

void Filesystem::setFireSensorAlarm(bool value) {
  setBoolPreference(preference_fire_sensor_alarm, value);
}

bool Filesystem::getRebootNeeded() { // ?
  return getBoolPreference(preference_reboot_needed);
}

void Filesystem::setRebootNeeded(bool value) { // !
  setBoolPreference(preference_reboot_needed, value);
  if (value) {
    setRebootNeededTimestamp(rtc->getUnixtime());
  } else {
    setRebootNeededTimestamp(0);
  }
  utils->setRebootNededTrigger(value); // !
  // ? - if reboot needed -> add event
  // addEvent("REBOOT", "POWER", false, "", true, "Pending");
}

String Filesystem::getHostname() {
  return getStringPreference(preference_hostname);
}

void Filesystem::setHostname(String value) {
  setStringPreference(preference_hostname, value);
}

String Filesystem::getIpAddress() {
  return getStringPreference(preference_ip_address);
}

void Filesystem::setIpAddress(String value) {
  setStringPreference(preference_ip_address, value);
}

String Filesystem::getFirmwareVersion() {
  return getStringPreference(preference_firmware_version);
}

void Filesystem::setFirmwareVersion(String value) {
  setStringPreference(preference_firmware_version, value);
}

String Filesystem::getHardwareVersion() {
  return getStringPreference(preference_hardware_version);
}

void Filesystem::setHardwareVersion(String value) {
  setStringPreference(preference_hardware_version, value);
}

String Filesystem::getRsaPubKey() { // ?
  return getStringPreference(preference_rsa_pub);
}

void Filesystem::setRsaPubKey(String value) { // ?
  setStringPreference(preference_rsa_pub, value);
}

String Filesystem::getRsaPrivKey() { // ?
  return getStringPreference(preference_rsa_priv);
}

void Filesystem::setRsaPrivKey(String value) { // ?
  setStringPreference(preference_rsa_priv, value);
}

String Filesystem::getMqttUsername() {
  return getStringPreference(preference_mqtt_username);
}

void Filesystem::setMqttUsername(String value) {
  setStringPreference(preference_mqtt_username, value);
}

String Filesystem::getMqttPassword() {
  return getStringPreference(preference_mqtt_password);
}

void Filesystem::setMqttPassword(String value) {
  setStringPreference(preference_mqtt_password, value);
}

String Filesystem::getDeviceUsername() {
  return getStringPreference(preference_device_username);
}

void Filesystem::setDeviceUsername(String value) {
  setStringPreference(preference_device_username, value);
}

String Filesystem::getDevicePassword() {
  return getStringPreference(preference_device_password);
}

void Filesystem::setDevicePassword(String value) {
  setStringPreference(preference_device_password, value);
}

String Filesystem::getMqttRootCertificate() {
  return getStringPreference(preference_ssl_root_ca);
}

void Filesystem::setMqttRootCertificate(String value) {
  setStringPreference(preference_ssl_root_ca, value);
}

String Filesystem::getMqttClientCertificate() {
  return getStringPreference(preference_ssl_client_certificate);
}

void Filesystem::setMqttClientCertificate(String value) {
  setStringPreference(preference_ssl_client_certificate, value);
}

String Filesystem::getMqttClientPrivateKey() {
  return getStringPreference(preference_ssl_client_private_key);
}

void Filesystem::setMqttClientPrivateKey(String value) {
  setStringPreference(preference_ssl_client_private_key, value);
}

String Filesystem::getNtpServer() {
  return getStringPreference(preference_ntp_server);
}

void Filesystem::setNtpServer(String value) {
  setStringPreference(preference_ntp_server, value);
}

String Filesystem::getMqttServer() {
  return getStringPreference(preference_mqtt_server);
}

void Filesystem::setMqttServer(String value) {
  setStringPreference(preference_mqtt_server, value);
}

String Filesystem::getTimezone() {
  return getStringPreference(preference_timezone);
}

void Filesystem::setTimezone(String value) {
  setStringPreference(preference_timezone, value);
}

String Filesystem::getTimezoneName() {
  return getStringPreference(preference_timezone_name);
}

void Filesystem::setTimezoneName(String value) {
  setStringPreference(preference_timezone_name, value);
}

String Filesystem::getIpStaticIp() {
  return getStringPreference(preference_ip_static_ip);
}

void Filesystem::setIpStaticIp(String value) {
  setStringPreference(preference_ip_static_ip, value);
}

String Filesystem::getIpStaticMask() {
  return getStringPreference(preference_ip_static_mask);
}

void Filesystem::setIpStaticMask(String value) {
  setStringPreference(preference_ip_static_mask, value);
}

String Filesystem::getIpStaticGateway() {
  return getStringPreference(preference_ip_static_gw);
}

void Filesystem::setIpStaticGateway(String value) {
  setStringPreference(preference_ip_static_gw, value);
}

String Filesystem::getIpStaticDns() {
  return getStringPreference(preference_ip_static_dns);
}

void Filesystem::setIpStaticDns(String value) {
  setStringPreference(preference_ip_static_dns, value);
}

String Filesystem::getLastBootTimeTimestamp() {
  return getStringPreference(preference_last_boot_time_timestamp);
}

void Filesystem::setLastBootTimeTimestamp(String value) {
  setStringPreference(preference_last_boot_time_timestamp, value);
}

int Filesystem::getLastBootTimeUnix() {
  return getIntPreference(preference_last_boot_time_unix);
}

void Filesystem::setLastBootTimeUnix(int value) {
  setIntPreference(preference_last_boot_time_unix, value);
}

int Filesystem::getMqttPort() {
  return getIntPreference(preference_mqtt_port);
}

void Filesystem::setMqttPort(int value) {
  setIntPreference(preference_mqtt_port, value);
}

int Filesystem::getButton1Timeout() {
  return getIntPreference(preference_button_1_timeout);
}

void Filesystem::setButton1Timeout(int value) {
  setIntPreference(preference_button_1_timeout, value);
}

int Filesystem::getButton2Timeout() {
  return getIntPreference(preference_button_2_timeout);
}

void Filesystem::setButton2Timeout(int value) {
  setIntPreference(preference_button_2_timeout, value);
}

int Filesystem::getReader1BuzzerTimeout() {
  return getIntPreference(preference_reader_1_buzzer_timeout);
}

void Filesystem::setReader1BuzzerTimeout(int value) {
  setIntPreference(preference_reader_1_buzzer_timeout, value);
}

int Filesystem::getReader2BuzzerTimeout() {
  return getIntPreference(preference_reader_2_buzzer_timeout);
}

void Filesystem::setReader2BuzzerTimeout(int value) {
  setIntPreference(preference_reader_2_buzzer_timeout, value);
}

int Filesystem::getReader1Timeout() {
  return getIntPreference(preference_reader_1_timeout);
}

void Filesystem::setReader1Timeout(int value) {
  setIntPreference(preference_reader_1_timeout, value);
}

int Filesystem::getReader2Timeout() {
  return getIntPreference(preference_reader_2_timeout);
}

void Filesystem::setReader2Timeout(int value) {
  setIntPreference(preference_reader_2_timeout, value);
}

int Filesystem::getRebootNeededTimestamp() { // ?
  return getIntPreference(preference_reboot_needed_timestamp);
}

void Filesystem::setRebootNeededTimestamp(int value) { // ?
  setIntPreference(preference_reboot_needed_timestamp, value);
}
