#include "Network.h"
#include "Filesystem.h"
#include "Utils.h"
// #include "Rtc.h"
#include "Logger.h"
#include "pins.h"
#include "pref.h"
#include "defaults.h"
#include "config.h"
#include "topics.h"

Network* network = new Network();

Network::Network() {
  pinMode(ESP_LED, OUTPUT);
  digitalWrite(ESP_LED, HIGH); // off
  pinMode(GSM_MODULE_POWER, OUTPUT);
  pinMode(GSM_MODULE_PWR, OUTPUT);
  pinMode(GSM_MODULE_INT, INPUT);
  _mqtt = new PubSubClient();
  _modem = new TinyGsm(Serial1);
  _client = new TinyGsmClient(*_modem); // _client = new TinyGsmClient();
  _ssl_client = new SSLClient(_client);
}

void Network::init() {
  // _is_network_connected = false;
  // _is_gprs_connected = false;
  _hostname = filesystem->preferences()->getString(preference_hostname, utils->defaultHostname());
  // _ntp_server = filesystem->preferences()->getString(preference_ntp_server, DEFAULT_NTP_SERVER); // ?
  // _timezone = filesystem->preferences()->getString(preference_timezone, DEFAULT_TIMEZONE); // ?
  // _timezone_name = filesystem->preferences()->getString(preference_timezone_name, DEFAULT_TIMEZONE_NAME); // ?
  _ssl_use = filesystem->preferences()->getBool(preference_ssl_use, DEFAULT_SSL_USE);
  _ssl_root_ca = filesystem->preferences()->getString(preference_ssl_root_ca, DEFAULT_SSL_CA_CERTIFICATE);
  _ssl_validate_client = filesystem->preferences()->getBool(preference_ssl_validate_client, DEFAULT_SSL_VALIDATE_CLIENT);
  _ssl_client_certificate = filesystem->preferences()->getString(preference_ssl_client_certificate, DEFAULT_SSL_CLIENT_CERTIFICATE);
  _ssl_client_private_key = filesystem->preferences()->getString(preference_ssl_client_private_key, DEFAULT_SSL_CLIENT_PRIVATE_KEY);
  _mqtt_server = filesystem->preferences()->getString(preference_mqtt_server, DEFAULT_MQTT_SERVER);
  _mqtt_port = filesystem->preferences()->getInt(preference_mqtt_port, DEFAULT_MQTT_PORT);
  _mqtt_credentials = filesystem->preferences()->getBool(preference_mqtt_credentials_set, DEFAULT_MQTT_CREDENTIALS);
  _mqtt_user = filesystem->preferences()->getString(preference_mqtt_username, DEFAULT_MQTT_USER);
  _mqtt_pass = filesystem->preferences()->getString(preference_mqtt_password, DEFAULT_MQTT_PASS);
  _topic_base = utils->getTopicBase();
  _client_id = utils->getMacNag();
  _mqtt_check_last_time = 0;
  _mqtt_check_period = 10000;
  // _mqtt_init_trigger = false;
  _gnss_mode = filesystem->preferences()->getInt(preference_gnss_mode, DEFAULT_GNSS_MODE);
  _gprs_apn = filesystem->preferences()->getString(preference_gprs_apn, DEFAULT_GPRS_APN);
  _gprs_user = filesystem->preferences()->getString(preference_gprs_username, DEFAULT_GPRS_USER);
  _gprs_pass = filesystem->preferences()->getString(preference_gprs_password, DEFAULT_GPRS_PASS);
  _gps_last_lat = filesystem->preferences()->getFloat(preference_last_lat, 0.0);
  _gps_last_lon = filesystem->preferences()->getFloat(preference_last_lon, 0.0);
  _gps_last_alt = filesystem->preferences()->getFloat(preference_last_alt, 0.0);
  _gps_last_speed = filesystem->preferences()->getFloat(preference_last_speed, 0.0);
  _gps_last_accuracy = filesystem->preferences()->getFloat(preference_last_accuracy, 0.0);
  _gps_last_datetime = DateTime(filesystem->preferences()->getString(preference_last_datetime, DateTime(2000,1,1,0,0,0).timestamp()).c_str());
  _gps_last_time = 0;
  _gps_period = 5000;

  // _client->init(_modem);

  if (_ssl_use) {
    _ssl_client->setCACert(_ssl_root_ca.c_str());
    if (_ssl_validate_client) {
      _ssl_client->setCertificate(_ssl_client_certificate.c_str());
      _ssl_client->setPrivateKey(_ssl_client_private_key.c_str());
    }
    _mqtt->setClient(*_ssl_client);
  } else {
    _mqtt->setClient(*_client);
  }
  _mqtt->setServer(_mqtt_server.c_str(), _mqtt_port);
  _mqtt->setCallback([&](char* topic, byte* payload, unsigned int length) { // ? - cleanup
      Log->print(F("[MQTT] Message arrived (")); // ?
      Log->print(topic); // ?
      Log->println(F(")")); // ?
      for (int i = 0; i < length; i++) { // ?
        Log->print((char)payload[i]); // ?
      } // ?
      Log->println(); // ?
    String topic_to_process = topic;
    payload[length] = '\0';
    String payload_to_process = (char*)payload;
    _handleMqttMessage(topic_to_process, payload_to_process);
  });
  _mqtt->setBufferSize(12288); // 8192

  _setupModem();
}

Ticker Network::getLedTicker() {
  return _led_ticker;
}

void Network::_connect() { // TODO
  if (_mqtt->connect(_client_id.c_str(), _mqtt_user.c_str(), _mqtt_pass.c_str(), (_topic_base + "/status").c_str(), 0, true, MQTT_PAYLOAD_AVAILABILITY_LWT, MQTT_CLEAN_SESSION)) {
    Log->println(F("[MQTT] Connected"));
    _mqtt_check_last_time = 0;
    _publishBirth();
    _publishInit();
    subscribeMqtt("/status");
    // TODO
  } else {
    Log->print(F("[MQTT] Failed to connect ("));
    Log->print(_mqtt->state());
    Log->println(F(")"));
  }
}

bool Network::mqttConnected() {
  return _mqtt->connected();
}

void Network::_handleMqttMessage(String topic, String payload) {
  if (topic == (_topic_base + "/status")) {
    if (payload != MQTT_PAYLOAD_AVAILABILITY_BIRTH) { // if lwt incoming with status != online by accident, rebroadcast online status
      _publishBirth();
    }
  }
  // TODO
}

void Network::subscribeMqtt(String topic/*, int qos*/) {
  _mqtt->subscribe((_topic_base + topic).c_str()/*, qos*/);
}

void Network::_publishBirth() {
  if (!publishMqtt("/status", MQTT_PAYLOAD_AVAILABILITY_BIRTH, true)) {
    Log->println(F("[MQTT] Failed to publish birth message"));
  }
}

void Network::_publishInit() {
  publishMqtt("/storage", utils->filesystemUsageJson().c_str(), true);
  publishMqtt("/boot", String(filesystem->preferences()->getInt(preference_last_boot_time)).c_str(), true);
  // _mqtt_init_trigger = true;
}

bool Network::publishMqtt(String topic, const char *payload, boolean retained) {
  if (_mqtt->connected()) {
    return _mqtt->publish((_topic_base + topic).c_str(), payload, retained);
  } else {
    return false;
  }
}

void Network::publishRealtimeGpsData(String data_out) { // TODO
  if (_mqtt->connected()) {
    if (!publishMqtt("/realtime", data_out.c_str(), true)) {
      return;
    }
  } else {
    // TODO
    Log->println(F("[DEBUG] mqtt not connected or failed to publish realtime data"));
    Log->println(data_out.c_str());
  }
  // ? - if gps_signal == true -> maybe save into database last known position
  // if (!publishMqtt("/realtime", data_out.c_str(), true)) {
  //   // TODO
  //   Log->println(F("[DEBUG] mqtt not connected or failed to publish realtime data"));
  //   Log->println(data_out.c_str());
  // }
}

String Network::getDataJson(bool gps_signal) { // TODO
  DynamicJsonDocument json_doc(1024);
  json_doc["time"] = rtc->getUnixtime();
  JsonObject temp_data = json_doc.createNestedObject("temp");
  temp_data["int"] = utils->round2(rtc->getTemperature());
  // temp_data["ext"] = round2(20); // ?
  json_doc["gps_signal"] = gps_signal;
  JsonObject gps_data = json_doc.createNestedObject("gps");
  gps_data["lat"] = _gps_last_lat; // gps_data["lat"] = serialized(String(gps_last_lat, 6));
  gps_data["lon"] = _gps_last_lon; // gps_data["lon"] = serialized(String(gps_last_lon, 6));
  gps_data["alt"] = utils->round2(_gps_last_alt); // gps_data["alt"] = serialized(String(gps_last_alt, 2));
  gps_data["speed"] = utils->round2(utils->nonNegative(_gps_last_speed)); // gps_data["speed"] = serialized(String(gps_last_speed, 2));
  gps_data["accuracy"] = utils->round2(_gps_last_accuracy); // gps_data["accuracy"] = serialized(String(gps_last_accuracy, 2));
  gps_data["time"] = _gps_last_datetime.unixtime();
  // JsonObject car_data = json_doc.createNestedObject("car");
  // car_data["val1"] = "TODO";
  // car_data["val2"] = "TODO";
  // car_data["val3"] = "TODO";
  String data_out;
  serializeJson(json_doc, data_out);
  return data_out;
}

void Network::getGpsPosition() { // TODO
  int year, month, day, hour, minute, second;
  if (_modem->getGPS(&_gps_last_lat, &_gps_last_lon, &_gps_last_speed, &_gps_last_alt, NULL, NULL, &_gps_last_accuracy, &year, &month, &day, &hour, &minute, &second)) {
    _gps_last_datetime = DateTime(year, month, day, hour, minute, second);
    filesystem->preferences()->putFloat(preference_last_lat, _gps_last_lat);
    filesystem->preferences()->putFloat(preference_last_lon, _gps_last_lon);
    filesystem->preferences()->putFloat(preference_last_alt, _gps_last_alt);
    filesystem->preferences()->putFloat(preference_last_speed, _gps_last_speed);
    filesystem->preferences()->putFloat(preference_last_accuracy, _gps_last_accuracy);
    filesystem->preferences()->putString(preference_last_datetime, _gps_last_datetime.timestamp());
    rtc->adjust(_gps_last_datetime); // ?
    publishRealtimeGpsData(getDataJson(true));
    // TODO - publish car monitor data
  } else {
    publishRealtimeGpsData(getDataJson(false));
    // TODO - publish car monitor data
  }
}

void Network::ledSetupSequence() {
  _led_ticker.detach();
  _led_ticker.attach_ms(200, []() {
    digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    digitalWrite(ESP_PWR_LED, !digitalRead(ESP_PWR_LED)); // TODO
  });
}

void Network::ledPrebootSequence() {
  _led_ticker.detach();
  _led_ticker.attach_ms(500, []() {
    digitalWrite(ESP_LED, !digitalRead(ESP_LED));
  });
}

void Network::ledBootDoneSequence() {
  _led_ticker.detach();
  digitalWrite(ESP_LED, LOW); // module LED -> ON
}

// void Network::clearWifi() {
//   WiFi.persistent(false);
//   WiFi.disconnect(true);
//   WiFi.softAPdisconnect(true);
//   WiFi.mode(WIFI_OFF);
// }

void Network::handle() {
  if (!_mqtt->connected()) {
    if (millis() - _mqtt_check_last_time >= _mqtt_check_period) {
      _mqtt_check_last_time = millis();
      _connect();
    }
  } else {
    _mqtt->loop();
  }

  // _is_network_connected = _modem->isNetworkConnected();
  // _is_gprs_connected = _modem->isGprsConnected();
  // if (_modem->isNetworkConnected()) {
  //   if (_modem->isGprsConnected()) { // gprs connected
  //     if (!_mqtt->connected()) {
  //       if (millis() - _mqtt_check_last_time >= _mqtt_check_period) { // TODO - redo this -> trigger immediately, then wait
  //         _mqtt_check_last_time = millis();
  //         _connect();
  //       }
  //     } else {
  //       _mqtt->loop();
  //     }
  //   } else { // gprs disconnected
  //     // TODO
  //     Log->println("[DEBUG] GPRS Disconnected");
  //       Log->print("[DEBUG] NETWORK: ");
  //       Log->print(_modem->isNetworkConnected() ? "connected" : "not connected");
  //       Log->print(" | GPRS: ");
  //       Log->print(_modem->isGprsConnected() ? "connected" : "not connected");
  //       Log->print(" | MQTT: ");
  //       Log->println(_mqtt->connected() ? "connected" : "not connected");
  //     // _mqtt->disconnect(); // ?
  //   }
  // } else { // network not connected
  //   // TODO
  //   Log->println(F("[DEBUG] NETWORK Disconnected"));
  //     Log->print("[DEBUG] NETWORK: ");
  //     Log->print(_modem->isNetworkConnected() ? "connected" : "not connected");
  //     Log->print(" | GPRS: ");
  //     Log->print(_modem->isGprsConnected() ? "connected" : "not connected");
  //     Log->print(" | MQTT: ");
  //     Log->println(_mqtt->connected() ? "connected" : "not connected");
  //   // _mqtt->disconnect(); // ?
  // }

  // _mqtt->loop();
  // _modem->maintain();
}

bool Network::sendSMS(String recipient, String message) {
  return _modem->sendSMS(recipient, message);
}

void Network::gpsHanle() {
  if (millis() - _gps_last_time >= _gps_period) {
    _gps_last_time = millis();
    getGpsPosition();
  }
}

void Network::_setupModem() {
  digitalWrite(GSM_MODULE_POWER, HIGH); // POWER_PIN: This pin controls the power supply of the SIM7600
  digitalWrite(GSM_MODULE_PWR, HIGH); // PWR_PIN：This Pin is the PWR-KEY of the SIM7600, The time of active low level impulse of PWRKEY pin to power on module , type 500 ms
  delay(500);
  digitalWrite(GSM_MODULE_PWR, LOW);
  attachInterrupt(GSM_MODULE_INT, []() { // If SIM7600 starts normally, then set the onboard LED to flash once every 1 second
    detachInterrupt(GSM_MODULE_INT);
    // _led_ticker.attach_ms(1000, []() { // ?
    //   digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    // });
  }, RISING);

  delay(3000); // ?

  Serial1.begin(GSM_MODULE_UART_BAUD, SERIAL_8N1, GSM_MODULE_RX, GSM_MODULE_TX);

  if (digitalRead(GSM_MODULE_INT) == 0) {
    Log->println(F("[GSM] Waiting For Power On..."));
  } else {
    detachInterrupt(GSM_MODULE_INT);
    // _led_ticker.attach_ms(1000, []() { // ?
    //   digitalWrite(ESP_LED, !digitalRead(ESP_LED));
    // });
  }

  while(digitalRead(GSM_MODULE_INT) == 0) {
    delay(1000); // Waiting for power on
  }

    _led_ticker.attach_ms(1000, []() { // ?
      digitalWrite(ESP_LED, !digitalRead(ESP_LED)); // ?
    }); // ?

  Log->print(F("[GSM] Initializing modem: "));
  if (_modem->init()) {
    Log->println(F("OK"));
  } else {
    Log->println(F("failed"));
  }

  Log->print(F("[GSM] Setting Network Mode: "));
  if (_modem->setNetworkMode(DEFAULT_NETWORK_MODE)) {
    Log->println(F("OK"));
  } else {
    Log->println(F("failed"));
  }

  if ( GSM_PIN && _modem->getSimStatus() != 3 ) {
    Log->print(F("[GSM] Unlocking SIM: "));
    if (_modem->simUnlock(GSM_PIN)) {
      Log->println(F("OK"));
    } else {
      Log->println(F("failed"));
    }
  }

  Log->print(F("[GSM] Setting GNSS Mode: "));
  String gnss_res = _modem->setGNSSMode(_gnss_mode, 1); // String gnss_res = modem.setGNSSMode(gnss_mode, 0);
  if (gnss_res == "OK") {
    Log->println(F("OK"));
  } else {
    Log->println(F("failed"));
  }

  Log->print(F("[GSM] Enabling GPS: "));
  if(_modem->enableGPS()) {
    Log->println(F("OK"));
  } else {
    Log->println(F("failed"));
  }

  String name = _modem->getModemName();
  Log->println("[GSM] Modem Name: " + name);
  String modemInfo = _modem->getModemInfo();
  Log->println("[GSM] Modem Info: " + modemInfo);
  SimStatus simStatus = _modem->getSimStatus();
  Log->println("[GSM] SIM Status: " + String(simStatus));
  int16_t networkMode = _modem->getNetworkMode();
  Log->println("[GSM] Network Mode: " + String(networkMode));
  RegStatus registrationStatus = _modem->getRegistrationStatus();
  Log->println("[GSM] Registration Status: " + String(registrationStatus));
  uint8_t gnssMode = _modem->getGNSSMode();
  Log->println("[GSM] GNSS Mode: " + String(gnssMode));
  int16_t csq = _modem->getSignalQuality();
  Log->println("[GSM] Signal quality: " + String(csq));
  String imei = _modem->getIMEI();
  Log->println("[GSM] IMEI: " + imei);
  String cop = _modem->getOperator();
  Log->println("[GSM] Operator: " + cop);

  if (!filesystem->hwRebootRequest()) { // TODO
    Log->print(F("[GSM] Waiting for network: "));
    if (_modem->waitForNetwork()) {
      Log->println(F("OK"));
    } else {
      delay(10000); // TODO
      Log->println(F("failed"));
      return; // TODO
    }

    // _is_network_connected = _modem->isNetworkConnected();
    Log->print(F("[GSM] Network status: "));
    Log->println(_modem->isNetworkConnected() ? "connected" : "not connected");

    // modem.NTPServerSync(ntp_server, 0); // ?
    // _modem->NTPServerSync("pool.ntp.org", 0); // ?

    Log->print("[GSM] Starting GPRS (apn: " + _gprs_apn);
    Log->print(", user: " + _gprs_user);
    Log->print(", pass: " + _gprs_pass);
    Log->print("): ");
    if (_modem->gprsConnect(_gprs_apn.c_str(), _gprs_user.c_str(), _gprs_pass.c_str())) {
      Log->println(F("OK"));
    } else {
      delay(10000); // TODO
      Log->println(F("failed"));
      return; // TODO
    }

    // _is_gprs_connected = _modem->isGprsConnected();
    Log->print(F("[GSM] GPRS status: "));
    Log->println(_modem->isGprsConnected() ? "connected" : "not connected");

    IPAddress local = _modem->localIP();
    Log->println("[GSM] Local IP: " + String(local));
    String imsi = _modem->getIMSI();
    Log->println("[GSM] IMSI: " + imsi);
    String ccid = _modem->getSimCCID();
    Log->println("[GSM] CCID: " + ccid);

    // sendSMS("+420xxxxxxxxx", String("Hello from ") + imei);

    // String time = _modem->getGSMDateTime(DATE_FULL);
    // Log->println("[DEBUG] Current Network Time: " + time);
  }
}

