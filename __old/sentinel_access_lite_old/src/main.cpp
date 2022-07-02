#include <Arduino.h>
#include <ETH.h>
#include <Wiegand.h>
#include <Wire.h>
#include <SPI.h>
#include <sqlite3.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <RTClib.h>
// #include <WebSocketsClient.h>
// #include <SocketIOclient.h>
// #include <ArduinoJson.h>

#include "configuration.h" // project configuration file

// ----------------------------------------------------------------------------------------------------------------

Wiegand wg;
RTC_DS3231 rtc;
sqlite3 *fs_db;
// SocketIOclient socketIO;

// ----------------------------------------------------------------------------------------------------------------

const String default_lock_id =  "01"; // because this module has only one reader, but to make it more universal

String backend_server = DEFAULT_BACKEND_SERVER;
int backend_port = DEFAULT_BACKEND_PORT;
String wsio_namespace = DEFAULT_WSIO_NAMESPACE;
String ntp_server = DEFAULT_NTP_SERVER;
String timezone = DEFAULT_TIMEZONE;
bool use_secure = DEFAULT_USE_SECURE;
bool data_reverse = DEFAULT_DATA_REVERSE;
bool silent_mode = DEFAULT_SILENT_MODE;
int lock_timeout = DEFAULT_LOCK_TIMEOUT;
int door_sensor = DEFAULT_DOOR_SENSOR;
int button_lock_timeout = DEFAULT_BUTTON_LOCK_TIMEOUT;

// bool wsio_connected = false;

bool eth_connected = false;
String eth_hostname;
String mac_address_nag;
String mac_address_raw;
IPAddress eth_ip_address;

bool wg_connected = false;

const int button_debounce_time = 500;
const int wg_error_timeout = 1500;
const int wg_panic_timeout = 1500;
unsigned long wg_last_time = 0;
bool wg_halt = false;
bool wg_success = false;
bool wg_error = false;
bool wg_panic = false;

const int door_sensor_debounce_time = 500;
unsigned long door_sensor_last_time = 0;
int door_sensor_last_state = 3; // default is invalid state

unsigned long button_press_last_time = 0;
int button_last_state = 1;
unsigned long button_last_time = 0;
bool button_halt = false;
bool button_success = false;

// ----------------------------------------------------------------------------------------------------------------

// void blockModeEnable() {
//   wg_halt = true;
//   wg_success = false;
//   wg_error = false;
//   wg_panic = false;
//   digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
//   digitalWrite(ESP_WG_RED_LED, LOW); // ON
//   digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
// }

// void blockModeDisable() {
//   wg_halt = false;
//   wg_success = false;
//   wg_error = false;
//   wg_panic = false;
//   digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
//   digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
//   digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
// }

// void freeModeEnable() {
//   wg_halt = true;
//   wg_success = false;
//   wg_error = false;
//   wg_panic = false;
//   digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
//   digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
//   digitalWrite(ESP_LOCK_TRIGGER, LOW); // ON
// }

// void freeModeDisable() {
//   wg_halt = false;
//   wg_success = false;
//   wg_error = false;
//   wg_panic = false;
//   digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
//   digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
//   digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
// }

void readerBuzzerOn(/*String readerID*/) {
  if (!silent_mode) digitalWrite(ESP_WG_BUZZER, LOW); // ON
}

void readerBuzzerOff(/*String readerID*/) {
  if (!silent_mode) digitalWrite(ESP_WG_BUZZER, HIGH); // OFF
}

void readerErrorStatus(/*String readerID*/) {
  wg_last_time = millis();
  wg_halt = true;
  wg_error = true;
  digitalWrite(ESP_WG_RED_LED, LOW); // ON
}

void readerPanicStatus(/*String readerID*/) {
  wg_last_time = millis();
  wg_halt = true;
  wg_panic = true;
  digitalWrite(ESP_WG_RED_LED, LOW); // ON
  digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
}

void readerSuccessStatus(/*String readerID*/) {
  wg_last_time = millis();
  wg_halt = true;
  wg_success = true;
  button_halt = true;
  digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
  digitalWrite(ESP_LOCK_TRIGGER, LOW); // ON
}

void buttonSuccessStatus() {
  button_last_time = millis();
  button_success = true;
  button_halt = true;
  wg_halt = true;
  if (wg_error) {
    wg_error = false;
    digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
  }
  if (wg_panic) {
    wg_panic = false;
    digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
  }
  if (wg_connected) digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
  digitalWrite(ESP_LOCK_TRIGGER, LOW); // ON
}

void updateNtpTime() {
  configTzTime(timezone.c_str(), ntp_server.c_str());
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)){
    Serial.print("[NTP] Received Time From NTP Server - ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    rtc.adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    Serial.print("[RTC] Time Updated From NTP Server - ");
    Serial.println(rtc.now().timestamp());
  }
}

void setupRtc() { // TODO
  if (!rtc.begin()) {
    Serial.println("[RTC] Error");
    while (1);
  }
  Serial.println("[RTC] Success");
  if (rtc.lostPower()) {
    Serial.println("[RTC] Lost power, setting default date (1/1/2000 00:00:00)");
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // 1.1. 1970 00:00:00
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // January 21, 2014 at 3am
    // TODO - notify to change RTC battery
  }
}

void setupEspPins() {
  pinMode(ESP_LOCK_TRIGGER, OUTPUT);
  digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
  pinMode(ESP_RXD_LED, OUTPUT);
  pinMode(ESP_TXD_LED, OUTPUT);
  digitalWrite(ESP_RXD_LED, HIGH); // OFF
  digitalWrite(ESP_TXD_LED, HIGH); // OFF
  pinMode(ESP_WG_D0, INPUT);
  pinMode(ESP_WG_D1, INPUT);
  pinMode(ESP_WG_GREEN_LED, OUTPUT);
  digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
  pinMode(ESP_WG_RED_LED, OUTPUT);
  digitalWrite(ESP_WG_RED_LED, LOW); // ON
  pinMode(ESP_WG_BUZZER, OUTPUT);
  digitalWrite(ESP_WG_BUZZER, HIGH); // OFF
  pinMode(ESP_DOOR_SENSOR, INPUT); // external input pull-up resistor
  pinMode(ESP_BUTTON, INPUT); // external input pull-up resistor
  Serial.println("[ESP] Success");
}

// void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
//   switch(type) {
//     case sIOtype_DISCONNECT:
//       Serial.println("[WSIO] Disconnected!");
//       wsio_connected = false;
//       break;
//     case sIOtype_CONNECT:
//       Serial.println("[WSIO] Connected!");
//       wsio_connected = true;
//       socketIO.send(sIOtype_CONNECT, "/"); // join default namespace (no auto join in Socket.IO V3)
//       break;
//     case sIOtype_ACK:
//       Serial.printf("[WSIO] get ack: %u\n", length);
//       break;
//     case sIOtype_ERROR:
//       Serial.printf("[WSIO] get error: %u\n", length);
//       break;
//     case sIOtype_BINARY_EVENT:
//       Serial.printf("[WSIO] get binary: %u\n", length);
//       break;
//     case sIOtype_BINARY_ACK:
//       Serial.printf("[WSIO] get binary ack: %u\n", length);
//       break;
//   }
// }

// void setupWebsockets() {
//   socketIO.beginSSL(backend_server, backend_port, "/socket.io/?EIO=4"); // socketIO.beginSSL("192.168.0.10", 443, "/socket.io/?EIO=4"); // socketIO.begin("10.11.100.100", 8880, "/socket.io/?EIO=4");
//   socketIO.onEvent(socketIOEvent);
// }

int openDb(const char *filename, sqlite3 **db) {
  int rc = sqlite3_open(filename, db);
  if (rc) {
    Serial.print("[DB] Can't open database: ");
    Serial.println(sqlite3_errmsg(*db));
    return rc;
  } 
  return rc;
}

static int dbCallback(void *data, int dataLength, char **colValue, char **colName) {
   Serial.println((const char*)data);
  // Serial.printf("     %s: ", (const char*)data);
  // Serial.println();
  // Serial.print("     Response Length: "); // TODO - test
  // Serial.println(dataLength);
  // for (int i = 0; i<dataLength; i++){
  //   Serial.printf("     %s = %s\n", colName[i], colValue[i] ? colValue[i] : "NULL");
  // }
  // // Serial.printf("\n");
  return 0;
}

int dbExec(sqlite3 *db, const char *sql) {
  const char* data = "Callback function called";
  char *dbErrorMessage = 0;

  Serial.print("[DB] ");
  Serial.println(sql); // TODO - test

  int rc = sqlite3_exec(db, sql, dbCallback, (void*)data, &dbErrorMessage);
  if (rc != SQLITE_OK) {
    Serial.print("[DB] SQL error (");
    Serial.print(rc);
    Serial.print("): ");
    Serial.println(dbErrorMessage);
    sqlite3_free(dbErrorMessage);
  } 
  return rc;
}

void dbInsertCardLog(String sql) { // TODO
  // TODO - check if send to server or log into database

  int rc = dbExec(fs_db, sql.c_str());
  if (rc != SQLITE_OK) {
    sqlite3_close(fs_db);
    return;
  }
}

void dbInsertButtonLog(/*String button_id*/) { // TODO
  String timestamp = rtc.now().timestamp();
  
  // TODO - check if send to server or log into database

  if (openDb("/spiffs/database.db", &fs_db))
    return;
  String sql = "INSERT INTO events_log VALUES ('button', NULL, '" + timestamp + "', '" + mac_address_nag + "', NULL, '" + default_lock_id + "', 1, 0)";
  int rc = dbExec(fs_db, sql.c_str());
  if (rc != SQLITE_OK) {
    sqlite3_close(fs_db);
    return;
  }
  sqlite3_close(fs_db);
}

void dbInsertDoorLog(/*String lock_id*/ int status) { // TODO 
  String timestamp = rtc.now().timestamp();
  // TODO - check if send to server or log into database
  // ? maybe send log only to server with protocol and not log into database
  // status
    // 1 - opened
    // 2 - closed
  if (openDb("/spiffs/database.db", &fs_db))
    return;
  String sql = "INSERT INTO events_log VALUES ('door', NULL, '" + timestamp + "', '" + mac_address_nag + "', NULL, '" + default_lock_id + "', 1, " + status + ")";
  int rc = dbExec(fs_db, sql.c_str());
  if (rc != SQLITE_OK) {
    sqlite3_close(fs_db);
    return;
  }
  sqlite3_close(fs_db);
}

// void dbInsertCard(String hex, String secure_hex) { // TODO
//   // TODO - check if card is already in database
//   // TODO - check for card update (secure change, ...)
//   // TODO - select card with rowid (SELECT rowid, * FROM table ...)
//   int row_id;
//   // String sql = "INSERT INTO cards VALUES ('" + hex + "', " + (int) is_secure + ")";
//   String insert_sql = "INSERT INTO cards VALUES ('" + hex + "', '" + secure_hex + "')";
//   // String insert_sql = "INSERT INTO cards VALUES ('" + hex + "', NULL)";
//   // String insert_sql = "INSERT INTO cards VALUES (NULL, '" + secure_hex + "')";
//   int rc_insert = dbExec(fs_db, insert_sql.c_str());
//   if (rc_insert != SQLITE_OK) {
//     sqlite3_close(fs_db);
//     return;
//   }
//   // String update_sql = "UPDATE cards SET card_hex = '" + hex + "' WHERE secure_hex = '" + secure_hex + "'";
//   // String update_sql = "UPDATE cards SET secure_hex = '" + secure_hex + "' WHERE card_hex = '" + hex + "'";
//   String update_sql = "UPDATE cards SET card_hex = '" + hex + "', secure_hex = '" + secure_hex + "' WHERE rowid = " + row_id;
//   int rc_update = dbExec(fs_db, update_sql.c_str());
//   if (rc_update != SQLITE_OK) {
//     sqlite3_close(fs_db);
//     return;
//   }
// }

void initDatabaseFile(const char *database_path, sqlite3 *database_instance) { // TODO
  if (openDb(database_path, &database_instance))
    return;

  // String config_create_query = "CREATE TABLE IF NOT EXISTS config (variable TEXT NOT NULL UNIQUE, value)"; 
  String config_create_query = "CREATE TABLE IF NOT EXISTS config (variable TEXT, value)";
  int rc_config = dbExec(database_instance, config_create_query.c_str());
  if (rc_config != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  // String cards_create_query = "CREATE TABLE IF NOT EXISTS cards (card_hex TEXT UNIQUE, secure_hex TEXT UNIQUE)"; // TODO - maybe apply restrictions for cards (hours when user can access, days in week when user can access)
  String cards_create_query = "CREATE TABLE IF NOT EXISTS cards (card_hex TEXT, secure_hex TEXT)";
  int rc_cards = dbExec(database_instance, cards_create_query.c_str());
  if (rc_cards != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  // String events_log_create_query = "CREATE TABLE IF NOT EXISTS events_log (initiator TEXT NOT NULL, card_hex TEXT, timestamp TEXT NOT NULL, unit_id TEXT NOT NULL, reader_id TEXT, lock_id TEXT NOT NULL, is_success INTEGER NOT NULL, status_code INTEGER)";
  String events_log_create_query = "CREATE TABLE IF NOT EXISTS events_log (initiator TEXT, card_hex TEXT, timestamp TEXT, unit_id TEXT, reader_id TEXT, lock_id TEXT, is_success INTEGER, status_code INTEGER)";
  int rc_events_log = dbExec(database_instance, events_log_create_query.c_str());
  if (rc_events_log != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }


  String config_fw_version_insert_query = "INSERT INTO config VALUES ('fw_version','" + String(MODULE_FIRMWARE_VERSION) + "')";
  int rc_config_fw_version = dbExec(database_instance, config_fw_version_insert_query.c_str());
  if (rc_config_fw_version != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_ntp_server_insert_query = "INSERT INTO config VALUES ('ntp_server','" + String(DEFAULT_NTP_SERVER) + "')";
  int rc_config_ntp_server = dbExec(database_instance, config_ntp_server_insert_query.c_str());
  if (rc_config_ntp_server != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_time_zone_insert_query = "INSERT INTO config VALUES ('time_zone','" + String(DEFAULT_TIMEZONE) + "')"; // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  int rc_config_time_zone = dbExec(database_instance, config_time_zone_insert_query.c_str());
  if (rc_config_time_zone != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_backend_server_insert_query = "INSERT INTO config VALUES ('backend_server','" + String(DEFAULT_BACKEND_SERVER) + "')";
  int rc_config_backend_server = dbExec(database_instance, config_backend_server_insert_query.c_str());
  if (rc_config_backend_server != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_backend_port_insert_query = "INSERT INTO config VALUES ('backend_port','" + String(DEFAULT_BACKEND_PORT) + "')";
  int rc_config_backend_port = dbExec(database_instance, config_backend_port_insert_query.c_str());
  if (rc_config_backend_port != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_lock_timeout_insert_query = "INSERT INTO config VALUES ('lock_timeout','" + String(DEFAULT_LOCK_TIMEOUT) + "')";
  int rc_config_lock_timeout = dbExec(database_instance, config_lock_timeout_insert_query.c_str());
  if (rc_config_lock_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_door_sensor_insert_query = "INSERT INTO config VALUES ('door_sensor','" + String(DEFAULT_DOOR_SENSOR) + "')";
  int rc_config_door_sensor = dbExec(database_instance, config_door_sensor_insert_query.c_str());
  if (rc_config_door_sensor != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_button_lock_timeout_insert_query = "INSERT INTO config VALUES ('button_lock_timeout','" + String(DEFAULT_BUTTON_LOCK_TIMEOUT) + "')";
  int rc_config_button_lock_timeout = dbExec(database_instance, config_button_lock_timeout_insert_query.c_str());
  if (rc_config_button_lock_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_use_secure_insert_query = "INSERT INTO config VALUES ('use_secure','" + String(DEFAULT_USE_SECURE) + "')";
  int rc_config_use_secure = dbExec(database_instance, config_use_secure_insert_query.c_str());
  if (rc_config_use_secure != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_data_reverse_insert_query = "INSERT INTO config VALUES ('data_reverse','" + String(DEFAULT_DATA_REVERSE) + "')";
  int rc_config_data_reverse = dbExec(database_instance, config_data_reverse_insert_query.c_str());
  if (rc_config_data_reverse != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }
  String config_silent_mode_insert_query = "INSERT INTO config VALUES ('silent_mode','" + String(DEFAULT_SILENT_MODE) + "')";
  int rc_config_silent_mode = dbExec(database_instance, config_silent_mode_insert_query.c_str());
  if (rc_config_silent_mode != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }


            // TODO - for test only
            String cards_insert_test_query_01 = "INSERT INTO cards VALUES ('04C2B401', NULL)";
            int cards_insert_test_01_rc = dbExec(database_instance, cards_insert_test_query_01.c_str());
            if (cards_insert_test_01_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_02 = "INSERT INTO cards VALUES ('A4DCCC1D', NULL)";
            int cards_insert_test_02_rc = dbExec(database_instance, cards_insert_test_query_02.c_str());
            if (cards_insert_test_02_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_03 = "INSERT INTO cards VALUES ('44C20C45', '364A9D')";
            int cards_insert_test_03_rc = dbExec(database_instance, cards_insert_test_query_03.c_str());
            if (cards_insert_test_03_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String config_insert_test_query_01 = "INSERT INTO config VALUES ('backend_server','192.168.0.10')";
            int config_insert_test_query_01_rc = dbExec(database_instance, config_insert_test_query_01.c_str());
            if (config_insert_test_query_01_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String config_insert_test_query_02 = "INSERT INTO config VALUES ('ntp_server','tik.cesnet.cz')";
            int config_insert_test_query_02_rc = dbExec(database_instance, config_insert_test_query_02.c_str());
            if (config_insert_test_query_02_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String config_insert_test_query_03 = "INSERT INTO config VALUES ('time_zone','CET-1CEST,M3.5.0,M10.5.0/3')";
            int config_insert_test_query_03_rc = dbExec(database_instance, config_insert_test_query_03.c_str());
            if (config_insert_test_query_03_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }

            backend_server = "192.168.0.10";
            ntp_server = "tik.cesnet.cz";
            timezone = "CET-1CEST,M3.5.0,M10.5.0/3";


  sqlite3_close(database_instance);
};

String loadConfigStringVariable(String sql, String default_value) { // TODO - how to check when column value is null => set column not null
  sqlite3_stmt *res;
  const char *tail;
  int rc = sqlite3_prepare_v2(fs_db, sql.c_str(), 1000, &res, &tail);
  if (rc != SQLITE_OK) {
    Serial.print("Failed to fetch data: ");
    Serial.println(sqlite3_errmsg(fs_db));
    return default_value;
  }
  while (sqlite3_step(res) == SQLITE_ROW) {
    return (const char *) sqlite3_column_text(res, 0);
  }
  sqlite3_finalize(res);
  return default_value; // failover if no rows
}

int loadConfigIntVariable(String sql, int default_value) { // TODO - how to check when column value is null => set column not null
  sqlite3_stmt *res;
  const char *tail;
  int rc = sqlite3_prepare_v2(fs_db, sql.c_str(), 1000, &res, &tail);
  if (rc != SQLITE_OK) {
    Serial.print("Failed to fetch data: ");
    Serial.println(sqlite3_errmsg(fs_db));
    return default_value;
  }
  while (sqlite3_step(res) == SQLITE_ROW) {
    return sqlite3_column_int(res, 0);
  }
  sqlite3_finalize(res);
  return default_value; // failover if no rows
}

void loadConfig() {
  if (openDb("/spiffs/database.db", &fs_db))
    return;
  
  ntp_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'ntp_server'", DEFAULT_NTP_SERVER);
  timezone = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'time_zone'", DEFAULT_TIMEZONE);
  backend_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'backend_server'", DEFAULT_BACKEND_SERVER);
  backend_port = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'backend_port'", DEFAULT_BACKEND_PORT);
  lock_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'lock_timeout'", DEFAULT_LOCK_TIMEOUT);
  door_sensor = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'door_sensor'", DEFAULT_DOOR_SENSOR);
  button_lock_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'button_lock_timeout'", DEFAULT_BUTTON_LOCK_TIMEOUT);
  use_secure = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'use_secure'", DEFAULT_USE_SECURE);
  data_reverse = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'data_reverse'", DEFAULT_DATA_REVERSE);
  silent_mode = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'silent_mode'", DEFAULT_SILENT_MODE);

  sqlite3_close(fs_db);
}

void setConfigValue() { // TODO
  // TODO - new config value received from backend server
  // TODO - set value into database
  // TODO - set value in runtime
}

int getLogsNumber(String initiator) {
  String count_sql = "SELECT count(*) FROM events_log WHERE initiator = '" + initiator + "'";
  sqlite3_stmt *res_count;
  const char *tail_count;
  int records_count = 0;
  int rc_count = sqlite3_prepare_v2(fs_db, count_sql.c_str(), 1000, &res_count, &tail_count);
  if (rc_count != SQLITE_OK) {
    Serial.print("Failed to fetch data: ");
    Serial.println(sqlite3_errmsg(fs_db));
    return -1;
  }

  while (sqlite3_step(res_count) == SQLITE_ROW) {
    records_count = sqlite3_column_int(res_count, 0);
  }
  sqlite3_finalize(res_count);

  return records_count;
}

void findLogs() { // TODO
  if (openDb("/spiffs/database.db", &fs_db))
    return;

  int reader_logs_count = getLogsNumber("reader");

  if (reader_logs_count == -1) {
    Serial.println("[DB] Failed to fetch number of reader events in events_log table");
  } else if (reader_logs_count == 0) {
    Serial.println("[DB] No reader events in events_log table");
  } else {
    Serial.print("[DB] Number of reader events in events_log table: ");
    Serial.println(reader_logs_count);
  }

  int button_logs_count = getLogsNumber("button");

  if (button_logs_count == -1) {
    Serial.println("[DB] Failed to fetch number of button events in events_log table");
  } else if (button_logs_count == 0) {
    Serial.println("[DB] No button events in events_log table");
  } else {
    Serial.print("[DB] Number of button events in events_log table: ");
    Serial.println(button_logs_count);    
  }

  int door_logs_count = getLogsNumber("door");

  if (door_logs_count == -1) {
    Serial.println("[DB] Failed to fetch number of door events in events_log table");
  } else if (door_logs_count == 0) {
    Serial.println("[DB] No door events in events_log table");
  } else {
    Serial.print("[DB] Number of door events in events_log table: ");
    Serial.println(door_logs_count);   
  }

  sqlite3_close(fs_db);
}

void findCard(String hex_uid, String reader_id, String timestamp, String lock_id) { // TODO
  if (openDb("/spiffs/database.db", &fs_db))
    return;

  // String count_sql = "SELECT count(*) FROM cards_" + lock_id + " WHERE is_secure = 0 AND card_hex = '" + hex_uid + "'";
  // String count_sql = "SELECT count(*) FROM cards WHERE is_secure = 0 AND card_hex = '" + hex_uid + "'";
  String count_sql = "SELECT count(*) FROM cards WHERE card_hex = '" + hex_uid + "'";
  if(use_secure) {
    // count_sql = "SELECT count(*) FROM cards_" + lock_id + " WHERE is_secure = 1 AND card_hex = '" + hex_uid + "'";
    // count_sql = "SELECT count(*) FROM cards WHERE is_secure = 1 AND card_hex = '" + hex_uid + "'";
    count_sql = "SELECT count(*) FROM cards WHERE secure_hex = '" + hex_uid + "'";
  }
  
  sqlite3_stmt *res_count;
  const char *tail_count;
  int records_count = 0;
  int rc_count = sqlite3_prepare_v2(fs_db, count_sql.c_str(), 1000, &res_count, &tail_count);
  if (rc_count != SQLITE_OK) {
    Serial.print("Failed to fetch data: ");
    Serial.println(sqlite3_errmsg(fs_db));
    return;
  }

  while (sqlite3_step(res_count) == SQLITE_ROW) {
    records_count = sqlite3_column_int(res_count, 0);
  }
  sqlite3_finalize(res_count);

  if (records_count > 0) {

    // TODO - here look for restrictions
    // TODO .
    // TODO .
    // TODO .

    readerSuccessStatus(/*reader_id*/);
    String success_log_sql = "INSERT INTO events_log VALUES ('reader', '" + hex_uid + "', '" + timestamp + "', '" + mac_address_nag + "', '" + reader_id + "', '" + lock_id + "', 1, 0)";
    // ? maybe put logging here - no idea how long the logging will run
    readerBuzzerOff(/*reader_id*/);
    dbInsertCardLog(success_log_sql);
  } else {
    readerErrorStatus(/*reader_id*/);
    String error_log_sql = "INSERT INTO events_log VALUES ('reader', '" + hex_uid + "', '" + timestamp + "', '" + mac_address_nag + "', '" + reader_id + "', '" + lock_id + "', 0, 1)";
    // ? maybe put logging here - no idea how long the logging will run
    readerBuzzerOff(/*reader_id*/);
    dbInsertCardLog(error_log_sql);
      // error codes
      // 1 - card is not in database - not permitted
      // 2 - card restrcitons in place - not permitted
  }

  sqlite3_close(fs_db);
}

// void dbUpgrade() { // TODO - handle firmware update database migrations here
//   String count_sql = "SELECT count(*) FROM config WHERE variable = 'fw_version'";
//   sqlite3_stmt *res_count;
//   const char *tail_count;
//   int records_count = 0;
//   int rc_count = sqlite3_prepare_v2(fs_db, count_sql.c_str(), 1000, &res_count, &tail_count);
//   if (rc_count != SQLITE_OK) {
//     Serial.print("Failed to fetch data: ");
//     Serial.println(sqlite3_errmsg(fs_db));
//     return;
//   }
//   while (sqlite3_step(res_count) == SQLITE_ROW) {
//     records_count = sqlite3_column_int(res_count, 0);
//   }
//   sqlite3_finalize(res_count);
//   if (records_count > 0) {
//     String db_version = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'fw_version'", MODULE_FIRMWARE_VERSION);
//     if (strcmp(MODULE_FIRMWARE_VERSION, db_version.c_str()) != 0) {
//       // TODO - upgrade database
//       Serial.println("[FS] Database Structure Updated");
//     } else {
//       Serial.println("[FS] Database Structure Up To Date");
//     }
//   }
// }

void setupFileSystem() {
  // if (!SPIFFS.begin(FORMAT_FS_IF_FAILED, "/spiffs", 10)) {
  if (!SPIFFS.begin(FORMAT_FS_IF_FAILED)) {
    Serial.println("[FS] File System Mount Failed");
    while(1);
  } else {
    Serial.println("[FS] File System Mounted");
    if (SPIFFS.exists("/database.db")) {
      Serial.println("[FS] Database File Exists");
      // dbUpgrade();
      loadConfig();
      Serial.println("[FS] Config Loaded From Database File");
    } else {
      Serial.println("[FS] Database File Does Not Exist");
      File dbFile;
      dbFile = SPIFFS.open("/database.db", FILE_WRITE);
      dbFile.close();
      Serial.println("[FS] Database File Created");
      initDatabaseFile("/spiffs/database.db", fs_db);
      Serial.println("[FS] Database Structure Written Into Database File");
    }
  }
}

void handleEthernetStart() {
  String mac_address = ETH.macAddress();
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  String hostname = MODULE_TYPE + '-' + mac_address;
  Serial.println("[ETH] Started");
  ETH.setHostname(hostname.c_str());
  eth_hostname = hostname;
  mac_address_nag = mac_address;

  if (MDNS.begin(hostname.c_str())) {
    Serial.println("[MDNS] Started" );
  } else {
    Serial.println("[MDNS] Error" );
  }

  if (NBNS.begin(hostname.c_str())) {
    Serial.println("[NBNS] Started" );
  } else {
    Serial.println("[NBNS] Error" );
  }
}

void handleEthernetConnect() {
  Serial.println("[ETH] Connected");
}

void handleEthernetDHCP() { // TODO
  Serial.print("[ETH] DHCP - MAC: ");
  Serial.print(ETH.macAddress());
  Serial.print(" | IPv4: ");
  Serial.print(ETH.localIP());
  if (ETH.fullDuplex()) {
    Serial.print(" | FULL_DUPLEX");
  }
  Serial.print(" | ");
  Serial.print(ETH.linkSpeed());
  Serial.println("Mbps");

  eth_connected = true;
  eth_ip_address = ETH.localIP();


  // TODO

  // check in DB for setup_done == true
  // true
    updateNtpTime(); // update time from backend

    // check if local logs exist
    // yes
      // send local logs to server
    // compare local database to server for this unit
    // sync changes to database
  // false
    // create http rest endpoint to enable setup this unit variables for backend server
    // set setup_done == true in DB
    // restart this unit
}

void handleEthernetDisconnect() {
  Serial.println("[ETH] Disconnected");
  eth_connected = false;
}

void handleEthernetStop() {
  Serial.println("[ETH] Stopped");
  eth_connected = false;
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      handleEthernetStart();
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      handleEthernetConnect();
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      handleEthernetDHCP();
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      handleEthernetDisconnect();
      break;
    case ARDUINO_EVENT_ETH_STOP:
      handleEthernetStop();
      break;
    default:
      break;
  }
}

void setupEthernet() {
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ESP_ETH_PHY_ADDR, ESP_ETH_PHY_POWER, ESP_ETH_PHY_MDC, ESP_ETH_PHY_MDIO, ESP_ETH_PHY_TYPE, ESP_ETH_CLK_MODE);
}

void wiegandReaderStateChanged(bool plugged, const char* reader_ID) {
  if (plugged) {
    wg_connected = true;
    digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
    digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
    Serial.println("[WG] Connected");
  } else {
    wg_connected = false;
    digitalWrite(ESP_WG_GREEN_LED, LOW); // ON
    digitalWrite(ESP_WG_RED_LED, LOW); // ON
    Serial.println("[WG] Disconnected");
  }
}

void wiegandReaderReceivedDataError(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* reader_ID) {
  readerPanicStatus(/*reader_ID*/);

  Serial.print(Wiegand::DataErrorStr(error)); // TODO
  Serial.print(" - Raw data: ");
  Serial.print(rawBits);
  Serial.print("bits / ");
  uint8_t bytes = (rawBits+7)/8;
  for (int i=0; i<bytes; i++) {
      Serial.print(rawData[i] >> 4, 16);
      Serial.print(rawData[i] & 0xF, 16);
  }
  Serial.println();
}

void wiegandReaderReceivedData(uint8_t* data, uint8_t bits, const char* reader_ID) {
  readerBuzzerOn(/*reader_ID*/);

  String timestamp = rtc.now().timestamp();
  String incomingData;
  uint8_t bytes = (bits+7)/8;

  if (data_reverse) {
    for (int i=bytes; i>0; i--) {
      incomingData.concat(String(data[i-1] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
      incomingData.concat(String(data[i-1] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
    }
  } else {
    for (int i=0; i<bytes; i++) {
      incomingData.concat(String(data[i] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
      incomingData.concat(String(data[i] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
    }
  }

  incomingData.toUpperCase();

  findCard(incomingData, reader_ID, timestamp, default_lock_id);
}

void setupWiegandReader() {
  wg.onReceive(wiegandReaderReceivedData, "01");
  wg.onReceiveError(wiegandReaderReceivedDataError, "01");
  wg.onStateChange(wiegandReaderStateChanged, "01");
  wg.begin(Wiegand::LENGTH_ANY, true);
  Serial.println("[WG] Started");
}

void wiegandReaderHandler() {
  wg.flush(); // Checks for pending messages
  wg.setPin0State(digitalRead(ESP_WG_D0)); // Check for changes on the the wiegand input pins
  wg.setPin1State(digitalRead(ESP_WG_D1));
}

void wiegandReaderStateHandler() {
  if (wg_success && (millis() - wg_last_time >= lock_timeout)) {
    digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
    digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
    wg_success = false;
    wg_halt = false;
    button_halt = false;
  }
  if (wg_error && (millis() - wg_last_time >= wg_error_timeout)) {
    digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
    wg_error = false;
    wg_halt = false;
  }
  if (wg_panic && (millis() - wg_last_time >= wg_panic_timeout)) {
    digitalWrite(ESP_WG_RED_LED, HIGH); // OFF
    digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
    wg_panic = false;
    wg_halt = false;
  }
}

void doorSensorChangeStateHandler() {
  int door_sensor_current_state = digitalRead(ESP_DOOR_SENSOR);
  if (door_sensor_current_state != door_sensor_last_state) {
    if (millis() - door_sensor_last_time >= door_sensor_debounce_time) {
      if (door_sensor_current_state == LOW) {
        dbInsertDoorLog(2); // Opened
      }
      if (door_sensor_current_state == HIGH) {
        dbInsertDoorLog(1); // Closed
      }
      door_sensor_last_state = door_sensor_current_state;
      door_sensor_last_time = millis();
    }
  }
}

void buttonPressStateHandler() {
  int button_current_state = digitalRead(ESP_BUTTON);
  if (button_current_state != button_last_state) {
    if (millis() - button_press_last_time >= button_debounce_time) {
      if (button_current_state == LOW) {
        buttonSuccessStatus();
        dbInsertButtonLog();
      }
      button_last_state = button_current_state;
      button_press_last_time = millis();
    }
  }
}

void buttonStateHandler() {
  if (button_success && (millis() - button_last_time >= button_lock_timeout)) {
    digitalWrite(ESP_LOCK_TRIGGER, HIGH); // OFF
    if (wg_connected) digitalWrite(ESP_WG_GREEN_LED, HIGH); // OFF
    button_success = false;
    button_halt = false;
    wg_halt = false;
  }
}

// void someTask(void * parameters) {
//   for( ;; ) {
//     vTaskDelay(100/portTICK_PERIOD_MS);
//   }
// }

// void core0Task(void * parameters) {
//   Serial.print("[Core0] Task is running on core: ");
//   Serial.println(xPortGetCoreID());
//   for( ;; ) {
//     // vTaskDelay(100/portTICK_PERIOD_MS);
//   }
// }

// void core1Task(void * parameters) {
//   Serial.print("[Core1] Task is running on core: ");
//   Serial.println(xPortGetCoreID());
//   for( ;; ) {
//     // vTaskDelay(100/portTICK_PERIOD_MS);
//   }
// }

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("----------------------------------------------------------------------------------------------------------------");
  Serial.println("[Module Unit Name] " + String(MODULE_NAME) + " (" + String(MODULE_TYPE) + ")");
  Serial.println("[Hardware Version] " + String(MODULE_HARDWARE_VERSION));
  Serial.println("[Firmware Version] " + String(MODULE_FIRMWARE_VERSION));
  Serial.println("----------------------------------------------------------------------------------------------------------------");
  // ----------------------------------------------------------------------------------------------------------------

        SPIFFS.format();

  setupEspPins();
  Wire.setPins(ESP_SDA, ESP_SCL);
  setupRtc();
  sqlite3_initialize();
  setupFileSystem();
  setupEthernet();
  // setupWebsockets(); // TODO
  // ----------------------------------------------------------------------------------------------------------------
  digitalWrite(ESP_TXD_LED, HIGH); // OFF
  digitalWrite(ESP_RXD_LED, LOW); // ON
  delay(500);
  digitalWrite(ESP_RXD_LED, HIGH); // OFF
  digitalWrite(ESP_TXD_LED, LOW); // ON
  delay(500);
  digitalWrite(ESP_TXD_LED, HIGH); // OFF
  digitalWrite(ESP_RXD_LED, LOW); // ON
  delay(500);
  digitalWrite(ESP_RXD_LED, HIGH); // OFF
  digitalWrite(ESP_TXD_LED, LOW); // ON
  delay(500);
  digitalWrite(ESP_TXD_LED, HIGH); // OFF
  digitalWrite(ESP_RXD_LED, LOW); // ON
  delay(500);
  digitalWrite(ESP_TXD_LED, LOW); // ON
  // ----------------------------------------------------------------------------------------------------------------
  delay(27500);
  setupWiegandReader();
  Serial.println("[BOOT] Completed");
  Serial.println("----------------------------------------------------------------------------------------------------------------");

  // Testing zone

  findLogs();

          // Serial.println("================================================================================================================");
          // Serial.print("eth_hostname: ");
          // Serial.println(eth_hostname);
          // Serial.print("mac_address_nag: ");
          // Serial.println(mac_address_nag);
          // Serial.print("mac_address_raw: ");
          // Serial.println(mac_address_raw);
          // Serial.print("eth_ip_address: ");
          // Serial.println(eth_ip_address);
          // Serial.print("ntp_server: ");
          // Serial.println(ntp_server);
          // Serial.print("time_zone: ");
          // Serial.println(timezone);
          // Serial.print("backend_server: ");
          // Serial.println(backend_server);
          // Serial.print("lock_timeout: ");
          // Serial.println(lock_timeout);
          // Serial.print("use_secure: ");
          // Serial.println(use_secure);
          // Serial.print("data_reverse: ");
          // Serial.println(data_reverse);
          // Serial.print("silent_mode: ");
          // Serial.println(silent_mode);
          // Serial.println("================================================================================================================");

  // xTaskCreatePinnedToCore(someTask, "SomeTask", 10000, NULL, tskIDLE_PRIORITY, NULL, 0); // TODO - can run other things on core 0 with some delay otherwise watchdog fail
  // xTaskCreatePinnedToCore(core0Task, "core0Task", 10000, NULL, tskIDLE_PRIORITY, NULL, 0);
  // xTaskCreatePinnedToCore(core1Task, "core1Task", 10000, NULL, 10, NULL, 1);
}

void loop() {

  // socketIO.loop(); // TODO

  if (!wg_connected || (!wg_halt && wg_connected)) wiegandReaderHandler();
  if (wg_halt && wg_connected) wiegandReaderStateHandler();

  if (!button_halt) buttonPressStateHandler();
  if (button_halt) buttonStateHandler();

  if (door_sensor) doorSensorChangeStateHandler();
}
