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
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Adafruit_MCP23X17.h>

#include "configuration.h" // project configuration file

// ----------------------------------------------------------------------------------------------------------------
/*
initiator_ids:
1   -   rtc - battery is dead
2   -   lock 1 - door sensor 1 - lock 1 door state
3   -   lock 1 - reader 1 - lock 1 request entry
4   -   lock 1 - button 1 - lock 1 request exit
5   -   lock 1 - reader 2 - lock 1 request exit
6   -   lock 1 - button 2 - lock 1 request entry
7   -   lock 2 - door sensor 2 - lock 2 door state
8   -   lock 2 - reader 1 - lock 2 request entry
9   -   lock 2 - button 1 - lock 2 request exit
10  -   lock 2 - reader 2 - lock 2 request exit
11  -   lock 2 - button 2 - lock 2 request entry

event_ids:
1   -   rtc - power loss
2   -   door sensor - close
3   -   door sensor - open
4   -   reader - entry - success (card in database)
5   -   button - exit
6   -   reader - entry - error (card not in database)
7   -   reader - exit - success (card in database)
8   -   reader - exit - error (card not in database)
9   -   button - entry
  TODO 10  -   reader - entry - error (outside allowed time)
  TODO 11  -   reader - exit - error (outside allowed time)
*/
// ----------------------------------------------------------------------------------------------------------------

Wiegand wg_1_1; // lock 1 - reader 1
Wiegand wg_1_2; // lock 1 - reader 2
Wiegand wg_2_1; // lock 2 - reader 1
Wiegand wg_2_2; // lock 2 - reader 2
RTC_DS3231 rtc;
sqlite3 *fs_db;
SocketIOclient socketIO;
WebServer webServer(80);
Adafruit_MCP23X17 mcp_1;
Adafruit_MCP23X17 mcp_2;

// ----------------------------------------------------------------------------------------------------------------

bool is_setup_done = DEFAULT_IS_SETUP_DONE;
String backend_server = DEFAULT_BACKEND_SERVER;
int backend_port = DEFAULT_BACKEND_PORT;
String wsio_namespace = DEFAULT_WSIO_NAMESPACE;
String ntp_server = DEFAULT_NTP_SERVER;
String timezone = DEFAULT_TIMEZONE;
bool use_secure_1 = DEFAULT_USE_SECURE_1;
bool use_secure_2 = DEFAULT_USE_SECURE_2;
bool data_reverse_1 = DEFAULT_DATA_REVERSE_1;
bool data_reverse_2 = DEFAULT_DATA_REVERSE_2;
bool silent_mode_1 = DEFAULT_SILENT_MODE_1;
bool silent_mode_2 = DEFAULT_SILENT_MODE_2;
int lock_1_reader_timeout = DEFAULT_LOCK_1_READER_TIMEOUT;
int lock_2_reader_timeout = DEFAULT_LOCK_2_READER_TIMEOUT;
int lock_1_button_timeout = DEFAULT_LOCK_1_BUTTON_TIMEOUT;
int lock_2_button_timeout = DEFAULT_LOCK_2_BUTTON_TIMEOUT;
int door_sensor_1 = DEFAULT_DOOR_SENSOR_1;
int door_sensor_2 = DEFAULT_DOOR_SENSOR_2;
String wsio_out_log_reader = DEFAULT_WSIO_OUT_LOG_READER;
String wsio_out_log_button = DEFAULT_WSIO_OUT_LOG_BUTTON;
String wsio_out_log_door = DEFAULT_WSIO_OUT_LOG_DOOR;
String wsio_out_log_rtc = DEFAULT_WSIO_OUT_LOG_RTC;

const int button_debounce_time = 500;
const int door_sensor_debounce_time = 500;
const int wg_error_timeout = 1500;
const int wg_panic_timeout = 1500;
bool wsio_connected = false;
bool eth_connected = false;
String eth_hostname;
String mac_address_nag;
String mac_address_raw;
IPAddress eth_ip_address;
bool wg_1_1_connected = false;
bool wg_1_1_halt = false;
bool wg_1_1_success = false;
bool wg_1_1_error = false;
bool wg_1_1_panic = false;
bool wg_1_2_connected = false;
bool wg_1_2_halt = false;
bool wg_1_2_success = false;
bool wg_1_2_error = false;
bool wg_1_2_panic = false;
unsigned long wg_1_1_last_time = 0;
unsigned long wg_1_2_last_time = 0;
bool wg_2_1_connected = false;
bool wg_2_1_halt = false;
bool wg_2_1_success = false;
bool wg_2_1_error = false;
bool wg_2_1_panic = false;
bool wg_2_2_connected = false;
bool wg_2_2_halt = false;
bool wg_2_2_success = false;
bool wg_2_2_error = false;
bool wg_2_2_panic = false;
unsigned long wg_2_1_last_time = 0;
unsigned long wg_2_2_last_time = 0;
unsigned long door_sensor_1_last_time = 0;
int door_sensor_1_last_state = 3; // default is invalid state
unsigned long door_sensor_2_last_time = 0;
int door_sensor_2_last_state = 3; // default is invalid state
bool button_1_1_halt = false;
bool button_1_1_success = false;
int button_1_1_last_state = 1;
bool button_1_2_halt = false;
bool button_1_2_success = false;
int button_1_2_last_state = 1;
unsigned long button_1_1_press_last_time = 0;
unsigned long button_1_1_last_time = 0;
unsigned long button_1_2_press_last_time = 0;
unsigned long button_1_2_last_time = 0;
bool button_2_1_halt = false;
bool button_2_1_success = false;
int button_2_1_last_state = 1;
bool button_2_2_halt = false;
bool button_2_2_success = false;
int button_2_2_last_state = 1;
unsigned long button_2_1_press_last_time = 0;
unsigned long button_2_1_last_time = 0;
unsigned long button_2_2_press_last_time = 0;
unsigned long button_2_2_last_time = 0;

// ----------------------------------------------------------------------------------------------------------------

void updateNtpTime() {
  configTzTime(timezone.c_str(), ntp_server.c_str());
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)){
    Serial.print("[NTP] Received Time - ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    rtc.adjust(DateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    Serial.print("[RTC] Time Updated - ");
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
    if (wsio_connected) { // log power loss - change RTC battery
      DynamicJsonDocument json_doc(1024);
      JsonArray json_array = json_doc.to<JsonArray>();
      json_array.add(wsio_out_log_rtc);
      JsonObject param_object = json_array.createNestedObject();
      param_object["unit_id"] = mac_address_nag;
      param_object["initiator_id"] = 1;
      param_object["event_id"] = 1;
      String wsio_event_data;
      serializeJson(json_doc, wsio_event_data);
      socketIO.sendEVENT(wsio_event_data);
    }
  }
}

int getInitiatorLockId(int initiator_id) {
  switch (initiator_id) {
    case 3: // reader 1_1
      return 1;
      break;
    case 5: // reader 1_2
      return 1;
      break;
    case 4: // button 1_1
      return 1;
      break;
    case 6: // button 1_2
      return 1;
      break;
    case 8: // reader 2_1
      return 2;
      break;
    case 10: // reader 2_2
      return 2;
      break;
    case 9: // button 2_1
      return 2; 
      break;
    case 11: // button 2_2
      return 2;
      break;
    default:
      return 0;
      break;
  }
}

int getReaderSuccessEventId(int initiator_id) {
  switch (initiator_id) {
    case 3: // reader 1
      return 4;
      break;
    case 5: // reader 2
      return 7;
      break;
    case 8:
      return 4;
      break;
    case 10:
      return 7;
      break;
    default:
      return 0;
      break;
  }
}

int getReaderErrorEventId(int initiator_id) {
  switch (initiator_id) {
    case 3: // reader 1
      return 6;
      break;
    case 5: // reader 2
      return 8;
      break;
    case 8:
      return 6;
      break;
    case 10:
      return 8;
      break;
    default:
      return 0;
      break;
  }
}

void openLock(int lock_id) {
  switch (lock_id) {
    case 1:
      mcp_1.digitalWrite(MCP_1_LOCK_1_TRIGGER, LOW); // ON
      break;
    case 2:
      mcp_1.digitalWrite(MCP_1_LOCK_2_TRIGGER, LOW); // ON
      break;
    default:
      break;
  }
}

void closeLock(int lock_id) {
  switch (lock_id) {
    case 1:
      mcp_1.digitalWrite(MCP_1_LOCK_1_TRIGGER, HIGH); // OFF
      break;
    case 2:
      mcp_1.digitalWrite(MCP_1_LOCK_2_TRIGGER, HIGH); // OFF
      break;
    default:
      break;
  }
}

void readerBuzzerOn(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (!silent_mode_1 && wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_BUZZER, LOW); // ON
      break;
    case 5:
      if (!silent_mode_1 && wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_BUZZER, LOW); // ON
      break;
    case 8:
      if (!silent_mode_2 && wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_BUZZER, LOW); // ON
      break;
    case 10:
      if (!silent_mode_2 && wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_BUZZER, LOW); // ON
      break;
    default:
      break;
  }
}

void readerBuzzerOff(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (!silent_mode_1 && wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_BUZZER, HIGH); // OFF
      break;
    case 5:
      if (!silent_mode_1 && wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_BUZZER, HIGH); // OFF
      break;
    case 8:
      if (!silent_mode_2 && wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_BUZZER, HIGH); // OFF
      break;
    case 10:
      if (!silent_mode_2 && wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_BUZZER, HIGH); // OFF
      break;
    default:
      break;
  }
}

void readerRedLedOn(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_RED_LED, LOW); // ON
      break;
    case 5:
      if (wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_RED_LED, LOW); // ON
      break;
    case 8:
      if (wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_RED_LED, LOW); // ON
      break;
    case 10:
      if (wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_RED_LED, LOW); // ON
      break;
    default:
      break;
  }
}

void readerRedLedOff(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_RED_LED, HIGH); // OFF
      break;
    case 5:
      if (wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_RED_LED, HIGH); // OFF
      break;
    case 8:
      if (wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_RED_LED, HIGH); // OFF
      break;
    case 10:
      if (wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_RED_LED, HIGH); // OFF
      break;
    default:
      break;
  }
}

void readerGreenLedOn(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_GREEN_LED, LOW); // ON
      break;
    case 5:
      if (wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_GREEN_LED, LOW); // ON
      break;
    case 8:
      if (wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_GREEN_LED, LOW); // ON
      break;
    case 10:
      if (wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_GREEN_LED, LOW); // ON
      break;
    default:
      break;
  }
}

void readerGreenLedOff(int initiator_id) {
  switch (initiator_id) {
    case 3:
      if (wg_1_1_connected) mcp_2.digitalWrite(MCP_2_WG_1_1_GREEN_LED, HIGH); // OFF
      break;
    case 5:
      if (wg_1_2_connected) mcp_2.digitalWrite(MCP_2_WG_1_2_GREEN_LED, HIGH); // OFF
      break;
    case 8:
      if (wg_2_1_connected) mcp_2.digitalWrite(MCP_2_WG_2_1_GREEN_LED, HIGH); // OFF
      break;
    case 10:
      if (wg_2_2_connected) mcp_2.digitalWrite(MCP_2_WG_2_2_GREEN_LED, HIGH); // OFF
      break;
    default:
      break;
  }
}

void readerErrorStatus(int initiator_id) {
  switch (initiator_id) {
    case 3:
      wg_1_1_last_time = millis();
      wg_1_1_halt = true;
      wg_1_1_error = true;
      readerRedLedOn(initiator_id);
      break;
    case 5:
      wg_1_2_last_time = millis();
      wg_1_2_halt = true;
      wg_1_2_error = true;
      readerRedLedOn(initiator_id);
      break;
    case 8:
      wg_2_1_last_time = millis();
      wg_2_1_halt = true;
      wg_2_1_error = true;
      readerRedLedOn(initiator_id);
      break;
    case 10:
      wg_2_2_last_time = millis();
      wg_2_2_halt = true;
      wg_2_2_error = true;
      readerRedLedOn(initiator_id);
      break;
    default:
      break;
  }
}

void readerPanicStatus(int initiator_id) {
  switch (initiator_id) {
    case 3:
      wg_1_1_last_time = millis();
      wg_1_1_halt = true;
      wg_1_1_panic = true;
      readerRedLedOn(initiator_id);
      readerGreenLedOn(initiator_id);
      break;
    case 5:
      wg_1_2_last_time = millis();
      wg_1_2_halt = true;
      wg_1_2_panic = true;
      readerRedLedOn(initiator_id);
      readerGreenLedOn(initiator_id);
      break;
    case 8:
      wg_2_1_last_time = millis();
      wg_2_1_halt = true;
      wg_2_1_panic = true;
      readerRedLedOn(initiator_id);
      readerGreenLedOn(initiator_id);
      break;
    case 10:
      wg_2_2_last_time = millis();
      wg_2_2_halt = true;
      wg_2_2_panic = true;
      readerRedLedOn(initiator_id);
      readerGreenLedOn(initiator_id);
      break;
    default:
      break;
  }
}

void readerSuccessStatus(int initiator_id) {
  switch (initiator_id) {
    case 3:
      wg_1_1_last_time = millis();
      wg_1_1_halt = true;
      wg_1_1_success = true;
      wg_1_2_halt = true;
      button_1_1_halt = true;
      button_1_2_halt = true;
      if (wg_1_2_error) {
        wg_1_2_error = false;
        readerRedLedOff(5);
      }
      if (wg_1_2_panic) {
        wg_1_2_panic = false;
        readerRedLedOff(5);
      }
      readerGreenLedOn(3);
      readerGreenLedOn(5);
      openLock(1);
      break;
    case 5:
      wg_1_2_last_time = millis();
      wg_1_2_halt = true;
      wg_1_2_success = true;
      wg_1_1_halt = true;
      button_1_1_halt = true;
      button_1_2_halt = true;
      if (wg_1_1_error) {
        wg_1_1_error = false;
        readerRedLedOff(3);
      }
      if (wg_1_1_panic) {
        wg_1_1_panic = false;
        readerRedLedOff(3);
      }
      readerGreenLedOn(5);
      readerGreenLedOn(3);
      openLock(1);
      break;
    case 8:
      wg_2_1_last_time = millis();
      wg_2_1_halt = true;
      wg_2_1_success = true;
      wg_2_2_halt = true;
      button_2_1_halt = true;
      button_2_2_halt = true;
      if (wg_2_2_error) {
        wg_2_2_error = false;
        readerRedLedOff(10);
      }
      if (wg_2_2_panic) {
        wg_2_2_panic = false;
        readerRedLedOff(10);
      }
      readerGreenLedOn(8);
      readerGreenLedOn(10);
      openLock(2);
      break;
    case 10:
      wg_2_2_last_time = millis();
      wg_2_2_halt = true;
      wg_2_2_success = true;
      wg_2_1_halt = true;
      button_2_1_halt = true;
      button_2_2_halt = true;
      if (wg_2_1_error) {
        wg_2_1_error = false;
        readerRedLedOff(8);
      }
      if (wg_2_1_panic) {
        wg_2_1_panic = false;
        readerRedLedOff(8);
      }
      readerGreenLedOn(10);
      readerGreenLedOn(8);
      openLock(2);
      break;
    default:
      break;
  }
}

void buttonSuccessStatus(int initiator_id) {
  switch (initiator_id) {
    case 4:
      button_1_1_last_time = millis();
      button_1_1_halt = true;
      button_1_1_success = true;
      wg_1_1_halt = true;
      wg_1_2_halt = true;
      button_1_2_halt = true;
      if (wg_1_1_error) {
        wg_1_1_error = false;
        readerRedLedOff(3);
      }
      if (wg_1_1_panic) {
        wg_1_1_panic = false;
        readerRedLedOff(3);
      }
      if (wg_1_2_error) {
        wg_1_2_error = false;
        readerRedLedOff(5);
      }
      if (wg_1_2_panic) {
        wg_1_2_panic = false;
        readerRedLedOff(5);
      }
      readerGreenLedOn(3);
      readerGreenLedOn(5);
      openLock(1);
      break;
    case 6:
      button_1_2_last_time = millis();
      button_1_2_halt = true;
      button_1_2_success = true;
      wg_1_1_halt = true;
      wg_1_2_halt = true;
      button_1_2_halt = true;
      if (wg_1_1_error) {
        wg_1_1_error = false;
        readerRedLedOff(3);
      }
      if (wg_1_1_panic) {
        wg_1_1_panic = false;
        readerRedLedOff(3);
      }
      if (wg_1_2_error) {
        wg_1_2_error = false;
        readerRedLedOff(5);
      }
      if (wg_1_2_panic) {
        wg_1_2_panic = false;
        readerRedLedOff(5);
      }
      readerGreenLedOn(5);
      readerGreenLedOn(3);
      openLock(1);
      break;
    case 9:
      button_2_1_last_time = millis();
      button_2_1_halt = true;
      button_2_1_success = true;
      wg_2_1_halt = true;
      wg_2_2_halt = true;
      button_2_2_halt = true;
      if (wg_2_1_error) {
        wg_2_1_error = false;
        readerRedLedOff(8);
      }
      if (wg_2_1_panic) {
        wg_2_1_panic = false;
        readerRedLedOff(8);
      }
      if (wg_2_2_error) {
        wg_2_2_error = false;
        readerRedLedOff(10);
      }
      if (wg_2_2_panic) {
        wg_2_2_panic = false;
        readerRedLedOff(10);
      }
      readerGreenLedOn(8);
      readerGreenLedOn(10);
      openLock(2);
      break;
    case 11:
      button_2_2_last_time = millis();
      button_2_2_halt = true;
      button_2_2_success = true;
      wg_2_1_halt = true;
      wg_2_2_halt = true;
      button_2_2_halt = true;
      if (wg_2_1_error) {
        wg_2_1_error = false;
        readerRedLedOff(8);
      }
      if (wg_2_1_panic) {
        wg_2_1_panic = false;
        readerRedLedOff(8);
      }
      if (wg_2_2_error) {
        wg_2_2_error = false;
        readerRedLedOff(10);
      }
      if (wg_2_2_panic) {
        wg_2_2_panic = false;
        readerRedLedOff(10);
      }
      readerGreenLedOn(10);
      readerGreenLedOn(8);
      openLock(2);
      break;
    default:
      break;
  }
}

void setupEspPins() {
  pinMode(ESP_RXD_LED, OUTPUT);
  pinMode(ESP_TXD_LED, OUTPUT);
  digitalWrite(ESP_RXD_LED, HIGH); // OFF
  digitalWrite(ESP_TXD_LED, HIGH); // OFF
  pinMode(ESP_WG_1_1_D0, INPUT);
  pinMode(ESP_WG_1_1_D1, INPUT);
  pinMode(ESP_WG_1_2_D0, INPUT);
  pinMode(ESP_WG_1_2_D1, INPUT);
  pinMode(ESP_WG_2_1_D0, INPUT);
  pinMode(ESP_WG_2_1_D1, INPUT);
  pinMode(ESP_WG_2_2_D0, INPUT);
  pinMode(ESP_WG_2_2_D1, INPUT);
  Serial.println("[ESP] Success");
}

void setupMcp1Pins() {
  if (!mcp_1.begin_I2C(MCP_1_ADDR)) {
    Serial.println("[MCP_1] Error");
    while (1);
  }
  mcp_1.pinMode(MCP_1_LOCK_1_TRIGGER, OUTPUT);
  mcp_1.digitalWrite(MCP_1_LOCK_1_TRIGGER, HIGH); // OFF - as soon as possible
  mcp_1.pinMode(MCP_1_LOCK_2_TRIGGER, OUTPUT);
  mcp_1.digitalWrite(MCP_1_LOCK_2_TRIGGER, HIGH); // OFF - as soon as possible
  mcp_1.pinMode(MCP_1_DOOR_SENSOR_1, INPUT); // ? - validate input mode // external input pull-up resistor
  mcp_1.pinMode(MCP_1_DOOR_SENSOR_2, INPUT); // ? - validate input mode // external input pull-up resistor
  mcp_1.pinMode(MCP_1_BUTTON_1_1, INPUT); // ? - validate input mode // external input pull-up resistor
  mcp_1.pinMode(MCP_1_BUTTON_1_2, INPUT); // ? - validate input mode // external input pull-up resistor
  mcp_1.pinMode(MCP_1_BUTTON_2_1, INPUT); // ? - validate input mode // external input pull-up resistor
  mcp_1.pinMode(MCP_1_BUTTON_2_2, INPUT); // ? - validate input mode // external input pull-up resistor
  Serial.println("[MCP_1] Success");
}

void setupMcp2Pins() {
  if (!mcp_2.begin_I2C(MCP_2_ADDR)) {
    Serial.println("[MCP_2] Error");
    while (1);
  }
  mcp_2.pinMode(MCP_2_WG_1_1_GREEN_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_1_GREEN_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_1_1_RED_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_1_RED_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_1_1_BUZZER, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_1_BUZZER, HIGH); // OFF
  mcp_2.pinMode(MCP_2_WG_1_2_GREEN_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_2_GREEN_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_1_2_RED_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_2_RED_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_1_2_BUZZER, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_1_2_BUZZER, HIGH); // OFF
  mcp_2.pinMode(MCP_2_WG_2_1_GREEN_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_1_GREEN_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_2_1_RED_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_1_RED_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_2_1_BUZZER, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_1_BUZZER, HIGH); // OFF
  mcp_2.pinMode(MCP_2_WG_2_2_GREEN_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_2_GREEN_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_2_2_RED_LED, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_2_RED_LED, LOW); // ON
  mcp_2.pinMode(MCP_2_WG_2_2_BUZZER, OUTPUT);
  mcp_2.digitalWrite(MCP_2_WG_2_2_BUZZER, HIGH); // OFF
  Serial.println("[MCP_2] Success");
}

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
  
  is_setup_done = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'is_setup_done'", DEFAULT_IS_SETUP_DONE);
  ntp_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'ntp_server'", DEFAULT_NTP_SERVER);
  timezone = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'time_zone'", DEFAULT_TIMEZONE);
  backend_server = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'backend_server'", DEFAULT_BACKEND_SERVER);
  backend_port = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'backend_port'", DEFAULT_BACKEND_PORT);
  lock_1_reader_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'lock_1_reader_timeout'", DEFAULT_LOCK_1_READER_TIMEOUT);
  lock_2_reader_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'lock_2_reader_timeout'", DEFAULT_LOCK_2_READER_TIMEOUT);
  door_sensor_1 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'door_sensor_1'", DEFAULT_DOOR_SENSOR_1);
  door_sensor_2 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'door_sensor_2'", DEFAULT_DOOR_SENSOR_2);
  lock_1_button_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'lock_1_button_timeout'", DEFAULT_LOCK_1_BUTTON_TIMEOUT);
  lock_2_button_timeout = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'lock_2_button_timeout'", DEFAULT_LOCK_2_BUTTON_TIMEOUT);
  use_secure_1 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'use_secure_1'", DEFAULT_USE_SECURE_1);
  use_secure_2 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'use_secure_2'", DEFAULT_USE_SECURE_2);
  data_reverse_1 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'data_reverse_1'", DEFAULT_DATA_REVERSE_1);
  data_reverse_2 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'data_reverse_2'", DEFAULT_DATA_REVERSE_2);
  silent_mode_1 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'silent_mode_1'", DEFAULT_SILENT_MODE_1);
  silent_mode_2 = loadConfigIntVariable("SELECT value FROM config WHERE variable = 'silent_mode_2'", DEFAULT_SILENT_MODE_2);
  wsio_out_log_reader = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'wsio_out_log_reader'", DEFAULT_WSIO_OUT_LOG_READER);
  wsio_out_log_button = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'wsio_out_log_button'", DEFAULT_WSIO_OUT_LOG_BUTTON);
  wsio_out_log_door = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'wsio_out_log_door'", DEFAULT_WSIO_OUT_LOG_DOOR);
  wsio_out_log_rtc = loadConfigStringVariable("SELECT value FROM config WHERE variable = 'wsio_out_log_rtc'", DEFAULT_WSIO_OUT_LOG_RTC);

  sqlite3_close(fs_db);

  Serial.println("[FS] Config Loaded From Database File");
}

void initDatabaseFile(const char *database_path, sqlite3 *database_instance) {
  if (openDb(database_path, &database_instance))
    return;

  String config_create_query = "CREATE TABLE IF NOT EXISTS config (variable TEXT, value)";
  int rc_config = dbExec(database_instance, config_create_query.c_str());
  if (rc_config != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String cards_1_create_query = "CREATE TABLE IF NOT EXISTS cards_1 (card_hex TEXT, secure_hex TEXT)";
  int rc_cards_1 = dbExec(database_instance, cards_1_create_query.c_str());
  if (rc_cards_1 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String cards_2_create_query = "CREATE TABLE IF NOT EXISTS cards_2 (card_hex TEXT, secure_hex TEXT)";
  int rc_cards_2 = dbExec(database_instance, cards_2_create_query.c_str());
  if (rc_cards_2 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String events_log_create_query = "CREATE TABLE IF NOT EXISTS events_log (timestamp TEXT, lock_id INTEGER, initiator_id INTEGER, event_id INTEGER, card_hex TEXT)";
  int rc_events_log = dbExec(database_instance, events_log_create_query.c_str());
  if (rc_events_log != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_is_setup_done_insert_query = "INSERT INTO config VALUES ('is_setup_done','" + String(DEFAULT_IS_SETUP_DONE) + "')";
  int rc_config_is_setup_done = dbExec(database_instance, config_is_setup_done_insert_query.c_str());
  if (rc_config_is_setup_done != SQLITE_OK) {
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

  String config_lock_1_reader_timeout_insert_query = "INSERT INTO config VALUES ('lock_1_reader_timeout','" + String(DEFAULT_LOCK_1_READER_TIMEOUT) + "')";
  int rc_config_lock_1_reader_timeout = dbExec(database_instance, config_lock_1_reader_timeout_insert_query.c_str());
  if (rc_config_lock_1_reader_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_lock_2_reader_timeout_insert_query = "INSERT INTO config VALUES ('lock_2_reader_timeout','" + String(DEFAULT_LOCK_2_READER_TIMEOUT) + "')";
  int rc_config_lock_2_reader_timeout = dbExec(database_instance, config_lock_2_reader_timeout_insert_query.c_str());
  if (rc_config_lock_2_reader_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_door_sensor_1_insert_query = "INSERT INTO config VALUES ('door_sensor_1','" + String(DEFAULT_DOOR_SENSOR_1) + "')";
  int rc_config_door_sensor_1 = dbExec(database_instance, config_door_sensor_1_insert_query.c_str());
  if (rc_config_door_sensor_1 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_door_sensor_2_insert_query = "INSERT INTO config VALUES ('door_sensor_2','" + String(DEFAULT_DOOR_SENSOR_2) + "')";
  int rc_config_door_sensor_2 = dbExec(database_instance, config_door_sensor_2_insert_query.c_str());
  if (rc_config_door_sensor_2 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_lock_1_button_timeout_insert_query = "INSERT INTO config VALUES ('lock_1_button_timeout','" + String(DEFAULT_LOCK_1_BUTTON_TIMEOUT) + "')";
  int rc_config_lock_1_button_timeout = dbExec(database_instance, config_lock_1_button_timeout_insert_query.c_str());
  if (rc_config_lock_1_button_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_lock_2_button_timeout_insert_query = "INSERT INTO config VALUES ('lock_2_button_timeout','" + String(DEFAULT_LOCK_2_BUTTON_TIMEOUT) + "')";
  int rc_config_lock_2_button_timeout = dbExec(database_instance, config_lock_2_button_timeout_insert_query.c_str());
  if (rc_config_lock_2_button_timeout != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_use_secure_1_insert_query = "INSERT INTO config VALUES ('use_secure_1','" + String(DEFAULT_USE_SECURE_1) + "')";
  int rc_config_use_secure_1 = dbExec(database_instance, config_use_secure_1_insert_query.c_str());
  if (rc_config_use_secure_1 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_use_secure_2_insert_query = "INSERT INTO config VALUES ('use_secure_2','" + String(DEFAULT_USE_SECURE_2) + "')";
  int rc_config_use_secure_2 = dbExec(database_instance, config_use_secure_2_insert_query.c_str());
  if (rc_config_use_secure_2 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_data_reverse_1_insert_query = "INSERT INTO config VALUES ('data_reverse_1','" + String(DEFAULT_DATA_REVERSE_1) + "')";
  int rc_config_data_reverse_1 = dbExec(database_instance, config_data_reverse_1_insert_query.c_str());
  if (rc_config_data_reverse_1 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_data_reverse_2_insert_query = "INSERT INTO config VALUES ('data_reverse_2','" + String(DEFAULT_DATA_REVERSE_2) + "')";
  int rc_config_data_reverse_2 = dbExec(database_instance, config_data_reverse_2_insert_query.c_str());
  if (rc_config_data_reverse_2 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_silent_mode_1_insert_query = "INSERT INTO config VALUES ('silent_mode_1','" + String(DEFAULT_SILENT_MODE_1) + "')";
  int rc_config_silent_mode_1 = dbExec(database_instance, config_silent_mode_1_insert_query.c_str());
  if (rc_config_silent_mode_1 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_silent_mode_2_insert_query = "INSERT INTO config VALUES ('silent_mode_2','" + String(DEFAULT_SILENT_MODE_2) + "')";
  int rc_config_silent_mode_2 = dbExec(database_instance, config_silent_mode_2_insert_query.c_str());
  if (rc_config_silent_mode_2 != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_wsio_out_log_reader_insert_query = "INSERT INTO config VALUES ('wsio_out_log_reader','" + String(DEFAULT_WSIO_OUT_LOG_READER) + "')";
  int rc_config_wsio_out_log_reader = dbExec(database_instance, config_wsio_out_log_reader_insert_query.c_str());
  if (rc_config_wsio_out_log_reader != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_wsio_out_log_button_insert_query = "INSERT INTO config VALUES ('wsio_out_log_button','" + String(DEFAULT_WSIO_OUT_LOG_BUTTON) + "')";
  int rc_config_wsio_out_log_button = dbExec(database_instance, config_wsio_out_log_button_insert_query.c_str());
  if (rc_config_wsio_out_log_button != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_wsio_out_log_door_insert_query = "INSERT INTO config VALUES ('wsio_out_log_door','" + String(DEFAULT_WSIO_OUT_LOG_DOOR) + "')";
  int rc_config_wsio_out_log_door = dbExec(database_instance, config_wsio_out_log_door_insert_query.c_str());
  if (rc_config_wsio_out_log_door != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

  String config_wsio_out_log_rtc_insert_query = "INSERT INTO config VALUES ('wsio_out_log_rtc','" + String(DEFAULT_WSIO_OUT_LOG_RTC) + "')";
  int rc_config_wsio_out_log_rtc = dbExec(database_instance, config_wsio_out_log_rtc_insert_query.c_str());
  if (rc_config_wsio_out_log_rtc != SQLITE_OK) {
    sqlite3_close(database_instance);
    return;
  }

            // TODO - for test only
            String cards_insert_test_query_01 = "INSERT INTO cards_1 VALUES ('04C2B401', NULL)";
            int cards_insert_test_01_rc = dbExec(database_instance, cards_insert_test_query_01.c_str());
            if (cards_insert_test_01_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_02 = "INSERT INTO cards_1 VALUES ('A4DCCC1D', NULL)";
            int cards_insert_test_02_rc = dbExec(database_instance, cards_insert_test_query_02.c_str());
            if (cards_insert_test_02_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_03 = "INSERT INTO cards_1 VALUES ('44C20C45', '364A9D')";
            int cards_insert_test_03_rc = dbExec(database_instance, cards_insert_test_query_03.c_str());
            if (cards_insert_test_03_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_04 = "INSERT INTO cards_2 VALUES ('04C2B401', NULL)";
            int cards_insert_test_04_rc = dbExec(database_instance, cards_insert_test_query_04.c_str());
            if (cards_insert_test_04_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_05 = "INSERT INTO cards_2 VALUES ('A4DCCC1D', NULL)";
            int cards_insert_test_05_rc = dbExec(database_instance, cards_insert_test_query_05.c_str());
            if (cards_insert_test_05_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }
            String cards_insert_test_query_06 = "INSERT INTO cards_2 VALUES ('44C20C45', '364A9D')";
            int cards_insert_test_06_rc = dbExec(database_instance, cards_insert_test_query_06.c_str());
            if (cards_insert_test_06_rc != SQLITE_OK) {
              sqlite3_close(database_instance);
              return;
            }

  sqlite3_close(database_instance);

  Serial.println("[FS] Database Structure Written Into Database File");
}

void setupFileSystem() {
  // if (!SPIFFS.begin(FORMAT_FS_IF_FAILED, "/spiffs", 10)) {
  if (!SPIFFS.begin(FORMAT_FS_IF_FAILED)) {
    Serial.println("[FS] File System Mount Failed");
    while(1);
  } else {
    Serial.println("[FS] File System Mounted");
    if (SPIFFS.exists("/database.db")) {
      Serial.println("[FS] Database File Exists");
      loadConfig();
    } else {
      Serial.println("[FS] Database File Does Not Exist");
      File dbFile;
      dbFile = SPIFFS.open("/database.db", FILE_WRITE);
      dbFile.close();
      Serial.println("[FS] Database File Created");
      initDatabaseFile("/spiffs/database.db", fs_db);
    }
  }
}

void handleWebServerRootGet() {
  DynamicJsonDocument json_doc(1024);
  json_doc["name"] = MODULE_NAME;
  json_doc["type"] = MODULE_TYPE;
  json_doc["hardware_version"] = MODULE_HARDWARE_VERSION;
  json_doc["firmware_version"] = MODULE_FIRMWARE_VERSION;
  json_doc["unit_id"] = mac_address_nag;
  json_doc["hostname"] = eth_hostname;
  json_doc["mac_address"] = mac_address_raw;
  String data_out;
  serializeJson(json_doc, data_out);
  webServer.send(200, "application/json", data_out);
}

void handleWebServerRootPost() {
  String postBody = webServer.arg("plain");
  DynamicJsonDocument json_doc(1024);
  DeserializationError error = deserializeJson(json_doc, postBody);
  if (error) {
    DynamicJsonDocument doc(512);
    doc["status"] = "error";
    doc["message"] = "Error in parsing json body";
    doc["error"] = error.c_str();
    String buf;
    serializeJson(doc, buf);
    webServer.send(400, "application/json", buf);
  } else {
    JsonObject postObj = json_doc.as<JsonObject>();
    if (postObj.containsKey("backend_server")
      && postObj.containsKey("backend_port")
      && postObj.containsKey("wsio_namespace")
      && postObj.containsKey("ntp_server")
      && postObj.containsKey("timezone")
      && postObj.containsKey("use_secure_1")
      && postObj.containsKey("use_secure_2")
      && postObj.containsKey("data_reverse_1")
      && postObj.containsKey("data_reverse_2")
      && postObj.containsKey("silent_mode_1")
      && postObj.containsKey("silent_mode_2")
      && postObj.containsKey("lock_1_reader_timeout")
      && postObj.containsKey("lock_2_reader_timeout")
      && postObj.containsKey("door_sensor_1")
      && postObj.containsKey("door_sensor_2")
      && postObj.containsKey("lock_1_button_timeout")
      && postObj.containsKey("lock_2_button_timeout")
      && postObj.containsKey("wsio_out_log_reader")
      && postObj.containsKey("wsio_out_log_button")
      && postObj.containsKey("wsio_out_log_door")
      && postObj.containsKey("wsio_out_log_rtc")) {
      JsonVariant json_backend_server = postObj.getMember("backend_server");
      String inc_backend_server = json_backend_server.as<String>();
      JsonVariant json_backend_port = postObj.getMember("backend_port");
      int inc_backend_port = json_backend_port.as<int>();
      JsonVariant json_wsio_namespace = postObj.getMember("wsio_namespace");
      String inc_wsio_namespace = json_wsio_namespace.as<String>();
      JsonVariant json_ntp_server = postObj.getMember("ntp_server");
      String inc_ntp_server = json_ntp_server.as<String>();
      JsonVariant json_timezone = postObj.getMember("timezone");
      String inc_timezone = json_timezone.as<String>();
      JsonVariant json_use_secure_1 = postObj.getMember("use_secure_1");
      bool inc_use_secure_1 = json_use_secure_1.as<bool>();
      JsonVariant json_use_secure_2 = postObj.getMember("use_secure_2");
      bool inc_use_secure_2 = json_use_secure_2.as<bool>();
      JsonVariant json_data_reverse_1 = postObj.getMember("data_reverse_1");
      bool inc_data_reverse_1 = json_data_reverse_1.as<bool>();
      JsonVariant json_data_reverse_2 = postObj.getMember("data_reverse_2");
      bool inc_data_reverse_2 = json_data_reverse_2.as<bool>();
      JsonVariant json_silent_mode_1 = postObj.getMember("silent_mode_1");
      bool inc_silent_mode_1 = json_silent_mode_1.as<bool>();
      JsonVariant json_silent_mode_2 = postObj.getMember("silent_mode_2");
      bool inc_silent_mode_2 = json_silent_mode_2.as<bool>();
      JsonVariant json_lock_1_reader_timeout = postObj.getMember("lock_1_reader_timeout");
      int inc_lock_1_reader_timeout = json_lock_1_reader_timeout.as<int>();
      JsonVariant json_lock_2_reader_timeout = postObj.getMember("lock_2_reader_timeout");
      int inc_lock_2_reader_timeout = json_lock_2_reader_timeout.as<int>();
      JsonVariant json_door_sensor_1 = postObj.getMember("door_sensor_1");
      bool inc_door_sensor_1 = json_door_sensor_1.as<bool>();
      JsonVariant json_door_sensor_2 = postObj.getMember("door_sensor_2");
      bool inc_door_sensor_2 = json_door_sensor_2.as<bool>();
      JsonVariant json_lock_1_button_timeout = postObj.getMember("lock_1_button_timeout");
      int inc_lock_1_button_timeout = json_lock_1_button_timeout.as<int>();
      JsonVariant json_lock_2_button_timeout = postObj.getMember("lock_2_button_timeout");
      int inc_lock_2_button_timeout = json_lock_2_button_timeout.as<int>();
      JsonVariant json_wsio_out_log_reader = postObj.getMember("wsio_out_log_reader");
      String inc_wsio_out_log_reader = json_wsio_out_log_reader.as<String>();
      JsonVariant json_wsio_out_log_button = postObj.getMember("wsio_out_log_button");
      String inc_wsio_out_log_button = json_wsio_out_log_button.as<String>();
      JsonVariant json_wsio_out_log_door = postObj.getMember("wsio_out_log_door");
      String inc_wsio_out_log_door = json_wsio_out_log_door.as<String>();
      JsonVariant json_wsio_out_log_rtc = postObj.getMember("wsio_out_log_rtc");
      String inc_wsio_out_log_rtc = json_wsio_out_log_rtc.as<String>();
      if (openDb("/spiffs/database.db", &fs_db))
        return;
      String config_backend_server = "UPDATE config SET value = '" + inc_backend_server + "' WHERE variable = 'backend_server'";
      int config_backend_server_rc = dbExec(fs_db, config_backend_server.c_str());
      if (config_backend_server_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_backend_port = "UPDATE config SET value = '" + String(inc_backend_port) + "' WHERE variable = 'backend_port'";
      int config_backend_port_rc = dbExec(fs_db, config_backend_port.c_str());
      if (config_backend_port_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_wsio_namespace = "UPDATE config SET value = '" + inc_wsio_namespace + "' WHERE variable = 'wsio_namespace'";
      int config_wsio_namespace_rc = dbExec(fs_db, config_wsio_namespace.c_str());
      if (config_wsio_namespace_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_ntp_server = "UPDATE config SET value = '" + inc_ntp_server + "' WHERE variable = 'ntp_server'";
      int config_ntp_server_rc = dbExec(fs_db, config_ntp_server.c_str());
      if (config_ntp_server_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_timezone = "UPDATE config SET value = '" + inc_timezone + "' WHERE variable = 'time_zone'";
      int config_timezone_rc = dbExec(fs_db, config_timezone.c_str());
      if (config_timezone_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_use_secure_1 = "UPDATE config SET value = '" + String(inc_use_secure_1) + "' WHERE variable = 'use_secure_1'";
      int config_use_secure_1_rc = dbExec(fs_db, config_use_secure_1.c_str());
      if (config_use_secure_1_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_use_secure_2 = "UPDATE config SET value = '" + String(inc_use_secure_2) + "' WHERE variable = 'use_secure_2'";
      int config_use_secure_2_rc = dbExec(fs_db, config_use_secure_2.c_str());
      if (config_use_secure_2_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_data_reverse_1 = "UPDATE config SET value = '" + String(inc_data_reverse_1) + "' WHERE variable = 'data_reverse_1'";
      int config_data_reverse_1_rc = dbExec(fs_db, config_data_reverse_1.c_str());
      if (config_data_reverse_1_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_data_reverse_2 = "UPDATE config SET value = '" + String(inc_data_reverse_2) + "' WHERE variable = 'data_reverse_2'";
      int config_data_reverse_2_rc = dbExec(fs_db, config_data_reverse_2.c_str());
      if (config_data_reverse_2_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_silent_mode_1 = "UPDATE config SET value = '" + String(inc_silent_mode_1) + "' WHERE variable = 'silent_mode_1'";
      int config_silent_mode_1_rc = dbExec(fs_db, config_silent_mode_1.c_str());
      if (config_silent_mode_1_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_silent_mode_2 = "UPDATE config SET value = '" + String(inc_silent_mode_2) + "' WHERE variable = 'silent_mode_2'";
      int config_silent_mode_2_rc = dbExec(fs_db, config_silent_mode_2.c_str());
      if (config_silent_mode_2_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_lock_1_reader_timeout = "UPDATE config SET value = '" + String(inc_lock_1_reader_timeout) + "' WHERE variable = 'lock_1_reader_timeout'";
      int config_lock_1_reader_timeout_rc = dbExec(fs_db, config_lock_1_reader_timeout.c_str());
      if (config_lock_1_reader_timeout_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_lock_2_reader_timeout = "UPDATE config SET value = '" + String(inc_lock_2_reader_timeout) + "' WHERE variable = 'lock_2_reader_timeout'";
      int config_lock_2_reader_timeout_rc = dbExec(fs_db, config_lock_2_reader_timeout.c_str());
      if (config_lock_2_reader_timeout_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_door_sensor_1 = "UPDATE config SET value = '" + String(inc_door_sensor_1) + "' WHERE variable = 'door_sensor_1'";
      int config_door_sensor_1_rc = dbExec(fs_db, config_door_sensor_1.c_str());
      if (config_door_sensor_1_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_door_sensor_2 = "UPDATE config SET value = '" + String(inc_door_sensor_2) + "' WHERE variable = 'door_sensor_2'";
      int config_door_sensor_2_rc = dbExec(fs_db, config_door_sensor_2.c_str());
      if (config_door_sensor_2_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_lock_1_button_timeout = "UPDATE config SET value = '" + String(inc_lock_1_button_timeout) + "' WHERE variable = 'lock_1_button_timeout'";
      int config_lock_1_button_timeout_rc = dbExec(fs_db, config_lock_1_button_timeout.c_str());
      if (config_lock_1_button_timeout_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_lock_2_button_timeout = "UPDATE config SET value = '" + String(inc_lock_2_button_timeout) + "' WHERE variable = 'lock_2_button_timeout'";
      int config_lock_2_button_timeout_rc = dbExec(fs_db, config_lock_2_button_timeout.c_str());
      if (config_lock_2_button_timeout_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_wsio_out_log_reader = "UPDATE config SET value = '" + inc_wsio_out_log_reader + "' WHERE variable = 'wsio_out_log_reader'";
      int config_wsio_out_log_reader_rc = dbExec(fs_db, config_wsio_out_log_reader.c_str());
      if (config_wsio_out_log_reader_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_wsio_out_log_button = "UPDATE config SET value = '" + inc_wsio_out_log_button + "' WHERE variable = 'wsio_out_log_button'";
      int config_wsio_out_log_button_rc = dbExec(fs_db, config_wsio_out_log_button.c_str());
      if (config_wsio_out_log_button_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_wsio_out_log_door = "UPDATE config SET value = '" + inc_wsio_out_log_door + "' WHERE variable = 'wsio_out_log_door'";
      int config_wsio_out_log_door_rc = dbExec(fs_db, config_wsio_out_log_door.c_str());
      if (config_wsio_out_log_door_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_wsio_out_log_rtc = "UPDATE config SET value = '" + inc_wsio_out_log_rtc + "' WHERE variable = 'wsio_out_log_rtc'";
      int config_wsio_out_log_rtc_rc = dbExec(fs_db, config_wsio_out_log_rtc.c_str());
      if (config_wsio_out_log_rtc_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      String config_is_setup_done = "UPDATE config SET value = '1' WHERE variable = 'is_setup_done'";
      int config_is_setup_done_rc = dbExec(fs_db, config_is_setup_done.c_str());
      if (config_is_setup_done_rc != SQLITE_OK) {
        sqlite3_close(fs_db);
        return;
      }
      delay(500);
      sqlite3_close(fs_db);
      delay(500);
      DynamicJsonDocument doc(512);
      doc["status"] = "success";
      doc["message"] = "Device setup completed - Restarting";
      String buf;
      serializeJson(doc, buf);
      webServer.send(200, "application/json", buf);
      delay(2500);
      ESP.restart();
    } else {
      DynamicJsonDocument doc(512);
      doc["status"] = "error";
      doc["message"] = "Mandatory attribute missing: check outgoing data";
      String buf;
      serializeJson(doc, buf);
      webServer.send(403, "application/json", buf);
    }
  }
}

void handleWebServerNotFound() {
  webServer.send(404, "text/plain", "404: Not found");
}

void setupWebServer() {
  webServer.on("/", HTTP_GET, handleWebServerRootGet);
  webServer.on("/", HTTP_POST, handleWebServerRootPost);
  webServer.onNotFound(handleWebServerNotFound);
  webServer.begin();
}

void handleEthernetStart() {
  String mac_address = ETH.macAddress();
  mac_address_raw = mac_address;
  mac_address.replace(":", "");
  String hostname = MODULE_TYPE + String('-') + mac_address;
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

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) { // TODO
  switch(type) {
    case sIOtype_DISCONNECT:
      wsio_connected = false;
      Serial.println("[WSIO] Disconnected!");
      break;
    case sIOtype_CONNECT:
      updateNtpTime(); // if WSIO is connected, we know that the server is available

      Serial.println("[WSIO] Connected!");

      // TODO - do some negotiation

      socketIO.send(sIOtype_CONNECT, "/"); // join default namespace (no auto join in Socket.IO V3)

      // then mark as connected
      wsio_connected = true;
      break;
    // case sIOtype_EVENT:
    //   { 
    //     // TODO 
    //   }
    //   break;
    case sIOtype_ACK:
      Serial.printf("[WSIO] get ack: %u\n", length);
      break;
    case sIOtype_ERROR:
      Serial.printf("[WSIO] get error: %u\n", length);
      break;
    case sIOtype_BINARY_EVENT:
      Serial.printf("[WSIO] get binary: %u\n", length);
      break;
    case sIOtype_BINARY_ACK:
      Serial.printf("[WSIO] get binary ack: %u\n", length);
      break;
  }
}

void setupWebsockets() {
  socketIO.setReconnectInterval(10000);
  socketIO.beginSSL(backend_server, backend_port, "/socket.io/?EIO=4");
  socketIO.onEvent(socketIOEvent);
}

void dbInsertCardLog(String hex_uid, String timestamp, int initiator_id, int lock_id, int event_id) {
  if (wsio_connected) {
    DynamicJsonDocument json_doc(1024);
    JsonArray json_array = json_doc.to<JsonArray>();
    json_array.add(wsio_out_log_reader);
    JsonObject param_object = json_array.createNestedObject();
    param_object["initiator_id"] = initiator_id;
    param_object["unit_id"] = mac_address_nag;
    param_object["lock_id"] = lock_id;
    param_object["timestamp"] = timestamp;
    param_object["event_id"] = event_id;
    param_object["hex_uid"] = hex_uid;
    String wsio_event_data;
    serializeJson(json_doc, wsio_event_data);
    socketIO.sendEVENT(wsio_event_data);
  } else {
    String sql = "INSERT INTO events_log VALUES ('" + timestamp + "', " + lock_id + ", " + initiator_id + ", " + event_id + ", '" + hex_uid + "')";
    int rc = dbExec(fs_db, sql.c_str());
    if (rc != SQLITE_OK) {
      sqlite3_close(fs_db);
      return;
    }
  }
}

void dbInsertButtonLog(int initiator_id, int lock_id, int event_id) {
  String timestamp = rtc.now().timestamp();
  if(wsio_connected) { 
    DynamicJsonDocument json_doc(1024);
    JsonArray json_array = json_doc.to<JsonArray>();
    json_array.add(wsio_out_log_button);
    JsonObject param_object = json_array.createNestedObject();
    param_object["initiator_id"] = initiator_id;
    param_object["timestamp"] = timestamp;
    param_object["unit_id"] = mac_address_nag;
    param_object["lock_id"] = lock_id;
    param_object["event_id"] = event_id;
    String wsio_event_data;
    serializeJson(json_doc, wsio_event_data);
    socketIO.sendEVENT(wsio_event_data);
  } else {
    if (openDb("/spiffs/database.db", &fs_db))
      return;
    String sql = "INSERT INTO events_log VALUES ('" + timestamp + "', " + lock_id + ", " + initiator_id + ", " + event_id + ", NULL)";
    int rc = dbExec(fs_db, sql.c_str());
    if (rc != SQLITE_OK) {
      sqlite3_close(fs_db);
      return;
    }
    sqlite3_close(fs_db);
  }
}

void dbInsertDoorLog(int initiator_id, int lock_id, int event_id) {
  String timestamp = rtc.now().timestamp();
  if (wsio_connected) {
    DynamicJsonDocument json_doc(1024);
    JsonArray json_array = json_doc.to<JsonArray>();
    json_array.add(wsio_out_log_door);
    JsonObject param_object = json_array.createNestedObject();
    param_object["initiator_id"] = initiator_id;
    param_object["timestamp"] = timestamp;
    param_object["unit_id"] = mac_address_nag;
    param_object["lock_id"] = lock_id;
    param_object["event_id"] = event_id;
    String wsio_event_data;
    serializeJson(json_doc, wsio_event_data);
    socketIO.sendEVENT(wsio_event_data);
  } else {
    if (openDb("/spiffs/database.db", &fs_db))
      return;
    String sql = "INSERT INTO events_log VALUES ('" + timestamp + "', " + lock_id + ", " + initiator_id + ", " + event_id + ", NULL)";
    int rc = dbExec(fs_db, sql.c_str());
    if (rc != SQLITE_OK) {
      sqlite3_close(fs_db);
      return;
    }
    sqlite3_close(fs_db);
  }
}

void findCard(String timestamp, String hex_uid, int initiator_id, int lock_id) {
  if (openDb("/spiffs/database.db", &fs_db))
    return;

  // String count_sql = "SELECT count(*) FROM cards_" + lock_id + " WHERE is_secure = 0 AND card_hex = '" + hex_uid + "'";
  // String count_sql = "SELECT count(*) FROM cards WHERE is_secure = 0 AND card_hex = '" + hex_uid + "'";
  String count_sql = "";
  // count_sql = "SELECT count(*) FROM cards_" + lock_id + " WHERE is_secure = 1 AND card_hex = '" + hex_uid + "'";
  // count_sql = "SELECT count(*) FROM cards WHERE is_secure = 1 AND card_hex = '" + hex_uid + "'";

  switch (lock_id) { // handle multiple lock
    case 1:
      if(use_secure_1) {
        count_sql = "SELECT count(*) FROM cards_" + String(lock_id) + " WHERE secure_hex = '" + hex_uid + "'";
      } else {
        count_sql = "SELECT count(*) FROM cards_" + String(lock_id) + " WHERE card_hex = '" + hex_uid + "'";
      }
      break;
    case 2:
      if(use_secure_2) {
        count_sql = "SELECT count(*) FROM cards_" + String(lock_id) + " WHERE secure_hex = '" + hex_uid + "'";
      } else {
        count_sql = "SELECT count(*) FROM cards_" + String(lock_id) + " WHERE card_hex = '" + hex_uid + "'";
      }
      break;
    default:
      break;
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

    int event_id = getReaderSuccessEventId(initiator_id);
    readerSuccessStatus(initiator_id);
    readerBuzzerOff(initiator_id);
    dbInsertCardLog(hex_uid, timestamp, initiator_id, lock_id, event_id);
  } else {
    int event_id = getReaderErrorEventId(initiator_id);
    readerErrorStatus(initiator_id);
    readerBuzzerOff(initiator_id);
    dbInsertCardLog(hex_uid, timestamp, initiator_id, lock_id, event_id);
  }

  sqlite3_close(fs_db);
}

void wg_stateChanged(bool plugged, const char* initiator_id) {
  int initiator = atoi(initiator_id);
  switch (initiator) {
    case 3: 
      if (plugged) {
        wg_1_1_connected = true;
        readerGreenLedOff(initiator);
        readerRedLedOff(initiator);
        Serial.println("[WG_1_1] Connected");
      } else {
        wg_1_1_connected = false;
        readerGreenLedOn(initiator);
        readerRedLedOn(initiator);
        Serial.println("[WG_1_1] Disconnected");
      }
      break;
    case 5: 
      if (plugged) {
        wg_1_2_connected = true;
        readerGreenLedOff(initiator);
        readerRedLedOff(initiator);
        Serial.println("[WG_1_2] Connected");
      } else {
        wg_1_2_connected = false;
        readerGreenLedOn(initiator);
        readerRedLedOn(initiator);
        Serial.println("[WG_1_2] Disconnected");
      }
      break;
    case 8: 
      if (plugged) {
        wg_2_1_connected = true;
        readerGreenLedOff(initiator);
        readerRedLedOff(initiator);
        Serial.println("[WG_2_1] Connected");
      } else {
        wg_2_1_connected = false;
        readerGreenLedOn(initiator);
        readerRedLedOn(initiator);
        Serial.println("[WG_2_1] Disconnected");
      }
      break;
    case 10:
      if (plugged) {
        wg_2_2_connected = true;
        readerGreenLedOff(initiator);
        readerRedLedOff(initiator);
        Serial.println("[WG_2_2] Connected");
      } else {
        wg_2_2_connected = false;
        readerGreenLedOn(initiator);
        readerRedLedOn(initiator);
        Serial.println("[WG_2_2] Disconnected");
      }
      break;
    default:
      break;
  }
}

void wg_receivedData_Error(Wiegand::DataError error, uint8_t* rawData, uint8_t rawBits, const char* initiator_id) {
  int initiator = atoi(initiator_id);
  readerPanicStatus(initiator);
  // Serial.print(Wiegand::DataErrorStr(error));
  // Serial.print(" - ");
  // Serial.print(rawBits);
  // Serial.println(" bits");
  // uint8_t bytes = (rawBits+7)/8;
  // for (int i=0; i<bytes; i++) {
  //   Serial.print(rawData[i] >> 4, 16);
  //   Serial.print(rawData[i] & 0xF, 16);
  // }
  // Serial.println();
}

void wg_receivedData_Success(uint8_t* data, uint8_t bits, const char* initiator_id) {
  int initiator = atoi(initiator_id);
  readerBuzzerOn(initiator);
  String timestamp = rtc.now().timestamp();
  String incomingData;
  uint8_t bytes = (bits+7)/8;
  int lock_id = getInitiatorLockId(initiator);

  // if (lock_id == 0) { // ? - remove this check?
  //   readerPanicStatus(readerId);
  // } else {
    switch (initiator) {
      case 3: case 5:
        if (data_reverse_1) {
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
        findCard(timestamp, incomingData, initiator, lock_id);
        break;
      case 8: case 10:
        if (data_reverse_2) {
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
        findCard(timestamp, incomingData, initiator, lock_id);
        break;
      default:
        break;
    }
  // }
}

void wg_1_1_Setup() {
  wg_1_1.onReceive(wg_receivedData_Success, "3");
  wg_1_1.onReceiveError(wg_receivedData_Error, "3");
  wg_1_1.onStateChange(wg_stateChanged, "3");
  wg_1_1.begin(Wiegand::LENGTH_ANY, true);
  Serial.println("[WG_1_1] Started");
}

void wg_1_2_Setup() {
  wg_1_2.onReceive(wg_receivedData_Success, "5");
  wg_1_2.onReceiveError(wg_receivedData_Error, "5");
  wg_1_2.onStateChange(wg_stateChanged, "5");
  wg_1_2.begin(Wiegand::LENGTH_ANY, true);
  Serial.println("[WG_1_2] Started");
}

void wg_2_1_Setup() {
  wg_2_1.onReceive(wg_receivedData_Success, "8");
  wg_2_1.onReceiveError(wg_receivedData_Error, "8");
  wg_2_1.onStateChange(wg_stateChanged, "8");
  wg_2_1.begin(Wiegand::LENGTH_ANY, true);
  Serial.println("[WG_2_1] Started");
}

void wg_2_2_Setup() {
  wg_2_2.onReceive(wg_receivedData_Success, "10");
  wg_2_2.onReceiveError(wg_receivedData_Error, "10");
  wg_2_2.onStateChange(wg_stateChanged, "10");
  wg_2_2.begin(Wiegand::LENGTH_ANY, true);
  Serial.println("[WG_2_2] Started");
}

void wg_1_1_Handler() {
  wg_1_1.flush(); // Checks for pending messages
  wg_1_1.setPin0State(digitalRead(ESP_WG_1_1_D0)); // Check for changes on the the wiegand input pins
  wg_1_1.setPin1State(digitalRead(ESP_WG_1_1_D1));
}

void wg_1_2_Handler() {
  wg_1_2.flush(); // Checks for pending messages
  wg_1_2.setPin0State(digitalRead(ESP_WG_1_2_D0)); // Check for changes on the the wiegand input pins
  wg_1_2.setPin1State(digitalRead(ESP_WG_1_2_D1));
}

void wg_2_1_Handler() {
  wg_2_1.flush(); // Checks for pending messages
  wg_2_1.setPin0State(digitalRead(ESP_WG_2_1_D0)); // Check for changes on the the wiegand input pins
  wg_2_1.setPin1State(digitalRead(ESP_WG_2_1_D1));
}

void wg_2_2_Handler() {
  wg_2_2.flush(); // Checks for pending messages
  wg_2_2.setPin0State(digitalRead(ESP_WG_2_2_D0)); // Check for changes on the the wiegand input pins
  wg_2_2.setPin1State(digitalRead(ESP_WG_2_2_D1));
}

void wg_1_1_StateHandler() {
  if (wg_1_1_success && (millis() - wg_1_1_last_time >= lock_1_reader_timeout)) {
    closeLock(1);
    readerGreenLedOff(3);
    readerGreenLedOff(5);
    wg_1_1_success = false;
    wg_1_1_halt = false;
    wg_1_2_halt = false;
    button_1_1_halt = false;
    button_1_2_halt = false;
  }
  if (wg_1_1_error && (millis() - wg_1_1_last_time >= wg_error_timeout)) {
    readerRedLedOff(3);
    wg_1_1_error = false;
    wg_1_1_halt = false;
  }
  if (wg_1_1_panic && (millis() - wg_1_1_last_time >= wg_panic_timeout)) {
    readerRedLedOff(3);
    readerGreenLedOff(3);
    wg_1_1_panic = false;
    wg_1_1_halt = false;
  }
}

void wg_1_2_StateHandler() {
  if (wg_1_2_success && (millis() - wg_1_2_last_time >= lock_1_reader_timeout)) {
    closeLock(1);
    readerGreenLedOff(5);
    readerGreenLedOff(3);
    wg_1_2_success = false;
    wg_1_2_halt = false;
    wg_1_1_halt = false;
    button_1_1_halt = false;
    button_1_2_halt = false;
  }
  if (wg_1_2_error && (millis() - wg_1_2_last_time >= wg_error_timeout)) {
    readerRedLedOff(5);
    wg_1_2_error = false;
    wg_1_2_halt = false;
  }
  if (wg_1_2_panic && (millis() - wg_1_2_last_time >= wg_panic_timeout)) {
    readerRedLedOff(5);
    readerGreenLedOff(5);
    wg_1_2_panic = false;
    wg_1_2_halt = false;
  }
}

void wg_2_1_StateHandler() {
  if (wg_2_1_success && (millis() - wg_2_1_last_time >= lock_2_reader_timeout)) {
    closeLock(2);
    readerGreenLedOff(8);
    readerGreenLedOff(10);
    wg_2_1_success = false;
    wg_2_1_halt = false;
    wg_2_2_halt = false;
    button_2_1_halt = false;
    button_2_2_halt = false;
  }
  if (wg_2_1_error && (millis() - wg_2_1_last_time >= wg_error_timeout)) {
    readerRedLedOff(8);
    wg_2_1_error = false;
    wg_2_1_halt = false;
  }
  if (wg_2_1_panic && (millis() - wg_2_1_last_time >= wg_panic_timeout)) {
    readerRedLedOff(8);
    readerGreenLedOff(8);
    wg_2_1_panic = false;
    wg_2_1_halt = false;
  }
}

void wg_2_2_StateHandler() {
  if (wg_2_2_success && (millis() - wg_2_2_last_time >= lock_2_reader_timeout)) {
    closeLock(2);
    readerGreenLedOff(10);
    readerGreenLedOff(8);
    wg_2_2_success = false;
    wg_2_2_halt = false;
    wg_2_1_halt = false;
    button_2_1_halt = false;
    button_2_2_halt = false;
  }
  if (wg_2_2_error && (millis() - wg_2_2_last_time >= wg_error_timeout)) {
    readerRedLedOff(10);
    wg_2_2_error = false;
    wg_2_2_halt = false;
  }
  if (wg_2_2_panic && (millis() - wg_2_2_last_time >= wg_panic_timeout)) {
    readerRedLedOff(10);
    readerGreenLedOff(10);
    wg_2_2_panic = false;
    wg_2_2_halt = false;
  }
}

void door_sensor_1_StateHandler() {
  int door_sensor_1_current_state = mcp_1.digitalRead(MCP_1_DOOR_SENSOR_1);
  if (door_sensor_1_current_state != door_sensor_1_last_state) {
    if (millis() - door_sensor_1_last_time >= door_sensor_debounce_time) {
      if (door_sensor_1_current_state == LOW) {
        dbInsertDoorLog(2, 1, 3); // Opened
      }
      if (door_sensor_1_current_state == HIGH) {
        dbInsertDoorLog(2, 1, 2); // Closed
      }
      door_sensor_1_last_state = door_sensor_1_current_state;
      door_sensor_1_last_time = millis();
    }
  }
}

void door_sensor_2_StateHandler() {
  int door_sensor_2_current_state = mcp_1.digitalRead(MCP_1_DOOR_SENSOR_2);
  if (door_sensor_2_current_state != door_sensor_2_last_state) {
    if (millis() - door_sensor_2_last_time >= door_sensor_debounce_time) {
      if (door_sensor_2_current_state == LOW) {
        dbInsertDoorLog(7, 2, 3); // Opened
      }
      if (door_sensor_2_current_state == HIGH) {
        dbInsertDoorLog(7, 2, 2); // Closed
      }
      door_sensor_2_last_state = door_sensor_2_current_state;
      door_sensor_2_last_time = millis();
    }
  }
}

void button_1_1_PressStateHandler() {
  int button_1_1_current_state = mcp_1.digitalRead(MCP_1_BUTTON_1_1);
  if (button_1_1_current_state != button_1_1_last_state) {
    if (millis() - button_1_1_press_last_time >= button_debounce_time) {
      if (button_1_1_current_state == LOW) {
        buttonSuccessStatus(4);
        dbInsertButtonLog(4, 1, 5);
      }
      button_1_1_last_state = button_1_1_current_state;
      button_1_1_press_last_time = millis();
    }
  }
}

void button_1_2_PressStateHandler() {
  int button_1_2_current_state = mcp_1.digitalRead(MCP_1_BUTTON_1_2);
  if (button_1_2_current_state != button_1_2_last_state) {
    if (millis() - button_1_2_press_last_time >= button_debounce_time) {
      if (button_1_2_current_state == LOW) {
        buttonSuccessStatus(6);
        dbInsertButtonLog(6, 1, 9);
      }
      button_1_2_last_state = button_1_2_current_state;
      button_1_2_press_last_time = millis();
    }
  }
}

void button_2_1_PressStateHandler() {
  int button_2_1_current_state = mcp_1.digitalRead(MCP_1_BUTTON_2_1);
  if (button_2_1_current_state != button_2_1_last_state) {
    if (millis() - button_2_1_press_last_time >= button_debounce_time) {
      if (button_2_1_current_state == LOW) {
        buttonSuccessStatus(9);
        dbInsertButtonLog(9, 2, 5);
      }
      button_2_1_last_state = button_2_1_current_state;
      button_2_1_press_last_time = millis();
    }
  }
}

void button_2_2_PressStateHandler() {
  int button_2_2_current_state = mcp_1.digitalRead(MCP_1_BUTTON_2_2);
  if (button_2_2_current_state != button_2_2_last_state) {
    if (millis() - button_2_2_press_last_time >= button_debounce_time) {
      if (button_2_2_current_state == LOW) {
        buttonSuccessStatus(11);
        dbInsertButtonLog(11, 2, 9);
      }
      button_2_2_last_state = button_2_2_current_state;
      button_2_2_press_last_time = millis();
    }
  }
}

void button_1_1_StateHandler() {
  if (button_1_1_success && (millis() - button_1_1_last_time >= lock_1_button_timeout)) {
    closeLock(1);
    readerGreenLedOff(3);
    readerGreenLedOff(5);
    button_1_1_success = false;
    button_1_1_halt = false;
    wg_1_1_halt = false;
    wg_1_2_halt = false;
    button_1_2_halt = false;
  }
}

void button_1_2_StateHandler() {
  if (button_1_2_success && (millis() - button_1_2_last_time >= lock_1_button_timeout)) {
    closeLock(1);
    readerGreenLedOff(5);
    readerGreenLedOff(3);
    button_1_2_success = false;
    button_1_2_halt = false;
    wg_1_1_halt = false;
    wg_1_2_halt = false;
    button_1_2_halt = false;
  }
}

void button_2_1_StateHandler() {
  if (button_2_1_success && (millis() - button_2_1_last_time >= lock_2_button_timeout)) {
    closeLock(2);
    readerGreenLedOff(8);
    readerGreenLedOff(10);
    button_2_1_success = false;
    button_2_1_halt = false;
    wg_2_1_halt = false;
    wg_2_2_halt = false;
    button_2_2_halt = false;
  }
}

void button_2_2_StateHandler() {
  if (button_2_2_success && (millis() - button_2_2_last_time >= lock_2_button_timeout)) {
    closeLock(2);
    readerGreenLedOff(10);
    readerGreenLedOff(8);
    button_2_2_success = false;
    button_2_2_halt = false;
    wg_2_1_halt = false;
    wg_2_2_halt = false;
    button_2_2_halt = false;
  }
}

void core0Task( void * parameter ) {
  Serial.print("[CORE_");
  Serial.print(xPortGetCoreID());
  Serial.println("] Task Started - Core ");
  for( ;; ){
    if (is_setup_done) {
      if (eth_connected) socketIO.loop();
    } else {
      if (eth_connected) webServer.handleClient();
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("----------------------------------------------------------------------------------------------------------------");
  Serial.println("[Module Unit Name] " + String(MODULE_NAME) + " (" + String(MODULE_TYPE) + ")");
  Serial.println("[Hardware Version] " + String(MODULE_HARDWARE_VERSION));
  Serial.println("[Firmware Version] " + String(MODULE_FIRMWARE_VERSION));
  Serial.println("----------------------------------------------------------------------------------------------------------------");
  // ----------------------------------------------------------------------------------------------------------------
  setupEspPins();
  Wire.setPins(ESP_SDA, ESP_SCL);
  setupMcp1Pins();
  setupMcp2Pins();
  setupRtc();
  sqlite3_initialize();
  setupFileSystem();
  setupEthernet();
  setupWebsockets();
  setupWebServer();
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
  wg_1_1_Setup();
  wg_1_2_Setup();
  wg_2_1_Setup();
  wg_2_2_Setup();
  Serial.println("[BOOT] Completed");
  Serial.println("----------------------------------------------------------------------------------------------------------------");
  // ----------------------------------------------------------------------------------------------------------------
  xTaskCreatePinnedToCore(core0Task, "Core0Task", 10000, NULL, tskIDLE_PRIORITY, NULL, 0);
  // xTaskCreatePinnedToCore(core1Task, "Core1Task", 10000, NULL, 1, NULL, 1);
  // ----------------------------------------------------------------------------------------------------------------
}

void loop() { // This is Core 1
  if (is_setup_done) {
    if (!wg_1_1_connected || (!wg_1_1_halt && wg_1_1_connected)) wg_1_1_Handler();
    if (!wg_1_2_connected || (!wg_1_2_halt && wg_1_2_connected)) wg_1_2_Handler();
    if (!wg_2_1_connected || (!wg_2_1_halt && wg_2_1_connected)) wg_2_1_Handler();
    if (!wg_2_2_connected || (!wg_2_2_halt && wg_2_2_connected)) wg_2_2_Handler();
    if (wg_1_1_halt && wg_1_1_connected) wg_1_1_StateHandler();
    if (wg_1_2_halt && wg_1_2_connected) wg_1_2_StateHandler();
    if (wg_2_1_halt && wg_2_1_connected) wg_2_1_StateHandler();
    if (wg_2_2_halt && wg_2_2_connected) wg_2_2_StateHandler();
    if (!button_1_1_halt) button_1_1_PressStateHandler();
    if (!button_1_2_halt) button_1_2_PressStateHandler();
    if (!button_2_1_halt) button_2_1_PressStateHandler();
    if (!button_2_2_halt) button_2_2_PressStateHandler();
    if (button_1_1_halt) button_1_1_StateHandler();
    if (button_1_2_halt) button_1_2_StateHandler();
    if (button_2_1_halt) button_2_1_StateHandler();
    if (button_2_2_halt) button_2_2_StateHandler();
    if (door_sensor_1) door_sensor_1_StateHandler();
    if (door_sensor_2) door_sensor_2_StateHandler();
  } else {
    mcp_2.digitalWrite(MCP_2_WG_1_1_GREEN_LED, HIGH); // OFF
    mcp_2.digitalWrite(MCP_2_WG_1_2_GREEN_LED, HIGH); // OFF
    mcp_2.digitalWrite(MCP_2_WG_2_1_GREEN_LED, HIGH); // OFF
    mcp_2.digitalWrite(MCP_2_WG_2_2_GREEN_LED, HIGH); // OFF
    delay(500);
    mcp_2.digitalWrite(MCP_2_WG_1_1_GREEN_LED, LOW); // ON
    mcp_2.digitalWrite(MCP_2_WG_1_2_GREEN_LED, LOW); // ON
    mcp_2.digitalWrite(MCP_2_WG_2_1_GREEN_LED, LOW); // ON
    mcp_2.digitalWrite(MCP_2_WG_2_2_GREEN_LED, LOW); // ON
    delay(500);
  }
}

// ----------------------------------------------------------------------------------------------------------------
