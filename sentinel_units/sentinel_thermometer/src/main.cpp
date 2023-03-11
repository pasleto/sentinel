#include "configuration.h" // project configuration file
#include <Arduino.h>

#include <Preferences.h>
#include <ETH.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
#include <sqlite3.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <RTClib.h>
#include <Ticker.h>
#include <PubSubClient.h>
#include <WifiClientSecure.h>
// #include <DNSServer.h>

// ----------------------------------------------------------------------------------------------------------------

// ! define YYSTACKDEPTH changed from 20 to 50 inside lib/SqliteDatabase/config_ext.h

// ----------------------------------------------------------------------------------------------------------------
/*

TODO -

*/
// ----------------------------------------------------------------------------------------------------------------

Preferences preferences;
RTC_DS3231 rtc;
sqlite3 *fs_db;
WebServer web_server(80);
HTTPAuthMethod web_server_auth_mode = DIGEST_AUTH/*BASIC_AUTH*/;
WiFiClient eth_client;
WiFiClientSecure eth_client_secure;
PubSubClient mqtt;

      // DNSServer dnsServer; // ?
      // IPAddress default_ip(192,168,11,1); // ?
      // IPAddress default_gw(192,168,11,1); // ?
      // IPAddress default_mask(255,255,255,0); // ?

// http://tomeko.net/online_tools/file_to_hex.php?lang=en
// https://www.onlinehexeditor.com/

const char favicon[] PROGMEM = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x26, 0x94, 0x4E,
0x3A, 0x00, 0x00, 0x01, 0x82, 0x69, 0x43, 0x43, 0x50, 0x73, 0x52, 0x47, 0x42, 0x20, 0x49, 0x45,
0x43, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2D, 0x32, 0x2E, 0x31, 0x00, 0x00, 0x28, 0x91, 0x75, 0x91,
0xB9, 0x4B, 0x43, 0x41, 0x10, 0x87, 0x3F, 0xA3, 0xA2, 0x68, 0x3C, 0xF0, 0x00, 0x0B, 0x8B, 0x20,
0xD1, 0x2A, 0x91, 0x18, 0x21, 0x68, 0x63, 0x11, 0xF1, 0x02, 0xB5, 0x48, 0x22, 0x18, 0xB5, 0x49,
0x5E, 0x2E, 0x21, 0xC7, 0xE3, 0xBD, 0x04, 0x09, 0xB6, 0x82, 0xAD, 0xA0, 0x20, 0xDA, 0x78, 0x15,
0xFA, 0x17, 0x68, 0x2B, 0x58, 0x0B, 0x82, 0xA2, 0x08, 0x62, 0x27, 0x58, 0x2B, 0xDA, 0xA8, 0x3C,
0xE7, 0x25, 0x81, 0x04, 0x31, 0xB3, 0xCC, 0xCE, 0xB7, 0xBF, 0xDD, 0x19, 0x76, 0x67, 0xC1, 0x12,
0x48, 0x2A, 0x29, 0xBD, 0xCE, 0x05, 0xA9, 0x74, 0x56, 0xF3, 0x4D, 0x79, 0x6D, 0x8B, 0xC1, 0x25,
0x5B, 0xC3, 0x0B, 0xCD, 0x74, 0xD3, 0x4A, 0x07, 0xAE, 0x90, 0xA2, 0xAB, 0x73, 0xFE, 0xC9, 0x00,
0x55, 0xED, 0xF3, 0x9E, 0x1A, 0x33, 0xDE, 0x3A, 0xCD, 0x5A, 0xD5, 0xCF, 0xFD, 0x6B, 0xCD, 0x91,
0xA8, 0xAE, 0x40, 0x4D, 0xA3, 0xF0, 0x98, 0xA2, 0x6A, 0x59, 0xE1, 0x69, 0xE1, 0xD9, 0xB5, 0xAC,
0x6A, 0xF2, 0x8E, 0x70, 0x97, 0x92, 0x08, 0x45, 0x84, 0xCF, 0x84, 0x1D, 0x9A, 0x5C, 0x50, 0xF8,
0xCE, 0xD4, 0xC3, 0x45, 0x7E, 0x35, 0x39, 0x5E, 0xE4, 0x6F, 0x93, 0xB5, 0x80, 0x6F, 0x1C, 0x2C,
0xED, 0xC2, 0xB6, 0x78, 0x05, 0x87, 0x2B, 0x58, 0x49, 0x68, 0x29, 0x61, 0x79, 0x39, 0xF6, 0x54,
0x32, 0xA7, 0x94, 0xEE, 0x63, 0xBE, 0xC4, 0x1A, 0x4D, 0x2F, 0xF8, 0x25, 0xF6, 0x89, 0xF7, 0xA2,
0xE3, 0x63, 0x0A, 0x2F, 0x36, 0x66, 0x98, 0x60, 0x1C, 0x0F, 0x43, 0x8C, 0xCA, 0xEC, 0xC1, 0x89,
0x9B, 0x41, 0x59, 0x51, 0x25, 0xDF, 0x55, 0xC8, 0x9F, 0x27, 0x23, 0xB9, 0x8A, 0xCC, 0x2A, 0x79,
0x34, 0x56, 0x89, 0x93, 0x20, 0x8B, 0x43, 0xD4, 0x9C, 0x54, 0x8F, 0x4A, 0x8C, 0x89, 0x1E, 0x95,
0x91, 0x24, 0x6F, 0xF6, 0xFF, 0x6F, 0x5F, 0xF5, 0xD8, 0xB0, 0xBB, 0x58, 0xDD, 0xEA, 0x85, 0xFA,
0x67, 0xC3, 0x78, 0xEF, 0x87, 0x86, 0x6D, 0xF8, 0xD9, 0x32, 0x8C, 0xAF, 0x23, 0xC3, 0xF8, 0x39,
0x86, 0xDA, 0x27, 0xB8, 0x4C, 0x97, 0xF3, 0x33, 0x87, 0x30, 0xF2, 0x21, 0xFA, 0x56, 0x59, 0xB3,
0x1F, 0x40, 0xDB, 0x06, 0x9C, 0x5F, 0x95, 0xB5, 0xF0, 0x2E, 0x5C, 0x6C, 0x42, 0xCF, 0xA3, 0x1A,
0xD2, 0x42, 0x05, 0xA9, 0x56, 0xDC, 0x12, 0x8B, 0xC1, 0xDB, 0x29, 0xB4, 0x04, 0xA1, 0xF3, 0x06,
0x9A, 0x96, 0x8B, 0x3D, 0x2B, 0xED, 0x73, 0xF2, 0x00, 0x81, 0x75, 0xF9, 0xAA, 0x6B, 0xD8, 0xDB,
0x87, 0x01, 0x39, 0xDF, 0xB6, 0xF2, 0x0B, 0x88, 0x42, 0x67, 0xF5, 0x7F, 0x30, 0x33, 0xBE, 0x00,
0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x1D, 0x87, 0x00, 0x00, 0x1D, 0x87, 0x01,
0x8F, 0xE5, 0xF1, 0x65, 0x00, 0x00, 0x01, 0xB7, 0x49, 0x44, 0x41, 0x54, 0x28, 0x91, 0x85, 0xD2,
0x3D, 0x48, 0xD5, 0x61, 0x14, 0xC7, 0xF1, 0xCF, 0xFF, 0xFE, 0x9F, 0x3F, 0x8D, 0xD5, 0x50, 0xE0,
0xA6, 0x25, 0xE5, 0x96, 0x82, 0x48, 0x45, 0xD0, 0xD0, 0x0B, 0x0D, 0x51, 0xE0, 0x50, 0x44, 0xEF,
0x64, 0xEF, 0x71, 0x43, 0x22, 0x4C, 0xA5, 0x32, 0x2B, 0x53, 0x2B, 0x24, 0x8B, 0x86, 0xD4, 0x68,
0x91, 0xB2, 0xA1, 0x2D, 0x30, 0xC8, 0x21, 0xC8, 0x12, 0x73, 0x28, 0x32, 0x88, 0x08, 0xB1, 0x21,
0x0A, 0x22, 0x7A, 0xA1, 0xA2, 0xC0, 0xBC, 0x0D, 0xF7, 0x2A, 0x9A, 0x46, 0xBF, 0xED, 0xF0, 0x7C,
0xBF, 0x87, 0xE7, 0x1C, 0x4E, 0x64, 0xBA, 0x54, 0xB4, 0x2D, 0xC2, 0xD1, 0x5C, 0x75, 0x5E, 0xFB,
0xEE, 0x67, 0x7F, 0x23, 0x91, 0xBD, 0x37, 0xF2, 0x31, 0x13, 0xF3, 0xB0, 0x00, 0xE5, 0x28, 0xC3,
0x13, 0x8C, 0x60, 0x09, 0xFA, 0x71, 0x07, 0xAF, 0x30, 0x84, 0xCF, 0x41, 0x9C, 0x0C, 0x21, 0xC2,
0x0F, 0x3C, 0xC7, 0x00, 0x0E, 0xBA, 0xBA, 0x79, 0x00, 0x1C, 0xE8, 0x2C, 0xC2, 0x0E, 0x6C, 0x40,
0x01, 0x66, 0x63, 0x34, 0x92, 0xEE, 0xCA, 0x60, 0x0B, 0x6E, 0x6A, 0xDD, 0x38, 0x3A, 0xED, 0xD7,
0x27, 0x26, 0xDD, 0xB5, 0x09, 0x9D, 0x41, 0x9C, 0xC0, 0x17, 0x2D, 0xE5, 0xFF, 0x97, 0x20, 0x4E,
0xBE, 0x21, 0x4A, 0x89, 0x93, 0x8C, 0x38, 0xC9, 0x4C, 0x01, 0x6A, 0xEE, 0xCF, 0xFA, 0x87, 0x98,
0x11, 0x27, 0xBF, 0x83, 0x38, 0xF9, 0x8A, 0x36, 0x55, 0xDD, 0x1D, 0x68, 0xD7, 0xB4, 0xE6, 0x4D,
0x96, 0x88, 0xAE, 0xAB, 0xE9, 0x99, 0x8B, 0x0E, 0xDC, 0xD6, 0xB0, 0xE2, 0x7B, 0x4E, 0x7C, 0x8B,
0xC7, 0x91, 0x9A, 0x9E, 0x42, 0x54, 0x60, 0x27, 0xE6, 0xA0, 0x1B, 0xD7, 0xC4, 0xE1, 0x13, 0x0E,
0x61, 0x3D, 0x7E, 0xE2, 0x16, 0xDA, 0x9D, 0x5E, 0xDE, 0x0F, 0x91, 0xBA, 0xDE, 0x3C, 0x7C, 0x34,
0x3A, 0x02, 0xEB, 0xB0, 0x07, 0xAB, 0xF0, 0x0E, 0x55, 0x52, 0xA1, 0x1B, 0xDB, 0xB1, 0x0B, 0x45,
0xB2, 0x9B, 0x3F, 0x1C, 0xA9, 0xEF, 0x2B, 0xC0, 0x43, 0xAC, 0x76, 0x62, 0xF1, 0x0B, 0x70, 0xEA,
0xD1, 0xFC, 0x1C, 0x98, 0x71, 0x72, 0x69, 0xED, 0xF8, 0x7C, 0xF5, 0x7D, 0x47, 0xD0, 0x84, 0x92,
0x08, 0x9C, 0x1D, 0xB8, 0x82, 0x12, 0x2C, 0x53, 0x5B, 0x3A, 0x75, 0x51, 0x59, 0x66, 0x06, 0x9E,
0xE2, 0x81, 0xDA, 0xD2, 0x7D, 0x29, 0x90, 0x0A, 0x67, 0xA4, 0x42, 0xB1, 0x54, 0xD8, 0x3F, 0xAD,
0x94, 0x65, 0x6A, 0xA4, 0x42, 0x9E, 0x54, 0x38, 0x0E, 0x59, 0xB1, 0xBA, 0xF8, 0xBD, 0x38, 0xB4,
0x88, 0x43, 0xA3, 0xE6, 0xC1, 0x95, 0x53, 0xA4, 0xE6, 0xC1, 0xB5, 0xE2, 0x70, 0x4C, 0x1C, 0xEA,
0x55, 0x17, 0x7F, 0x80, 0x30, 0xFE, 0x18, 0x87, 0x3A, 0xD9, 0x7B, 0xBD, 0xE7, 0xE2, 0xCB, 0x06,
0xD4, 0xE5, 0x1A, 0x9F, 0x43, 0x25, 0x1A, 0x71, 0x69, 0x0C, 0x8F, 0x26, 0x75, 0x6E, 0x79, 0x1D,
0xE3, 0x02, 0xD2, 0xE8, 0x45, 0x82, 0x85, 0xD8, 0xAA, 0xB2, 0xF0, 0xEE, 0x44, 0x74, 0xB2, 0x38,
0x96, 0xD6, 0xE1, 0x32, 0x5C, 0xC6, 0x2F, 0x6C, 0x93, 0xCE, 0x1F, 0xFE, 0x1B, 0xF9, 0x03, 0x7C,
0x15, 0x71, 0x71, 0xDC, 0xC3, 0x42, 0x8A, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE,
0x42, 0x60, 0x82
};

const char main_html[] PROGMEM = R"rawliteral(
// TODO
)rawliteral";

const char main_css[] PROGMEM = R"rawliteral(
// TODO
)rawliteral";

const char main_js[] PROGMEM = R"rawliteral(
// TODO
)rawliteral";

// ----------------------------------------------------------------------------------------------------------------

sqlite3_stmt *db_res;
const char *db_tail;
int db_rc;
const char *db_path = "/littlefs/database.db";
unsigned long current_millis;
// Ticker setup_led_ticker;
Ticker ntp_ticker;
bool is_setup_done = DEFAULT_IS_SETUP_DONE;
String mac_address_raw;
String mac_address_nag;
String eth_hostname;
IPAddress eth_ip_address;
bool eth_connected = false;
bool ip_static = DEFAULT_DHCP_STATIC;
String ip_static_ip = DEFAULT_DHCP_IP;
String ip_static_mask = DEFAULT_DHCP_MASK;
String ip_static_gw = DEFAULT_DHCP_GW;
String ip_static_dns = DEFAULT_DHCP_DNS;
bool ssl_use = DEFAULT_SSL_USE;
String ssl_root_ca = DEFAULT_SSL_CA_CERTIFICATE;
bool ssl_validate_client = DEFAULT_SSL_VALIDATE_CLIENT; // TODO
String ssl_client_certificate = DEFAULT_SSL_CLIENT_CERTIFICATE; // TODO
String ssl_client_private_key = DEFAULT_SSL_CLIENT_PRIVATE_KEY; // TODO
String device_admin_username = DEFAULT_DEVICE_ADMIN_USERNAME;
String device_admin_password = DEFAULT_DEVICE_ADMIN_PASSWORD;
String mqtt_server = DEFAULT_MQTT_SERVER;
int mqtt_port = DEFAULT_MQTT_PORT;
bool mqtt_credentials = DEFAULT_MQTT_CREDENTIALS;
String mqtt_user = DEFAULT_MQTT_USER;
String mqtt_pass = DEFAULT_MQTT_PASS;
const int mqtt_check_period = 10000;
unsigned long mqtt_check_last_time = 0;
String mqtt_base_topic;
String ntp_server = DEFAULT_NTP_SERVER;
String timezone = DEFAULT_TIMEZONE;
String timezone_name = DEFAULT_TIMEZONE_NAME;

const int config_button_debounce = 250;
unsigned long config_button_period = 5000;
bool config_button_pressed = false;
bool config_button_active = false;
int config_button_last_state = 0;
unsigned long config_button_last_time = 0;

// ----------------------------------------------------------------------------------------------------------------

String mac_to_string(byte ar[]) { // done
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

String default_hostname() { // done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac_to_string((byte*) &esp_id);
  mac_address.replace(":", "");
  String hostname = MODULE_TYPE + String('-') + mac_address;
  return hostname;
}

double round_2(float value) { // done
  return (int)(value * 100 + 0.5) / 100.0;
}

double filesystemPercentageUsage() { // done
  float calc = ((float)LittleFS.usedBytes() / (float)LittleFS.totalBytes()) * 100;
  return round_2(calc);
}

String filesystemUsage() { // done
  const size_t capacity = JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> json_doc;
  json_doc["percentage"] = filesystemPercentageUsage();
  json_doc["used"] = LittleFS.usedBytes();
  json_doc["free"] = LittleFS.totalBytes() - LittleFS.usedBytes();
  json_doc["total"] = LittleFS.totalBytes();
  String data_out;
  serializeJson(json_doc, data_out);
  return data_out;
}

void setup_unit_variables() { // done
  uint64_t esp_id = ESP.getEfuseMac();
  String mac_address = mac_to_string((byte*) &esp_id);
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  mac_address_nag = mac_address;
  eth_hostname = default_hostname();
  mqtt_base_topic = "thermometer/" + mac_address_nag;
}

void setup_pins_esp() { // TODO
  pinMode(ESP_CONFIG_BUTTON, INPUT); // external pull-down (gpio2 strapping)

  SerialMon.println(F("[ESP] Success"));
}

void update_ntp_time() { // done
  configTzTime(timezone.c_str(), ntp_server.c_str(), NULL, NULL);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    SerialMon.print(F("[NTP] Received Time - "));
    SerialMon.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    rtc.adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    SerialMon.print(F("[RTC] Time Updated - "));
    SerialMon.println(rtc.now().timestamp());
  }
}

void setup_rtc() { // done
  if (!rtc.begin()) {
    SerialMon.println(F("[RTC] Error"));
    while (1);
  }
  SerialMon.println(F("[RTC] Success"));
  if (rtc.lostPower()) {
    SerialMon.println(F("[RTC] Lost power, setting default date (1/1/2000 00:00:00)"));
    rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
  }
  SerialMon.print(F("[RTC] Temperature is: "));
  SerialMon.print(rtc.getTemperature());
  SerialMon.println(F(" °C"));
}

// ----------------------------------------------------------------------------------------------------------------

int open_db(const char *filename, sqlite3 **db) { // done
  db_rc = sqlite3_open(filename, db);
  if (db_rc) {
    SerialMon.print(F("[DB] Can't open database: "));
    SerialMon.println(sqlite3_errmsg(*db));
    return db_rc;
  }
  return db_rc;
}

const char * db_exec_prep(const char *sql) { // ? - cleanup
  SerialMon.print(F("[DB] ")); // ?
  SerialMon.println(sql); // ?
  db_rc = sqlite3_prepare_v2(fs_db, sql, strlen(sql), &db_res, &db_tail);
  if (db_rc != SQLITE_OK) {
    const char *err = sqlite3_errmsg(fs_db);
    SerialMon.print(F("[DB] Failed preparing sql: "));
    SerialMon.println(err);
    sqlite3_close(fs_db);
    return err;
  }
  if (sqlite3_step(db_res) != SQLITE_DONE) {
    const char *err = sqlite3_errmsg(fs_db);
    SerialMon.print(F("[DB] ERROR executing stmt ("));
    SerialMon.print(sqlite3_step(db_res));
    SerialMon.print(F("): "));
    SerialMon.println(err);
    sqlite3_close(fs_db);
    return err;
  }
  // sqlite3_clear_bindings(db_res); // ?
  // db_rc = sqlite3_reset(db_res); // ?
  // if (db_rc != SQLITE_OK) { // ?
  //   sqlite3_close(fs_db); // ?
  //   return false; // ?
  // } // ?
  sqlite3_finalize(db_res);
  return "OK";
}

// ----------------------------------------------------------------------------------------------------------------

void publish_mqtt_birth_message() { // done
  if (!mqtt.publish((mqtt_base_topic + "/status").c_str(), MQTT_AVAILABILITY_BIRTH, true)) {
    SerialMon.println(F("[MQTT] Failed to publish birth message"));
  }
}

void publish_mqtt_init_messages() { // done
  if (mqtt.connected()) {
    mqtt.publish((mqtt_base_topic + "/storage").c_str(), filesystemUsage().c_str(), true /*false*/);

    // ?

  }
}

void handle_mqtt_message(String topic, String payload) { // TODO - subcribed topics handlers
  if (topic == (mqtt_base_topic + "/status")) {
    if (payload != MQTT_AVAILABILITY_BIRTH) { // if lwt incoming with status != online by accident, rebroadcast online status
      publish_mqtt_birth_message();
    }
  }

  // TODO

}

void mqtt_callback(char* topic, byte* payload, unsigned int length) { // ? - cleanup
  SerialMon.print(F("[MQTT] Message arrived (")); // ?
  SerialMon.print(topic); // ?
  SerialMon.println(F(")")); // ?
  for (int i = 0; i < length; i++) { // ?
    SerialMon.print((char)payload[i]); // ?
  } // ?
  SerialMon.println(); // ?

  String topic_to_process = topic;
  payload[length] = '\0';
  String payload_to_process = (char*)payload;
  handle_mqtt_message(topic_to_process, payload_to_process);
}

void setup_mqtt() { // done
  if (ssl_use) {
    mqtt.setClient(eth_client_secure);
  } else {
    mqtt.setClient(eth_client);
  }
  mqtt.setServer(mqtt_server.c_str(), mqtt_port);
  mqtt.setCallback(mqtt_callback);
  mqtt.setBufferSize(12288); // 8192
  SerialMon.println(F("[MQTT] Success"));
}

void connect_mqtt() { // TODO - subscribe to topics
  if (mqtt.connect(mac_address_nag.c_str(), mqtt_user.c_str(), mqtt_pass.c_str(), (mqtt_base_topic + "/status").c_str(), 0, true, MQTT_AVAILABILITY_LWT, MQTT_CLEAN_SESSION)) {
    SerialMon.println(F("[MQTT] Connected"));
    mqtt_check_last_time = 0;
    publish_mqtt_birth_message();
    publish_mqtt_init_messages();
    mqtt.subscribe((mqtt_base_topic + "/status").c_str());

    // TODO - subscribe to needed topics

  } else {
    SerialMon.print(F("[MQTT] Failed to connect ("));
    SerialMon.print(mqtt.state());
    SerialMon.println(F(")"));
  }
}

void handle_mqtt_connection() { // done
  if (!mqtt.connected()) {
    if (current_millis - mqtt_check_last_time >= mqtt_check_period) {
      mqtt_check_last_time = current_millis;
      connect_mqtt();
    }
  } else {
    mqtt.loop();
  }
}

// ----------------------------------------------------------------------------------------------------------------

void handle_database() { // done // TODO
  String sqlArray[] = {

  };

  if (open_db(db_path, &fs_db))
    return;
  for (int i = 0; i < sizeof(sqlArray)/sizeof(sqlArray[0]); i++) {
    db_exec_prep(sqlArray[i].c_str());
  }
  sqlite3_close(fs_db);
  SerialMon.println(F("[CONFIG] Database Loaded"));
}

void handle_config_preferences() { // done // TODO
  preferences.begin("module-config", false);
  if (preferences.isKey("eth_hostname")) {
    eth_hostname = preferences.getString("eth_hostname");
  } else {
    preferences.putString("eth_hostname", default_hostname());
  }
  if (preferences.isKey("device_username")) {
    device_admin_username = preferences.getString("device_username");
  } else {
    preferences.putString("device_username", DEFAULT_DEVICE_ADMIN_USERNAME);
  }
  if (preferences.isKey("device_password")) {
    device_admin_password = preferences.getString("device_password");
  } else {
    preferences.putString("device_password", DEFAULT_DEVICE_ADMIN_PASSWORD);
  }
  if (preferences.isKey("ip_static")) {
    ip_static = preferences.getBool("ip_static");
  } else {
    preferences.putBool("ip_static", DEFAULT_DHCP_STATIC);
  }
  if (preferences.isKey("ip_static_ip")) {
    ip_static_ip = preferences.getString("ip_static_ip");
  } else {
    preferences.putString("ip_static_ip", DEFAULT_DHCP_IP);
  }
  if (preferences.isKey("ip_static_mask")) {
    ip_static_mask = preferences.getString("ip_static_mask");
  } else {
    preferences.putString("ip_static_mask", DEFAULT_DHCP_MASK);
  }
  if (preferences.isKey("ip_static_gw")) {
    ip_static_gw = preferences.getString("ip_static_gw");
  } else {
    preferences.putString("ip_static_gw", DEFAULT_DHCP_GW);
  }
  if (preferences.isKey("ip_static_dns")) {
    ip_static_dns = preferences.getString("ip_static_dns");
  } else {
    preferences.putString("ip_static_dns", DEFAULT_DHCP_DNS);
  }
  if (preferences.isKey("ntp_server")) {
    ntp_server = preferences.getString("ntp_server");
  } else {
    preferences.putString("ntp_server", DEFAULT_NTP_SERVER);
  }
  if (preferences.isKey("time_zone")) {
    timezone = preferences.getString("time_zone");
  } else {
    preferences.putString("time_zone", DEFAULT_TIMEZONE);
  }
  if (preferences.isKey("tz_name")) {
    timezone_name = preferences.getString("tz_name");
  } else {
    preferences.putString("tz_name", DEFAULT_TIMEZONE_NAME);
  }
  if (preferences.isKey("mqtt_server")) {
    mqtt_server = preferences.getString("mqtt_server");
  } else {
    preferences.putString("mqtt_server", DEFAULT_MQTT_SERVER);
  }
  if (preferences.isKey("mqtt_port")) {
    mqtt_port = preferences.getInt("mqtt_port");
  } else {
    preferences.putInt("mqtt_port", DEFAULT_MQTT_PORT);
  }
  if (preferences.isKey("mqtt_cred")) {
    mqtt_credentials = preferences.getBool("mqtt_cred");
  } else {
    preferences.putBool("mqtt_cred", DEFAULT_MQTT_CREDENTIALS);
  }
  if (preferences.isKey("mqtt_user")) {
    mqtt_user = preferences.getString("mqtt_user");
  } else {
    preferences.putString("mqtt_user", DEFAULT_MQTT_USER);
  }
  if (preferences.isKey("mqtt_pass")) {
    mqtt_pass = preferences.getString("mqtt_pass");
  } else {
    preferences.putString("mqtt_pass", DEFAULT_MQTT_PASS);
  }
  if (preferences.isKey("ssl_use")) {
    ssl_use = preferences.getBool("ssl_use");
  } else {
    preferences.putBool("ssl_use", DEFAULT_SSL_USE);
  }
  if (preferences.isKey("ssl_root_ca")) {
    ssl_root_ca = preferences.getString("ssl_root_ca");
  } else {
    preferences.putString("ssl_root_ca", DEFAULT_SSL_CA_CERTIFICATE);
  }
  if (preferences.isKey("ssl_validate_cl")) { // ?
    ssl_validate_client = preferences.getBool("ssl_validate_cl");
  } else {
    preferences.putBool("ssl_validate_cl", DEFAULT_SSL_VALIDATE_CLIENT);
  }
  if (preferences.isKey("ssl_client_cert")) { // ?
    ssl_client_certificate = preferences.getString("ssl_client_cert");
  } else {
    preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
  }
  if (preferences.isKey("ssl_client_priv")) { // ?
    ssl_client_private_key = preferences.getString("ssl_client_priv");
  } else {
    preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
  }
  if (preferences.isKey("setup_done")) {
    is_setup_done = preferences.getBool("setup_done");
  } else {
    preferences.putBool("setup_done", DEFAULT_IS_SETUP_DONE);
  }
  preferences.end();
  SerialMon.println(F("[CONFIG] Preferences Loaded"));
}

void setup_file_system() { // done
  if (!LittleFS.begin(FORMAT_FS_IF_FAILED)) {
    SerialMon.println(F("[FS] File System Mount Failed"));
    while(1);
  } else {
    SerialMon.println(F("[FS] File System Mounted"));
    // LittleFS.remove("/database.db");
    SerialMon.print(F("[FS] Usage: "));
    SerialMon.print(filesystemPercentageUsage());
    SerialMon.print(F(" % ("));
    SerialMon.print(LittleFS.usedBytes());
    SerialMon.print(F(" / "));
    SerialMon.print(LittleFS.totalBytes());
    SerialMon.println(F(" bytes)"));
    handle_config_preferences();
    handle_database();
  }
}

// ----------------------------------------------------------------------------------------------------------------

void handle_reboot() { // done
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  SerialMon.println(F("[BOOT] Restarting"));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  ESP.restart();
}

const char *handle_post_json(DynamicJsonDocument json_doc) { // TODO // ? - proper validation of certain fields
  preferences.begin("module-config", false);
  if (json_doc.size() > 0) {
    if (json_doc.containsKey("hostname")) { // value is provided
      String json_hostname = json_doc["hostname"]; // default to ""
      json_hostname.trim();
      if (json_hostname.length() > 0) { // value is not default

        // ? - add validation for correct format

        if (json_hostname != eth_hostname) {
          preferences.putString("eth_hostname", json_hostname);
        }
      } else { // ?
        preferences.end();
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

            if (json_device_password != device_admin_password) {
              preferences.putString("device_password", json_device_password);
            }
          } else { // ?
            preferences.end();
            return "Device password change was requested, but the value of device password field is invalid!";
          }
        } else { // ?
          preferences.end();
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

                        if (!ip_static) {
                          preferences.putBool("ip_static", true);
                        }
                        if (json_ip != ip_static_ip) {
                          preferences.putString("ip_static_ip", json_ip);
                        }
                        if (json_mask != ip_static_mask) {
                          preferences.putString("ip_static_mask", json_mask);
                        }
                        if (json_gateway != ip_static_gw) {
                          preferences.putString("ip_static_gw", json_gateway);
                        }
                        if (json_dns != ip_static_dns) {
                          preferences.putString("ip_static_dns", json_dns);
                        }
                      } else {
                        preferences.end();
                        return "Set of static ip configuration was requested, but the value of dns server field is invalid!";
                      }
                    } else {
                      preferences.end();
                      return "Set of static ip configuration was requested, but the dns server field is missing!";
                    }
                  } else {
                    preferences.end();
                    return "Set of static ip configuration was requested, but the value of gateway field is invalid!";
                  }
                } else {
                  preferences.end();
                  return "Set of static ip configuration was requested, but the gateway field is missing!";
                }
              } else { // ?
                preferences.end();
                return "Set of static ip configuration was requested, but the value of subnet mask field is invalid!";
              }
            } else { // ?
              preferences.end();
              return "Set of static ip configuration was requested, but the subnet mask field is missing!";
            }
          } else { // ?
            preferences.end();
            return "Set of static ip configuration was requested, but the value of ip address field is invalid!";
          }
        } else { // ?
          preferences.end();
          return "Set of static ip configuration was requested, but the ip address field is missing!";
        }
      } else {
        if (ip_static) {
          preferences.putBool("ip_static", false);
        }
        if (String(DEFAULT_DHCP_IP) != ip_static_ip) {
          preferences.putString("ip_static_ip", DEFAULT_DHCP_IP);
        }
        if (String(DEFAULT_DHCP_MASK) != ip_static_mask) {
          preferences.putString("ip_static_mask", DEFAULT_DHCP_MASK);
        }
        if (String(DEFAULT_DHCP_GW) != ip_static_gw) {
          preferences.putString("ip_static_gw", DEFAULT_DHCP_GW);
        }
        if (String(DEFAULT_DHCP_DNS) != ip_static_dns) {
          preferences.putString("ip_static_dns", DEFAULT_DHCP_DNS);
        }
      }
    }
    if (json_doc.containsKey("mqtt_server")) { // value is provided
      String json_mqtt_server = json_doc["mqtt_server"]; // default to ""
      json_mqtt_server.trim();
      if (json_mqtt_server.length() > 0) { // value is not default
        if (json_mqtt_server != mqtt_server) {
          preferences.putString("mqtt_server", json_mqtt_server);
        }
      } else { // ?
        preferences.end();
        return "Change of mqtt server was requested, but the value of mqtt server field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_port")) { // value is provided
      int json_mqtt_port = json_doc["mqtt_port"]; // default to 0
      if (json_mqtt_port > 0) { // value is not default

        // ? - add validation for correct format

        if (json_mqtt_port != mqtt_port) {
          preferences.putInt("mqtt_port", json_mqtt_port);
        }
      } else { // ?
        preferences.end();
        return "Change of mqtt port was requested, but the value of mqtt port field is invalid!";
      }
    }
    if (json_doc.containsKey("mqtt_credentials")) { // value is provided // TODO
      bool json_mqtt_credentials = json_doc["mqtt_credentials"]; // default to false
      if (json_mqtt_credentials) {
        if (json_doc.containsKey("mqtt_username")) {
          String json_mqtt_username = json_doc["mqtt_username"]; // default to ""
          json_mqtt_username.trim();
          if (json_mqtt_username.length() > 0) {
            if (mqtt_pass != DEFAULT_MQTT_PASS && mqtt_pass.length() > 0) { // some password set -> check if password change was requested
              if (json_doc.containsKey("mqtt_password_change")) {
                bool json_mqtt_password_change = json_doc["mqtt_password_change"]; // default to false
                if (json_mqtt_password_change) {
                  if (json_doc.containsKey("mqtt_password")) {
                    String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                    json_mqtt_password.trim();
                    if (json_mqtt_password.length() > 0) {
                      if (!mqtt_credentials) {
                        preferences.putBool("mqtt_cred", true);
                      }
                      if (json_mqtt_username != mqtt_user) {
                        preferences.putString("mqtt_user", json_mqtt_username);
                      }
                      if (json_mqtt_password != mqtt_pass) {
                        preferences.putString("mqtt_pass", json_mqtt_password);
                      }
                    } else {
                      preferences.end();
                      return "Mqtt password change was requested, but the value of mqtt password field is invalid!";
                    }
                  } else {
                    preferences.end();
                    return "Mqtt password change was requested, but the mqtt password field is missing!";
                  }
                }
              }
            } else { // password not set -> enforce mqtt_pass
              if (json_doc.containsKey("mqtt_password")) {
                String json_mqtt_password = json_doc["mqtt_password"]; // default to ""
                json_mqtt_password.trim();
                if (json_mqtt_password.length() > 0) {
                  if (!mqtt_credentials) {
                    preferences.putBool("mqtt_cred", true);
                  }
                  if (json_mqtt_username != mqtt_user) {
                    preferences.putString("mqtt_user", json_mqtt_username);
                  }
                  if (json_mqtt_password != mqtt_pass) {
                    preferences.putString("mqtt_pass", json_mqtt_password);
                  }
                } else { // ?
                  preferences.end();
                  return "Set mqtt server credentials was requested, but the value of mqtt password field is invalid!";
                }
              } else {
                preferences.end();
                return "Set mqtt server credentials was requested, but the mqtt password field is missing!";
              }
            }
          } else { // ?
            preferences.end();
            return "Set mqtt server credentials was requested, but the value of mqtt username field is invalid!";
          }
        } else {
          preferences.end();
          return "Set mqtt server credentials was requested, but the mqtt username field is missing!";
        }
      } else {
        if (mqtt_credentials) {
          preferences.putBool("mqtt_cred", false);
        }
        if (String(DEFAULT_MQTT_USER) != mqtt_user) {
          preferences.putString("mqtt_user", DEFAULT_MQTT_USER);
        }
        if (String(DEFAULT_MQTT_PASS) != mqtt_pass) {
          preferences.putString("mqtt_pass", DEFAULT_MQTT_PASS);
        }
      }
    }
    if (json_doc.containsKey("ntp")) { // value is provided
      String json_ntp = json_doc["ntp"]; // default to ""
      json_ntp.trim();
      if (json_ntp.length() > 0) { // value is not default
        if (json_ntp != ntp_server) {
          preferences.putString("ntp_server", json_ntp);
        }
      } else { // ?
        preferences.end();
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
                        if (!ssl_use) {
                          preferences.putBool("ssl_use", true); // ?
                        }
                        if (json_ssl_root_ca != ssl_root_ca) {
                          preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
                        }
                        if (!ssl_validate_client) {
                          preferences.putBool("ssl_validate_cl", true); // ?
                        }
                        if (json_ssl_client_certificate != ssl_client_certificate) {
                          preferences.putString("ssl_client_cert", json_ssl_client_certificate); // ?
                        }
                        if (json_ssl_client_private_key != ssl_client_private_key) {
                          preferences.putString("ssl_client_priv", json_ssl_client_private_key); // ?
                        }
                      } else {
                        preferences.end();
                        return "Validation of tls client certificate was requested, but the value of ssl client certificate private key field is invalid!";
                      }
                    } else {
                      preferences.end();
                      return "Validation of tls client certificate was requested, but the ssl client certificate private key field is missing!";
                    }
                  } else {
                    preferences.end();
                    return "Validation of tls client certificate was requested, but the value of ssl client certificate field is invalid!";
                  }
                } else {
                  preferences.end();
                  return "Validation of tls client certificate was requested, but the ssl client certificate field is missing!";
                }
              } else {
                if (!ssl_use) {
                  preferences.putBool("ssl_use", true); // ?
                }
                if (json_ssl_root_ca != ssl_root_ca) {
                  preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
                }
                if (ssl_validate_client) { // ?
                  preferences.putBool("ssl_validate_cl", false);
                }
                if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
                  preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
                }
                if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
                  preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
                }
              }
            } else {
              // ? - what if field is missing ? -> enable tls, disable client validation || enable tls, do not modify client validation
              if (!ssl_use) {
                preferences.putBool("ssl_use", true); // ?
              }
              if (json_ssl_root_ca != ssl_root_ca) {
                preferences.putString("ssl_root_ca", json_ssl_root_ca); // ?
              }
              if (ssl_validate_client) { // ?
                preferences.putBool("ssl_validate_cl", false);
              }
              if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
                preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
              }
              if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
                preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
              }
            }
          } else { // ?
            preferences.end();
            return "Tls was requested, but the value of ssl root certificate field is invalid!";
          }
        } else { // ?
          preferences.end();
          return "Tls was requested, but the ssl root certificate field is missing!";
        }
      } else {
        if (ssl_use) {
          preferences.putBool("ssl_use", false); // ?
        }
        if (String(DEFAULT_SSL_CA_CERTIFICATE) != ssl_root_ca) {
          preferences.putString("ssl_root_ca", DEFAULT_SSL_CA_CERTIFICATE); // ?
        }
        if (ssl_validate_client) { // ?
          preferences.putBool("ssl_validate_cl", false);
        }
        if (String(DEFAULT_SSL_CLIENT_CERTIFICATE) != ssl_client_certificate) { // ?
          preferences.putString("ssl_client_cert", DEFAULT_SSL_CLIENT_CERTIFICATE);
        }
        if (String(DEFAULT_SSL_CLIENT_PRIVATE_KEY) != ssl_client_private_key) { // ?
          preferences.putString("ssl_client_priv", DEFAULT_SSL_CLIENT_PRIVATE_KEY);
        }
      }
    }
    if (!is_setup_done) {
      preferences.putBool("setup_done", true);
    }
    preferences.end();
    return "OK";
  } else {
    preferences.end();
    return "Json with empty body provided!";
  }
}

void handle_web_server_favicon_get() { // done
  web_server.send_P(200, "image/x-icon", favicon, sizeof(favicon));
}

void handle_web_server_config_get() { // TODO
  // if (!web_server.authenticate(device_admin_username.c_str(), device_admin_password.c_str())) return web_server.requestAuthentication(web_server_auth_mode, "Login Required", "Authentication Failed");
  DynamicJsonDocument json_doc(12288); // 8192
  json_doc["module_type"] = MODULE_TYPE;
  json_doc["firmware"] = MODULE_FIRMWARE_VERSION;
  json_doc["unit_id"] = mac_address_nag;
  json_doc["mac_address"] = mac_address_raw;
  json_doc["eth_hostname"] = eth_hostname;
  json_doc["mqtt_server"] = mqtt_server;
  json_doc["mqtt_port"] = mqtt_port;
  json_doc["mqtt_credentials"] = mqtt_credentials;
  json_doc["mqtt_user"] = mqtt_user;
  json_doc["mqtt_pass_set"] = (mqtt_pass != DEFAULT_MQTT_PASS && mqtt_pass.length() > 0);
  json_doc["ssl_use"] = ssl_use;
  json_doc["ssl_root_ca"] = ssl_root_ca;
  json_doc["ssl_validate_client"] = ssl_validate_client; // TODO
  json_doc["ssl_client_certificate"] = ssl_client_certificate; // TODO
  json_doc["ssl_client_private_key"] = ssl_client_private_key; // TODO
  json_doc["ntp"] = ntp_server;
  json_doc["ip_static"] = ip_static;
  json_doc["ip_static_ip"] = ip_static_ip;
  json_doc["ip_static_mask"] = ip_static_mask;
  json_doc["ip_static_gw"] = ip_static_gw;
  json_doc["ip_static_dns"] = ip_static_dns;
  // TODO
  String data_out;
  serializeJson(json_doc, data_out);
  web_server.send(200, "application/json", data_out);
}

void handle_web_server_config_post() { // done
  // if (!web_server.authenticate(device_admin_username.c_str(), device_admin_password.c_str())) return web_server.requestAuthentication(web_server_auth_mode, "Login Required", "Authentication Failed");
  if (web_server.hasArg("plain")) {
    String data = web_server.arg("plain");
    DynamicJsonDocument json_doc(12288); // 8192
    DeserializationError json_des_error = deserializeJson(json_doc, data);
    if (!json_des_error) {
      const char * result = handle_post_json(json_doc);
      if (result != "OK") {
        web_server.send(500, "text/plain", result);
      } else {
        web_server.send(200, "text/plain", "Successfully saved! Rebooting!");
        handle_reboot();
      }
    } else {
      web_server.send(500, "text/plain", json_des_error.c_str());
    }
  } else {
    web_server.send(500, "text/plain", "No payload provided!");
  }
}

void handle_web_server_css_get() { // done
  web_server.send_P(200, "text/css", main_css);
}

void handle_web_server_js_get() { // done
  web_server.send_P(200, "text/javascript", main_js);
}

void handle_web_server_html_get() { // done
  if (!web_server.authenticate(device_admin_username.c_str(), device_admin_password.c_str())) return web_server.requestAuthentication(web_server_auth_mode, "Login Required", "Authentication Failed");
  web_server.send_P(200, "text/html", main_html);
}

void setup_web_server() { // done
  web_server.on("/favicon.ico", HTTP_GET, handle_web_server_favicon_get); // public
  web_server.on("/main.css", HTTP_GET, handle_web_server_css_get); // public
  web_server.on("/main.js", HTTP_GET, handle_web_server_js_get); // public
  web_server.on("/config", HTTP_POST, handle_web_server_config_post); // public
  web_server.on("/config", HTTP_GET, handle_web_server_config_get); // protected
  web_server.on("/", HTTP_GET, handle_web_server_html_get); // protected
  web_server.onNotFound(handle_web_server_html_get);
  web_server.begin();
  SerialMon.println(F("[WEB] Success"));
}

// void setup_captive_portal() { // ?
//   WiFi.mode(WIFI_AP);
//   WiFi.softAPConfig(wifi_ap_ip, wifi_ap_gw, wifi_ap_mask);
//   WiFi.softAP(wifi_hostname.c_str(), NULL, 1, false, 1);
//   SerialMon.print(F("[AP] Created with IP Gateway "));
//   SerialMon.println(WiFi.softAPIP());
//   dnsServer.start(53, "*", wifi_ap_ip);
//   setup_web_server();
// }

// void handle_captive_portal() { // ?
//   web_server.handleClient();
//   dnsServer.processNextRequest();
// }

// void switch_to_config_mode() { // ?
//   SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
//   SerialMon.println(F("[CONFIG] Switching to config mode"));
//   preferences.begin("module-config", false);
//   preferences.putBool("setup_done", false);
//   preferences.end();
//   handle_reboot();
// }

void config_button_handler() { // reverse logic - trigger by 3v3 // TODO - usage | --- debounce not working !!!
  int config_button_current_state = digitalRead(ESP_CONFIG_BUTTON);
  if (config_button_current_state != config_button_last_state) {
    if ((current_millis - config_button_last_time) >= config_button_debounce) {
      if (config_button_current_state == HIGH) {
        if (!config_button_pressed) {
          SerialMon.println(F("[CONFIG] BUTTON PRESS")); // TODO - remove
          config_button_pressed = true;
        }
      } else {
        if (config_button_pressed) {
          SerialMon.println(F("[CONFIG] BUTTON RELEASE")); // TODO - remove
          config_button_active = false;
          config_button_pressed = false;
        }
      }
      config_button_last_state = config_button_current_state;
      config_button_last_time = current_millis;
    }
  } else {
    if (config_button_current_state == HIGH) {
      if (!config_button_active) {
        if (config_button_pressed) {
          if ((current_millis - config_button_last_time) > config_button_period) {
            SerialMon.println(F("[CONFIG] LONG PRESS")); // TODO - remove
            config_button_active = true;
            // switch_to_config_mode();
            // TODO
          }
        }
      }
    }
  }
}

void handle_ethernet_start() { // done
  SerialMon.println(F("[ETH] Started"));
  ETH.setHostname(eth_hostname.c_str());
  if (MDNS.begin(eth_hostname.c_str())) {
    SerialMon.println(F("[MDNS] Success"));
  } else {
    SerialMon.println(F("[MDNS] Error"));
  }
  if (NBNS.begin(eth_hostname.c_str())) {
    SerialMon.println(F("[NBNS] Success"));
  } else {
    SerialMon.println(F("[NBNS] Error"));
  }
}

void handle_ethernet_connect() { // done
  SerialMon.println(F("[ETH] Connected"));
}

void handle_ethernet_dhcp() { // done
  SerialMon.print(F("[ETH] Hostname: "));
  SerialMon.print(ETH.getHostname());
  SerialMon.print(F(" | MAC: "));
  SerialMon.print(ETH.macAddress());
  SerialMon.print(F(" | IPv4: "));
  SerialMon.print(ETH.localIP());
  if (ETH.fullDuplex()) SerialMon.print(F(" | FULL_DUPLEX"));
  SerialMon.print(F(" | "));
  SerialMon.print(ETH.linkSpeed());
  SerialMon.println(F("Mbps"));
  eth_connected = true;
  eth_ip_address = ETH.localIP();
}

void handle_ethernet_disconnect() { // done
  SerialMon.println(F("[ETH] Disconnected"));
  eth_connected = false;
}

void handle_ethernet_stop() { // done
  SerialMon.println(F("[ETH] Stopped"));
  eth_connected = false;
}

void wifi_event(WiFiEvent_t event) { // done // ? - add support for wifi ?
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      handle_ethernet_start();
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      handle_ethernet_connect();
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      handle_ethernet_dhcp();
      update_ntp_time(); // sync NTP immediately
      ntp_ticker.detach();
      ntp_ticker.attach(3600, []() { // sync NTP every hour
        update_ntp_time();
      });
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      ntp_ticker.detach();
      handle_ethernet_disconnect();
      break;
    case ARDUINO_EVENT_ETH_STOP:
      ntp_ticker.detach();
      handle_ethernet_stop();
      break;
    default:
      break;
  }
}

void setup_ethernet() { //  done // ?
  WiFi.onEvent(wifi_event);
  ETH.begin(ESP_ETH_PHY_ADDR, ESP_ETH_PHY_POWER, ESP_ETH_PHY_MDC, ESP_ETH_PHY_MDIO, ESP_ETH_PHY_TYPE, ESP_ETH_CLK_MODE);

  // ? - if setup is done
  if (ip_static) {
    IPAddress ip;
    if (ip.fromString(ip_static_ip)) {
      IPAddress gw;
      if (gw.fromString(ip_static_gw)) {
        IPAddress mask;
        if (mask.fromString(ip_static_mask)) {
          IPAddress dns;
          if (dns.fromString(ip_static_dns)) {
            SerialMon.println(F("[ETH] Using Static IP"));
            ETH.config(ip, gw, mask, dns);
          }
        }
      }
    }
  }
  // ? if not yet setup done -> startup dns server + use dhcp
  SerialMon.println(F("[ETH] Success"));
}

void handle_tls() { // done // TODO
  if (ssl_use) {
    eth_client_secure.setCACert(ssl_root_ca.c_str());
    SerialMon.println(F("[TLS] SSL is ON - accepting only traffic validated by provided CA certificate/s"));
    if (ssl_validate_client) { // TODO - handle client cert auth
      eth_client_secure.setCertificate(ssl_client_certificate.c_str());
      eth_client_secure.setPrivateKey(ssl_client_private_key.c_str());
      SerialMon.println(F("[TLS] Client certificate validation is ON - server will be validating provided client certificate"));
    }
  } else {
    SerialMon.println(F("[TLS] SSL is OFF - disabling TLS"));
  }
}

// ----------------------------------------------------------------------------------------------------------------

void core_0_task(void * parameter) { // This is Core 0
  for( ;; ){
    if (eth_connected) web_server.handleClient();
    if (is_setup_done) {
      config_button_handler();
      if (eth_connected) handle_mqtt_connection();
    }
  }
}

// ----------------------------------------------------------------------------------------------------------------

void setup() { // done
  SerialMon.begin(9600);
  SerialMon.println();
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  SerialMon.println("[Device Unit Type] " + String(MODULE_NAME) + " (" + String(MODULE_TYPE) + ")");
  SerialMon.println("[Firmware Version] " + String(MODULE_FIRMWARE_VERSION));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  // ----------------------------------------------------------------------------------------------------------------
  setup_pins_esp();
  Sensor1.setPins(ESP_SDA_1, ESP_SCL_1);
  Sensor2.setPins(ESP_SDA_2, ESP_SCL_2);
  setup_unit_variables();
  setup_file_system();
  sqlite3_initialize();
  setup_rtc();
  setup_ethernet();
  handle_tls();
  setup_web_server();
  setup_mqtt();
  // ----------------------------------------------------------------------------------------------------------------
  if (!is_setup_done) {
    // setup_led_ticker.detach();
    // setup_led_ticker.attach_ms(1000, []() {
    //   mcp.digitalWrite(MCP_WG_1_GREEN_LED, !mcp.digitalRead(MCP_WG_1_GREEN_LED));
    //   mcp.digitalWrite(MCP_WG_2_GREEN_LED, !mcp.digitalRead(MCP_WG_2_GREEN_LED));
    // });
  } else {
    // mcp.digitalWrite(MCP_WG_1_GREEN_LED, HIGH); // OFF
    // mcp.digitalWrite(MCP_WG_1_RED_LED, HIGH); // OFF
    // mcp.digitalWrite(MCP_WG_2_GREEN_LED, HIGH); // OFF
    // mcp.digitalWrite(MCP_WG_2_RED_LED, HIGH); // OFF
  }
  // ----------------------------------------------------------------------------------------------------------------
  SerialMon.println(F("[BOOT] Completed"));
  SerialMon.println(F("----------------------------------------------------------------------------------------------------------------"));
  // ----------------------------------------------------------------------------------------------------------------
  xTaskCreatePinnedToCore(core_0_task, "core_0_task", 16384, NULL, tskIDLE_PRIORITY, NULL, 0);
  // xTaskCreatePinnedToCore(core_0_task, "core_0_task", 10000, NULL, tskIDLE_PRIORITY, NULL, 0);
  // ----------------------------------------------------------------------------------------------------------------
}

// ----------------------------------------------------------------------------------------------------------------

void loop() { // This is Core 1
  current_millis = millis();
  if (is_setup_done) {
    // TODO
  }
}

// ----------------------------------------------------------------------------------------------------------------
