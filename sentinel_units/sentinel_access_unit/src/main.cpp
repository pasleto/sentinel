#include <Arduino.h> // platformio Arduino framework library
#include <ETH.h> // ethernet library
#include <Wiegand.h> // wiegand reader library
#include <Adafruit_MCP23X17.h> // mcp23017 library
#include <Wire.h> // i2c library
#include <SPI.h> // spi library
// #include <stdio.h>
// #include <stdlib.h>
#include <sqlite3.h> //sqlite library
#include <FS.h>
#include <SD.h>
// #include <SPIFFS.h>
// #include <LittleFS.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <RTClib.h>

#include "config.h" // project configuration file

// --------------------------------------------------

Wiegand wg01;
Wiegand wg02;
// Wiegand wg03;
// Wiegand wg04;
RTC_DS3231 rtc;
Adafruit_MCP23X17 mcp01;
Adafruit_MCP23X17 mcp02;
// Adafruit_MCP23X17 mcp03;
sqlite3 *sd_db;
// sqlite3 *spiffs_db; // TODO - get rid off sd card and migrate to spiffs


IPAddress backend_server_ip = IPAddress(195, 113, 144, 201); // tik.cesnet.cz // TODO - load from config

const String backend_server = "xxx"; // TODO - load from config
const String ntp_server = "tik.cesnet.cz"; // TODO - load from config
const String timezone = "CET-1CEST,M3.5.0,M10.5.0/3"; // TODO - load from config
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h


// maybe static ?
bool eth_connected = false;
String eth_hostname;
String mac_address_nag;
String mac_address_raw;
IPAddress eth_ip_address;
bool wg01_should_reverse_order = false;
bool wg02_should_reverse_order = false;
bool wg03_should_reverse_order = false;
bool wg04_should_reverse_order = false;
// bool wg01_is_connected = false;
// bool wg02_is_connected = false;
// bool wg03_is_connected = false;
// bool wg04_is_connected = false;


// static int wg01_Lock_Timeout; // read dip01 switches values
// static int wg01_Lock_Timeout; // read dip02 switches values
// static bool wg01_Power = false; // for wg01 shutdown/restart mode
// static bool wg01_Hold = false; // for wg01 hold mode
// static bool wg02_Power = false; // for wg02 shutdown/restart mode
// static bool wg02_Hold = false; // for wg02 hold mode

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

void setupRtc() {
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

void setupMcp01() {
  if (!mcp01.begin_I2C(MCP01_ADDR)) {
    Serial.println("[MCP01] Error");
    while (1);
  }

  mcp01.pinMode(MCP01_WG01_POWER, OUTPUT);
  mcp01.pinMode(MCP01_WG01_GREEN_LED, OUTPUT);
  mcp01.pinMode(MCP01_WG01_RED_LED, OUTPUT);
  mcp01.pinMode(MCP01_WG01_HOLD, OUTPUT);
  mcp01.pinMode(MCP01_WG01_BUZZER, OUTPUT);
  // mcp01.pinMode(MCP01_WG01_TAMPER, INPUT);
  mcp01.pinMode(MCP01_LOCK01, OUTPUT);
  mcp01.pinMode(MCP01_BUTTON01, INPUT_PULLDOWN);
  mcp01.pinMode(MCP01_WG02_POWER, OUTPUT);
  mcp01.pinMode(MCP01_WG02_GREEN_LED, OUTPUT);
  mcp01.pinMode(MCP01_WG02_RED_LED, OUTPUT);
  mcp01.pinMode(MCP01_WG02_HOLD, OUTPUT);
  mcp01.pinMode(MCP01_WG02_BUZZER, OUTPUT);
  // mcp01.pinMode(MCP01_WG02_TAMPER, INPUT);
  mcp01.pinMode(MCP01_LOCK02, OUTPUT);
  mcp01.pinMode(MCP01_BUTTON02, INPUT_PULLDOWN);

  mcp01.digitalWrite(MCP01_WG01_POWER, LOW);
  mcp01.digitalWrite(MCP01_WG01_GREEN_LED, LOW);
  mcp01.digitalWrite(MCP01_WG01_RED_LED, LOW);
  mcp01.digitalWrite(MCP01_WG01_HOLD, LOW);
  mcp01.digitalWrite(MCP01_WG01_BUZZER, LOW);
  mcp01.digitalWrite(MCP01_LOCK01, LOW);
  mcp01.digitalWrite(MCP01_WG02_POWER, LOW);
  mcp01.digitalWrite(MCP01_WG02_GREEN_LED, LOW);
  mcp01.digitalWrite(MCP01_WG02_RED_LED, LOW);
  mcp01.digitalWrite(MCP01_WG02_HOLD, LOW);
  mcp01.digitalWrite(MCP01_WG02_BUZZER, LOW);
  mcp01.digitalWrite(MCP01_LOCK02, LOW);

  Serial.println("[MCP01] Success");
}

void setupMcp02() {
  if (!mcp02.begin_I2C(MCP02_ADDR)) {
    Serial.println("[MCP02] Error");
    while (1);
  }

  mcp02.pinMode(MCP02_STATUS_LED01_GREEN, OUTPUT);
  mcp02.pinMode(MCP02_STATUS_LED01_RED, OUTPUT);
  mcp02.pinMode(MCP02_STATUS_LED02_GREEN, OUTPUT);
  mcp02.pinMode(MCP02_STATUS_LED02_RED, OUTPUT);
  mcp02.pinMode(MCP02_WG01_DATA_REVERSE, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_WG02_DATA_REVERSE, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_WG03_DATA_REVERSE, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_WG04_DATA_REVERSE, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_DIP02_2, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_DIP02_3, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_DIP03_1, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_DIP03_2, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_DIP03_3, INPUT_PULLDOWN);
  mcp02.pinMode(MCP02_RF_DATA, INPUT); // TODO - check
  mcp02.pinMode(MCP02_RF_LEARN_BTN, INPUT_PULLDOWN); // TODO - check
  mcp02.pinMode(MCP02_RF_DELETE_BTN, INPUT_PULLDOWN); // TODO - check

  mcp02.digitalWrite(MCP02_STATUS_LED01_GREEN, LOW);
  mcp02.digitalWrite(MCP02_STATUS_LED01_RED, LOW);
  mcp02.digitalWrite(MCP02_STATUS_LED02_GREEN, LOW);
  mcp02.digitalWrite(MCP02_STATUS_LED02_RED, LOW);

  Serial.println("[MCP02] Success");
}

// TODO
// void setupMcp03() {
//   if (!mcp03.begin_I2C(MCP03_ADDR)) {
//     Serial.println("[MCP03] Error");
//     while (1);
//   }
//   Serial.println("[MCP03] Success");
// }

void setupDipSwitches() {
  if (mcp02.digitalRead(MCP02_WG01_DATA_REVERSE) == HIGH) {
    wg01_should_reverse_order = true;
  } else {
    wg01_should_reverse_order = false;
  }

  if (mcp02.digitalRead(MCP02_WG02_DATA_REVERSE) == HIGH) {
    wg02_should_reverse_order = true;
  } else {
    wg02_should_reverse_order = false;
  }

  if (mcp02.digitalRead(MCP02_WG03_DATA_REVERSE) == HIGH) {
    wg03_should_reverse_order = true;
  } else {
    wg03_should_reverse_order = false;
  }

  if (mcp02.digitalRead(MCP02_WG04_DATA_REVERSE) == HIGH) {
    wg04_should_reverse_order = true;
  } else {
    wg04_should_reverse_order = false;
  }
}

void handleEthernetStart() {
  String mac_address = ETH.macAddress();
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  String hostname = "SENTINEL-ACCESS-" + mac_address;
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

// TODO
void handleEthernetDHCP() {
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
  // mac_address_raw = ETH.macAddress();

  // check if backend server is available
    // yes
      // refresh time from NTP server
      updateNtpTime();

      // check if local logs exist
        // yes
          // send local logs to server database
        // no
          // continue
      // compare local database to server database for this node
        // is changed
          // retrieve updated database from server
        // is unchanged
          // continue
    // no
      // continue
}

void handleEthernetDisconnect() {
  Serial.println("[ETH] Disconnected");
  eth_connected = false;
}

void handleEthernetStop() {
  Serial.println("[ETH] Stopped");
  eth_connected = false;
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      handleEthernetStart();
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      handleEthernetConnect();
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      handleEthernetDHCP();
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      handleEthernetDisconnect();
      break;
    case SYSTEM_EVENT_ETH_STOP:
      handleEthernetStop();
      break;
    default:
      break;
  }
}

void setupEthernet() {
  WiFi.onEvent(WiFiEvent);
  ETH.begin(ESP_ETH_PHY_ADDR, ESP_ETH_PHY_POWER, ESP_ETH_PHY_MDC, ESP_ETH_PHY_MDIO, ESP_ETH_PHY_TYPE, ESP_ETH_CLK_MODE); // ETH.begin();
}

int openDb(const char *filename, sqlite3 **db) {
  int rc = sqlite3_open(filename, db);
  if (rc) {
    Serial.printf("[DB] Can't open database: %s\n", sqlite3_errmsg(*db));
    return rc;
  } 
  return rc;
}

static int dbCallback(void *data, int dataLength, char **colValue, char **colName){
  Serial.printf("     %s: ", (const char*)data);
  Serial.println();

  Serial.print("     Response Length: "); // TODO - test
  Serial.println(dataLength);

  for (int i = 0; i<dataLength; i++){
    Serial.printf("     %s = %s\n", colName[i], colValue[i] ? colValue[i] : "NULL");
  }
  return 0;
}

int dbExec(sqlite3 *db, const char *sql, char* data) {
  char *dbErrorMessage = 0;

  Serial.print("     "); // TODO - test
  Serial.println(sql);

  int rc = sqlite3_exec(db, sql, dbCallback, (void*)data, &dbErrorMessage);
  if (rc != SQLITE_OK) {
    Serial.printf("     SQL error: %s\n", dbErrorMessage);
    sqlite3_free(dbErrorMessage);
  } 
  return rc;
}

void dbExecQuery(String query) {
  if (openDb("/sd/database.db", &sd_db))
    return;
  int rc = dbExec(sd_db, query.c_str(),(char*)"Executing query");
  if (rc != SQLITE_OK) {
      sqlite3_close(sd_db);
      return;
  }
  sqlite3_close(sd_db);
}

String getStringDelimiterValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

static int dbCardDataLookupCallback(void *data, int dataLength, char **colValue, char **colName){
  String passed_data = (const char*)data; // hex;reader;timestamp
  String card_hex = getStringDelimiterValue(passed_data, ';', 0);
  String reader_id = getStringDelimiterValue(passed_data, ';', 1);
  String timestamp = getStringDelimiterValue(passed_data, ';', 2);
  String lock_id = getStringDelimiterValue(passed_data, ';', 3);

  // TODO - check for restrictions
  Serial.print("     ");
  Serial.println(passed_data);
  Serial.print("     ");
  Serial.print(card_hex);
  Serial.print(" | ");
  Serial.print(reader_id);
  Serial.print(" | ");
  Serial.print(timestamp);
  Serial.print(" | ");
  Serial.println(lock_id);

  for (int i = 0; i<dataLength; i++){
    Serial.printf("     %s = %s\n", colName[i], colValue[i] ? colValue[i] : "NULL");
  }

  // CREATE TABLE IF NOT EXISTS cards (card_hex TEXT, has_days_restrictions INTEGER, allowed_days TEXT, has_time_restrictions INTEGER, start_time TEXT, end_time TEXT, has_validity INTEGER, valid_until TEXT)
  // 0 - card_hex
  Serial.printf("     %s\n", colValue[0] ? colValue[0] : "NULL");
  // 1 - has_days_restrictions
  // Serial.printf("     %s\n", colValue[1] ? colValue[1] : "NULL");
  // // 2 - allowed_days
  // Serial.printf("     %s\n", colValue[2] ? colValue[2] : "NULL");
  // // 3 - has_time_restrictions
  // Serial.printf("     %s\n", colValue[3] ? colValue[3] : "NULL");
  // // 4 - start_time
  // Serial.printf("     %s\n", colValue[4] ? colValue[4] : "NULL");
  // // 5 - end_time
  // Serial.printf("     %s\n", colValue[5] ? colValue[5] : "NULL");
  // // 6 - has_validity
  // Serial.printf("     %s\n", colValue[6] ? colValue[6] : "NULL");
  // // 7 - valid_until
  // Serial.printf("     %s\n", colValue[7] ? colValue[7] : "NULL");

  // TODO

  return 0;
}

int dbCardDataLookupExec(sqlite3 *db, const char *sql, char* data) {
  char *dbErrorMessage = 0;
  int rc = sqlite3_exec(db, sql, dbCardDataLookupCallback, (void*)data, &dbErrorMessage);
  if (rc != SQLITE_OK) {
    Serial.printf("     SQL error: %s\n", dbErrorMessage);
    sqlite3_free(dbErrorMessage);
  }
  return rc;
}

void lookupCardData(String sql, String data) {
  // if (openDb("/sd/database.db", &sd_db))
  //   return;
  int rc = dbCardDataLookupExec(sd_db, sql.c_str(),(char*)data.c_str());
  if (rc != SQLITE_OK) {
      sqlite3_close(sd_db);
      return;
  }
  // sqlite3_close(sd_db);
}

void dbInsertLog(String sql, String data) {
  // if (openDb("/sd/database.db", &sd_db))
  //   return;
  int rc = dbExec(sd_db, sql.c_str(), (char*)data.c_str());
  if (rc != SQLITE_OK) {
    sqlite3_close(sd_db);
    return;
  }
  // sqlite3_close(sd_db);
}

static int dbCardLookupCallback(void *data, int dataLength, char **colValue, char **colName){
  String passed_data = (const char*)data;
  String card_hex = getStringDelimiterValue(passed_data, ';', 0);
  String reader_id = getStringDelimiterValue(passed_data, ';', 1);
  String timestamp = getStringDelimiterValue(passed_data, ';', 2);
  String lock_id = getStringDelimiterValue(passed_data, ';', 3);
  long count = atol(colValue[0]);

  if (count > 0) {
    Serial.print("     Card IS in table (cards_");
    Serial.print(lock_id);
    Serial.print("): ");
    Serial.println(count);
    // TODO

    // call another set of db functions to list wanted card and its parameters to check for restrictions
    // String card_data_sql = "SELECT * FROM cards_" + lock_id + " WHERE card_hex = '" + card_hex + "'";
    // lookupCardData(card_data_sql, passed_data);

    // call function to open door lock
    // check if backend server is available
      // yes
        // send log to server database
      // no
        // send log to local database
        String success_query = "INSERT INTO cards_log VALUES ('" + card_hex + "', '" + timestamp + "', '" + mac_address_nag + "', '" + reader_id + "', '" + lock_id + "', 1, 0)"; // cards_log (card_hex TEXT, timestamp TEXT, unit_id TEXT, reader_id TEXT, lock_id TEXT, is_success INTEGER, error_code INT)
        dbInsertLog(success_query, "Logging Entry Success");

  } else {
    Serial.print("     Card IS NOT in table (cards_");
    Serial.print(lock_id);
    Serial.println(")!");

    // deny access

    // TODO
    // check if backend server is available
      // yes
        // send log to server database
      // no
        // send log to local database
        String error_query = "INSERT INTO cards_log VALUES ('" + card_hex + "', '" + timestamp + "', '" + mac_address_nag + "', '" + reader_id + "', '" + lock_id + "', 0, 1)";
        dbInsertLog(error_query, "Logging Entry Error");

        // error codes
        // 1 - card is not in database - not permitted
        // 2 - card restrcitons in place - not permitted
  }

  return 0;
}

int dbCardLookupExec(sqlite3 *db, const char *sql, char* data) {
  char *dbErrorMessage = 0;
  int rc = sqlite3_exec(db, sql, dbCardLookupCallback, (void*)data, &dbErrorMessage);
  if (rc != SQLITE_OK) {
    Serial.printf("     SQL error: %s\n", dbErrorMessage);
    sqlite3_free(dbErrorMessage);
  }
  return rc;
}

// void lookupCard(String hex_uid, String reader_id, String timestamp) {
//   String pass_data = hex_uid + ";" + reader_id + ";" + timestamp;
//   String count = "SELECT count(*) FROM cards WHERE card_hex = '" + hex_uid + "'";
//   if (openDb("/sd/database.db", &sd_db))
//     return;
//   int rc = dbCardLookupExec(sd_db, count.c_str(),(char*)pass_data.c_str());
//   if (rc != SQLITE_OK) {
//     sqlite3_close(sd_db);
//     return;
//   }
//   sqlite3_close(sd_db);
// }

void lookupCard(String hex_uid, String reader_id, String timestamp, String lock_id) { // lock_id -> "01", "02"
  String pass_data = hex_uid + ";" + reader_id + ";" + timestamp + ";" + lock_id;
  String count = "SELECT count(*) FROM cards_" + lock_id + " WHERE card_hex = '" + hex_uid + "'";
  if (openDb("/sd/database.db", &sd_db))
    return;
  int rc = dbCardLookupExec(sd_db, count.c_str(),(char*)pass_data.c_str());
  if (rc != SQLITE_OK) {
    sqlite3_close(sd_db);
    return;
  }
  sqlite3_close(sd_db);
}

// TODO - write database structure
void initDatabaseFile(const char *database_path, sqlite3 *database_instance) {
  if (openDb(database_path, &database_instance))
    return;

  String config_create_query = "CREATE TABLE IF NOT EXISTS config (variable TEXT, value TEXT)"; 
  int rc_config = dbExec(database_instance, config_create_query.c_str(), (char*)"Creating Config Table");
  if (rc_config != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  // TODO - write default config values into config table and use them
  // String config_01_insert_query = "INSERT INTO config VALUES ('ntp_server','pool.ntp.org')";
  // int rc_config_01 = dbExec(database_instance, config_01_insert_query.c_str(), (char*)"Insert ntp_server config value");
  // if (rc_config_01 != SQLITE_OK) {
  //   sqlite3_close(database_instance);
  //   return;
  // }
  // ntp_server = "pool.ntp.org";
  // String config_02_insert_query = "INSERT INTO config VALUES ('time_zone','UTC0')"; // https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  // int rc_config_02 = dbExec(database_instance, config_02_insert_query.c_str(), (char*)"Insert time_zone config value");
  // if (rc_config_02 != SQLITE_OK) {
  //   sqlite3_close(database_instance);
  //   return;
  // }
  // timezone = "UTC0";
  // String config_03_insert_query = "INSERT INTO config VALUES ('backend_server','sentinel-gateway.local')";
  // int rc_config_03 = dbExec(database_instance, config_03_insert_query.c_str(), (char*)"Insert backend_server config value");
  // if (rc_config_03 != SQLITE_OK) {
  //   sqlite3_close(database_instance);
  //   return;
  // }
  // backend_server = "sentinel-gateway.local";

  // TODO - maybe apply restrictions for cards_01 and cards_02 (hours when user can access, days in week when user can access)
  // String cards_create_query = "CREATE TABLE IF NOT EXISTS cards (card_hex TEXT, has_days_restrictions INTEGER, allowed_days TEXT, has_time_restrictions INTEGER, start_time TEXT, end_time TEXT, has_validity INTEGER, valid_until TEXT)"; 
  String cards_01_create_query = "CREATE TABLE IF NOT EXISTS cards_01 (card_hex TEXT)"; 
  int rc_cards_01 = dbExec(database_instance, cards_01_create_query.c_str(), (char*)"Creating Cards_01 Table");
  if (rc_cards_01 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String cards_02_create_query = "CREATE TABLE IF NOT EXISTS cards_02 (card_hex TEXT)"; 
  int rc_cards_02 = dbExec(database_instance, cards_02_create_query.c_str(), (char*)"Creating Cards_02 Table");
  if (rc_cards_02 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  // TODO - for test only
        String cards_01_insert_dummy_query = "INSERT INTO cards_01 VALUES ('04C2B401')";
        int cards_01_insert_dummy_rc = dbExec(database_instance, cards_01_insert_dummy_query.c_str(), (char*)"Test Card 01");
        if (cards_01_insert_dummy_rc != SQLITE_OK) {
          sqlite3_close(database_instance);
          return;
        }
        String cards_02_insert_dummy_query = "INSERT INTO cards_02 VALUES ('A4DCCC1D')";
        int cards_02_insert_dummy_rc = dbExec(database_instance, cards_02_insert_dummy_query.c_str(), (char*)"Test Card 02");
        if (cards_02_insert_dummy_rc != SQLITE_OK) {
          sqlite3_close(database_instance);
          return;
        }

  String cards_log_create_query = "CREATE TABLE IF NOT EXISTS cards_log (card_hex TEXT, timestamp TEXT, unit_id TEXT, reader_id TEXT, lock_id TEXT, is_success INTEGER, error_code INTEGER)";
  int rc_cards_log = dbExec(database_instance, cards_log_create_query.c_str(), (char*)"Creating Log Table");
  if (rc_cards_log != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String rf_01_create_query = "CREATE TABLE IF NOT EXISTS rf_01 (rf_code TEXT)"; 
  int rc_rf_01 = dbExec(database_instance, rf_01_create_query.c_str(), (char*)"Creating Rf_01 Table");
  if (rc_rf_01 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String rf_02_create_query = "CREATE TABLE IF NOT EXISTS rf_02 (rf_code TEXT)"; 
  int rc_rf_02 = dbExec(database_instance, rf_02_create_query.c_str(), (char*)"Creating Rf_02 Table");
  if (rc_rf_02 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String rf_log_create_query = "CREATE TABLE IF NOT EXISTS rf_log (rf_code TEXT, timestamp TEXT, unit_id TEXT, lock_id TEXT, is_success INTEGER, error_code INTEGER)";
  int rc_rf_log = dbExec(database_instance, rf_log_create_query.c_str(), (char*)"Creating Log Table");
  if (rc_rf_log != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  sqlite3_close(database_instance);
};

// TODO - migrate from sd card to spiffs
void setupFileSystem() {

  // if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
  //   Serial.println("[SPIFFS] File System Mount Failed");
  //   // return;
  //   while(1);
  // } else {
  //   Serial.println("[SPIFFS] File System Mounted");
  //   File root = SPIFFS.open("/");
  //   if (!root) {
  //     Serial.println("- failed to open directory");
  //     return;
  //   }
  //   if (!root.isDirectory()) {
  //     Serial.println(" - not a directory");
  //     return;
  //   }
  //   File file = root.openNextFile();
  //   while (file) {
  //     if (file.isDirectory()) {
  //       Serial.print("  DIR : ");
  //       Serial.println(file.name());
  //     } else {
  //       Serial.print("  FILE: ");
  //       Serial.print(file.name());
  //       Serial.print("\tSIZE: ");
  //       Serial.println(file.size());
  //     }
  //     file = root.openNextFile();
  //   }
  // }


  if (!SD.begin(ESP_CS)) {
    Serial.println("[SD] Mount Failed");
    while(1);
  } else {
    Serial.println("[SD] Card Mounted");

    if (SD.exists("/database.db")) { // check if database file exist
      Serial.println("[SD] Database File Exists");

      // TODO - load config from config table
    } else {
      Serial.println("[SD] Database File Does Not Exist");
      Serial.println("     Creating Database File");
      File dbFile;
      dbFile = SD.open("/database.db", FILE_WRITE);
      dbFile.close();
      Serial.println("     Database File Created");
      Serial.println("     Writing Database Structure Into Database File");
      initDatabaseFile("/sd/database.db", sd_db);
      Serial.println("     Database Structure Written Into Database File");
    }
  }
}

// void wiegandStateChanged(bool plugged, const char* readerID) {
//   Serial.print("[");
//   Serial.print(readerID);
//   Serial.print("] - ");
//   Serial.println(plugged ? "Connected" : "Disconnected");
// }

void wiegandReaderReceivedDataError(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* readerID) { // Notifies when an invalid transmission is detected
  String timestamp = rtc.now().timestamp();

  Serial.print("[WG"); // TODO - test
  Serial.print(readerID);
  Serial.print("] ");
  Serial.print("(");
  Serial.print(rawBits);
  Serial.print("bits) - ");
  Serial.print(Wiegand::DataErrorStr(error));
  Serial.print(" - ");

  String incomingData;
  uint8_t bytes = (rawBits+7)/8; //Print value in HEX
  for (int i=0; i<bytes; i++) {
      incomingData.concat(String(rawData[i] >> 4, 16));     // Serial.print(rawData[i] >> 4, 16);
      incomingData.concat(String(rawData[i] & 0xF, 16));    // Serial.print(rawData[i] & 0xF, 16);
  }
  incomingData.toUpperCase();

  Serial.print(timestamp); // TODO - test
  Serial.print(" | ");
  Serial.println(incomingData);                             // Serial.println();

  // TODO - log somewhere ?
}

void wiegandReader01ReceivedData(uint8_t* data, uint8_t bits, const char* lockID) { // Notifies when a card was read. Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
  String timestamp = rtc.now().timestamp();
  String readerID = "01";
  String incomingData;
  uint8_t bytes = (bits+7)/8;

  Serial.print("[WG"); // TODO - test
  Serial.print(readerID);
  Serial.print("] ");
  Serial.print("(");
  Serial.print(bits);
  Serial.print("bits) - ");

  if (wg01_should_reverse_order) {
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

  Serial.print(timestamp); // TODO - test
  Serial.print(" | ");
  Serial.print(incomingData);
  Serial.print(" | ");
  Serial.println(lockID);

  // lookupCard(incomingData, readerID, timestamp);
  lookupCard(incomingData, readerID, timestamp, lockID);
}

void wiegandReader02ReceivedData(uint8_t* data, uint8_t bits, const char* lockID) { // Notifies when a card was read. Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
  String timestamp = rtc.now().timestamp();
  String readerID = "02";
  String incomingData;
  uint8_t bytes = (bits+7)/8;

  Serial.print("[WG"); // TODO - test
  Serial.print(readerID);
  Serial.print("] ");
  Serial.print("(");
  Serial.print(bits);
  Serial.print("bits) - ");

  if (wg02_should_reverse_order) {
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

  Serial.print(timestamp); // TODO - test
  Serial.print(" | ");
  Serial.print(incomingData);
  Serial.print(" | ");
  Serial.println(lockID);

  // lookupCard(incomingData, readerID, timestamp);
  lookupCard(incomingData, readerID, timestamp, lockID);
}

// TODO
// void wiegandReader03ReceivedData(uint8_t* data, uint8_t bits, const char* lockID) { // Notifies when a card was read. Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
//   String timestamp = rtc.now().timestamp();
//   String readerID = "03";
//   String incomingData;
//   uint8_t bytes = (bits+7)/8;
//   if (wg03_should_reverse_order) {
//     for (int i=bytes; i>0; i--) {
//       incomingData.concat(String(data[i-1] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
//       incomingData.concat(String(data[i-1] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
//     }
//   } else {
//     for (int i=0; i<bytes; i++) {
//       incomingData.concat(String(data[i] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
//       incomingData.concat(String(data[i] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
//     }
//   }
//   incomingData.toUpperCase();
//   lookupCard(incomingData, readerID, timestamp, lockID);
// }

// TODO
// void wiegandReader04ReceivedData(uint8_t* data, uint8_t bits, const char* lockID) { // Notifies when a card was read. Instead of a message, the seconds parameter can be anything you want -- Whatever you specify on `wiegand.onReceive()`
//   String timestamp = rtc.now().timestamp();
//   String readerID = "04";
//   String incomingData;
//   uint8_t bytes = (bits+7)/8;
//   if (wg04_should_reverse_order) {
//     for (int i=bytes; i>0; i--) {
//       incomingData.concat(String(data[i-1] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
//       incomingData.concat(String(data[i-1] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
//     }
//   } else {
//     for (int i=0; i<bytes; i++) {
//       incomingData.concat(String(data[i] >> 4, 16));        // Serial.print(data[i] >> 4, 16);
//       incomingData.concat(String(data[i] & 0xF, 16));       // Serial.print(data[i] & 0xF, 16);
//     }
//   }
//   incomingData.toUpperCase();
//   lookupCard(incomingData, readerID, timestamp, lockID);
// }

void setupWiegandReader01() {
  const char* lock_id = "01"; // default
  wg01.onReceive(wiegandReader01ReceivedData, lock_id); //Install listeners and initialize Wiegand reader
  wg01.onReceiveError(wiegandReaderReceivedDataError, "01");
  // wg01.onStateChange(wiegandStateChanged, "01");
  wg01.begin(Wiegand::LENGTH_ANY, true);
  pinMode(ESP_WG01_D0, INPUT);
  pinMode(ESP_WG01_D1, INPUT);
  Serial.println("[WG01] Started");
}

void setupWiegandReader02() {
  const char* lock_id = "02"; // default
  wg02.onReceive(wiegandReader02ReceivedData, lock_id); //Install listeners and initialize Wiegand reader
  wg02.onReceiveError(wiegandReaderReceivedDataError, "02");
  // wg02.onStateChange(wiegandStateChanged, "02");
  wg02.begin(Wiegand::LENGTH_ANY, true);
  pinMode(ESP_WG02_D0, INPUT);
  pinMode(ESP_WG02_D1, INPUT);
  Serial.println("[WG02] Started");
}

// TODO
// void setupWiegandReader03() {
//   const char* lock_id = "01"; // default
//   wg03.onReceive(wiegandReader03ReceivedData, lock_id); //Install listeners and initialize Wiegand reader
//   wg03.onReceiveError(wiegandReaderReceivedDataError, "03");
//   // wg03.onStateChange(wiegandStateChanged, "03");
//   wg03.begin(Wiegand::LENGTH_ANY, true);
//   pinMode(ESP_WG03_D0, INPUT);
//   pinMode(ESP_WG03_D1, INPUT);
//   Serial.println("[WG03] Started");
// }

// TODO
// void setupWiegandReader04() {
//   const char* lock_id = "02"; // default
//   wg04.onReceive(wiegandReader04ReceivedData, lock_id); //Install listeners and initialize Wiegand reader
//   wg04.onReceiveError(wiegandReaderReceivedDataError, "04");
//   // wg04.onStateChange(wiegandStateChanged, "04");
//   wg04.begin(Wiegand::LENGTH_ANY, true);
//   pinMode(ESP_WG04_D0, INPUT);
//   pinMode(ESP_WG04_D1, INPUT);
//   Serial.println("[WG04] Started");
// }

void wiegandReader01Handler() {
  wg01.flush(); // Checks for pending messages
  wg01.setPin0State(digitalRead(ESP_WG01_D0)); // Check for changes on the the wiegand input pins
  wg01.setPin1State(digitalRead(ESP_WG01_D1));
}

void wiegandReader02Handler() {
  wg02.flush(); // Checks for pending messages
  wg02.setPin0State(digitalRead(ESP_WG02_D0)); // Check for changes on the the wiegand input pins
  wg02.setPin1State(digitalRead(ESP_WG02_D1));
}

// TODO
// void wiegandReader03Handler() {
//   wg03.flush(); // Checks for pending messages
//   wg03.setPin0State(digitalRead(ESP_WG03_D0)); // Check for changes on the the wiegand input pins
//   wg03.setPin1State(digitalRead(ESP_WG03_D1));
// }

// TODO
// void wiegandReader04Handler() {
//   wg04.flush(); // Checks for pending messages
//   wg04.setPin0State(digitalRead(ESP_WG04_D0)); // Check for changes on the the wiegand input pins
//   wg04.setPin1State(digitalRead(ESP_WG04_D1));
// }

void setup() {
  Serial.begin(9600);
  delay(5000);
  pinMode(STATUS_BOOT_LED, OUTPUT);
  pinMode(STATUS_SUCCESS_LED, OUTPUT);
  digitalWrite(STATUS_SUCCESS_LED, HIGH);
  digitalWrite(STATUS_BOOT_LED, LOW);
  delay(500);
  digitalWrite(STATUS_BOOT_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_BOOT_LED, LOW);
  delay(500);
  digitalWrite(STATUS_BOOT_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_BOOT_LED, LOW);
  Serial.println();
  Serial.println("------------------------------------------------------------------------------");
  Serial.println("[ " + String(MODULE_NAME) + " | Hardware Version - " + String(MODULE_HW_VERSION) + " | Software Version - " + String(MODULE_SW_VERSION) + " ]");
  Serial.println("------------------------------------------------------------------------------");
  Serial.println();
  // --------------------------------------------------

  Wire.setPins(ESP_SDA, ESP_SCL);
  SPI.begin(ESP_SCLK, ESP_MISO, ESP_MOSI, ESP_CS);

  sqlite3_initialize();
  setupRtc();
  setupFileSystem();
  
  setupMcp01();
  setupMcp02();
  // setupMcp03();
  setupDipSwitches();
  setupWiegandReader01();
  setupWiegandReader02();
  // setupWiegandReader03();
  // setupWiegandReader04();
  setupEthernet();

  // --------------------------------------------------
  delay(5000);
  digitalWrite(STATUS_SUCCESS_LED, LOW);
  delay(500);
  digitalWrite(STATUS_SUCCESS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_SUCCESS_LED, LOW);
  delay(500);
  digitalWrite(STATUS_SUCCESS_LED, HIGH);
  delay(500);
  digitalWrite(STATUS_SUCCESS_LED, LOW);
  Serial.println();
  Serial.println("------------------------------------------------------------------------------");
  Serial.println();


  // Testing zone

          // cards
          // 04C2B401
          // A4DCCC1D
          // 2DDC44A1

          delay(5000);
          Serial.println("==============================================================================");

          // Serial.print("[TEST] Hostname - ");
          // Serial.println(eth_hostname);
          // Serial.print("[TEST] Mac Address Nag - ");
          // Serial.println(mac_address_nag);
          // Serial.print("[TEST] Mac Address - ");
          // Serial.println(mac_address_raw);
          // Serial.print("[TEST] IP Address - ");
          // Serial.println(eth_ip_address);
          // Serial.println("===================================================");

          // Serial.println("===================================================");

          // Serial.println("===================================================");

          // String cards_01_insert_dummy = "04C2B401";
          // String cards_01_insert_dummy_query = "INSERT INTO cards_01 VALUES ('"+cards_01_insert_dummy+"')";
          // dbExecQuery(cards_01_insert_dummy_query);
          // Serial.println("===================================================");
          // String cards_02_insert_dummy = "A4DCCC1D";
          // String cards_02_insert_dummy_query = "INSERT INTO cards_02 VALUES ('"+cards_02_insert_dummy+"')";
          // dbExecQuery(cards_02_insert_dummy_query);
          // Serial.println("===================================================");
}





void testClient(const char * host, uint16_t port) {
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void ethTest() {
  if (eth_connected) {
    testClient("google.com", 80);
  }
  delay(10000);
}





void loop() {

  // ethTest();


  wiegandReader01Handler();
  wiegandReader02Handler();
  // wiegandReader03Handler();
  // wiegandReader04Handler();
}