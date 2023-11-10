#include "Network.h"
#include "Filesystem.h"
#include "Utils.h"
#include "Rtc.h"
#include "Oled.h"
#include "Logger.h"
#include "StatusLed.h"
#include "pins.h"
#include "prefs.h"
#include "defaults.h"
#include "config.h"
#include "topics.h"

#define EMC_ALLOW_NOT_CONNECTED_PUBLISH 0 // ? 1
// ! #define EMC_RX_BUFFER_SIZE 12288 // ? 1440
// ! #define EMC_TX_BUFFER_SIZE 12288 // ? 1440
// ? - EMC_MIN_FREE_MEMORY 16384
// ? - EMC_TASK_STACK_SIZE 5120
// ? - EMC_PAYLOAD_BUFFER_SIZE 32
// ? - EMC_TX_TIMEOUT 5000

Network* network = new Network();

Network::Network() {}

void Network::init() {
  _eth_connected = false;
  _hostname = filesystem->getHostname();
  _ip_static = filesystem->getIpStatic();
  _ip_static_ip = filesystem->getIpStaticIp();
  _ip_static_mask = filesystem->getIpStaticMask();
  _ip_static_gw = filesystem->getIpStaticGateway();
  _ip_static_dns = filesystem->getIpStaticDns();
  _ntp_server = filesystem->getNtpServer();
  _timezone = filesystem->getTimezone();
  _ssl_use = filesystem->getMqttSslUse();
  _ssl_root_ca = filesystem->getMqttRootCertificate();
  _ssl_validate_client = filesystem->getMqttValidateClientCert();
  _ssl_client_certificate = filesystem->getMqttClientCertificate();
  _ssl_client_private_key = filesystem->getMqttClientPrivateKey();
  _mqtt_server = filesystem->getMqttServer();
  _mqtt_port = filesystem->getMqttPort();
  _mqtt_credentials = filesystem->getMqttCredentialsUse();
  _mqtt_user = filesystem->getMqttUsername();
  _mqtt_pass = filesystem->getMqttPassword();

  _topic_base = utils->getTopicBase();
  _client_id = utils->getMacNag();
  _mqtt_check_last_time = 0;
  _mqtt_check_period = 10000;
  _mqtt_init_trigger = false;

  _mqtt_lock_trigger = false; // !
  _mqtt_lock_command = MqttLockCommand::NA; // !

  _lwt_topic = _topic_base + mqtt_topic_status;
  _lwt_payload = mqtt_payload_status_lwt;
  _lwt_birth_payload = mqtt_payload_status_birth;
  _lwt_qos = 0;

  WiFi.onEvent([&](WiFiEvent_t event, WiFiEventInfo_t info){
    switch (event) {
      case ARDUINO_EVENT_ETH_START:
        ETH.setHostname(_hostname.c_str());
        if (!MDNS.begin(_hostname.c_str())) {
          Log->println(F("[MDNS] Error"));
        }
        if (!NBNS.begin(_hostname.c_str())) {
          Log->println(F("[NBNS] Error"));
        }
        break;
      case ARDUINO_EVENT_ETH_GOT_IP:
        Log->print(F("[ETH] Hostname: "));
        Log->print(ETH.getHostname());
        Log->print(F(" | MAC: "));
        Log->print(ETH.macAddress());
        Log->print(F(" | IPv4: "));
        Log->print(ETH.localIP());
        if (ETH.fullDuplex()) Log->print(F(" | FULL_DUPLEX"));
        Log->print(F(" | "));
        Log->print(ETH.linkSpeed());
        Log->println(F("Mbps"));
        statusLed->setNetworkConnected(true);
        filesystem->setIpAddress(ETH.localIP().toString());
        oled->setIpAddress(ETH.localIP().toString());
        configTzTime(_timezone.c_str(), _ntp_server.c_str(), NULL, NULL);
        rtc->updateFromNTP(); // sync NTP immediately
        _ntp_ticker.detach();
        _ntp_ticker.attach(3600, +[](Rtc* _rtc) { // sync NTP every hour
          _rtc->updateFromNTP();
        }, rtc);
        _eth_connected = true;
        break;
      case ARDUINO_EVENT_ETH_DISCONNECTED:
        _eth_connected = false;
        _disconnect();
        statusLed->setMqttConnected(false);
        _ntp_ticker.detach();
        statusLed->setNetworkConnected(false);
        oled->setIpAddress("not-connected");
        break;
      case ARDUINO_EVENT_ETH_STOP:
        _eth_connected = false;
        _disconnect();
        statusLed->setMqttConnected(false);
        _ntp_ticker.detach();
        statusLed->setNetworkConnected(false);
        oled->setIpAddress("not-connected");
        break;
      case ARDUINO_EVENT_ETH_CONNECTED:
      // case ARDUINO_EVENT_ETH_LOST_IP:
      default:
        break;
    }
  });

  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);

  // #if CONFIG_IDF_TARGET_ESP32
  //   ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE); // !
  // #else
  //   ETH.beginSPI(ETH_MISO_PIN, ETH_MOSI_PIN, ETH_SCLK_PIN, ETH_CS_PIN, ETH_RST_PIN, ETH_INT_PIN); // !
  // #endif

  if (_ip_static) {
    IPAddress ip;
    if (ip.fromString(_ip_static_ip)) {
      IPAddress gw;
      if (gw.fromString(_ip_static_gw)) {
        IPAddress mask;
        if (mask.fromString(_ip_static_mask)) {
          IPAddress dns;
          if (dns.fromString(_ip_static_dns)) {
            ETH.config(ip, gw, mask, dns);
          }
        }
      }
    }
  }

  if (_ssl_use) {
    _mqttSecure = new espMqttClientSecure(espMqttClientTypes::UseInternalTask::NO);
    _mqttSecure->setCACert(_ssl_root_ca.c_str());
    if (_ssl_validate_client) {
      _mqttSecure->setCertificate(_ssl_client_certificate.c_str());
      _mqttSecure->setPrivateKey(_ssl_client_private_key.c_str());
    }
    _mqttSecure->setClientId(_client_id.c_str());
    _mqttSecure->setServer(_mqtt_server.c_str(), _mqtt_port);
    if (_mqtt_credentials) {
      _mqttSecure->setCredentials(_mqtt_user.c_str(), _mqtt_pass.c_str());
    }
    _mqttSecure->setWill(_lwt_topic.c_str(), _lwt_qos, true, _lwt_payload.c_str()); // !
    _mqttSecure->setCleanSession(MQTT_CLEAN_SESSION);
    _mqttSecure->onConnect([&](bool sessionPresent) {
      _onMqttConnect(sessionPresent);
    });
    _mqttSecure->onDisconnect([&](espMqttClientTypes::DisconnectReason reason) {
      _onMqttDisconnect(reason);
    });
    _mqttSecure->onMessage([&](const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
      _onMqttMessage(properties, topic, (byte*)payload, len, index, total);
    });
    _mqttSecure->onSubscribe([&](uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len) {
      _onMqttSubscribe(packetId, codes, len);
    });
    _mqttSecure->onUnsubscribe([&](uint16_t packetId) {
      _onMqttUnsubscribe(packetId);
    });
    _mqttSecure->onPublish([&](uint16_t packetId) {
      _onMqttPublish(packetId);
    });
    // _mqttSecure->clearQueue(); // ?
  } else {
    _mqtt = new espMqttClient(espMqttClientTypes::UseInternalTask::NO);
    _mqtt->setClientId(_client_id.c_str());
    _mqtt->setServer(_mqtt_server.c_str(), _mqtt_port);
    if (_mqtt_credentials) {
      _mqtt->setCredentials(_mqtt_user.c_str(), _mqtt_pass.c_str());
    }
    _mqtt->setWill(_lwt_topic.c_str(), _lwt_qos, true, _lwt_payload.c_str()); // !
    _mqtt->setCleanSession(MQTT_CLEAN_SESSION);
    _mqtt->onConnect([&](bool sessionPresent) {
      _onMqttConnect(sessionPresent);
    });
    _mqtt->onDisconnect([&](espMqttClientTypes::DisconnectReason reason) {
      _onMqttDisconnect(reason);
    });
    _mqtt->onMessage([&](const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
      _onMqttMessage(properties, topic, (byte*)payload, len, index, total);
    });
    _mqtt->onSubscribe([&](uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len) {
      _onMqttSubscribe(packetId, codes, len);
    });
    _mqtt->onUnsubscribe([&](uint16_t packetId) {
      _onMqttUnsubscribe(packetId);
    });
    _mqtt->onPublish([&](uint16_t packetId) {
      _onMqttPublish(packetId);
    });
    // _mqtt->clearQueue(); // ?
  }
}

const char* Network::_mqttDisconnectReason(const espMqttClientTypes::DisconnectReason& reason) {
  switch (reason) {
    case espMqttClientTypes::DisconnectReason::USER_OK:                            return "No error";
    case espMqttClientTypes::DisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION: return "Unacceptable protocol version";
    case espMqttClientTypes::DisconnectReason::MQTT_IDENTIFIER_REJECTED:           return "Identified rejected";
    case espMqttClientTypes::DisconnectReason::MQTT_SERVER_UNAVAILABLE:            return "Server unavailable";
    case espMqttClientTypes::DisconnectReason::MQTT_MALFORMED_CREDENTIALS:         return "Malformed credentials";
    case espMqttClientTypes::DisconnectReason::MQTT_NOT_AUTHORIZED:                return "Not authorized";
    case espMqttClientTypes::DisconnectReason::TLS_BAD_FINGERPRINT:                return "Bad fingerprint";
    case espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED:                   return "TCP disconnected";
    default:                                                                       return "Unknown";
  }
}

void Network::_onMqttConnect(const bool &sessionPresent) {
  Log->println(F("[MQTT] Connected"));
  _mqtt_check_last_time = 0;
  statusLed->setMqttConnected(true);
  _publishBirth();
  _publishInit();
  subscribeMqtt(mqtt_topic_status, _lwt_qos);
  subscribeMqtt(mqtt_topic_database_card_request, /*1*/0);
  subscribeMqtt(mqtt_topic_lock_command, /*1*/0);

  subscribeMqtt(mqtt_topic_reboot_command, /*1*/0); // TODO
  subscribeMqtt(mqtt_topic_sync_config, /*1*/0); // TODO

  // mqtt.subscribe((utils->getTopicBase() + "/db/log/request").c_str(), 1); // TODO
  // TODO - subscribe to needed topics
  // TODO - check database, send data to mqtt, remove from database
  // TODO - sync settings
  // TODO - sync card database
}

void Network::_onMqttDisconnect(const espMqttClientTypes::DisconnectReason &reason) { // TODO - on disconnect - try to connect -> start reconnect sequence -> redo
  Log->print(F("[MQTT] Disconnected - Reason: "));
  Log->println(_mqttDisconnectReason(reason));
  statusLed->setMqttConnected(false);
  // _mqtt_check_last_time = 0; // TODO
}

void Network::_onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, byte* payload, size_t len, size_t index, size_t total) { // ?
  // String topic_to_process = topic;
  payload[len] = '\0';

  Log->print("[MQTT] Publish received - topic: "); // ?
  Log->print(topic); // ?
  Log->print(" | qos: "); // ?
  Log->print(properties.qos); // ?
  Log->print(" | dup: "); // ?
  Log->print(properties.dup); // ?
  Log->print(" | retain: "); // ?
  Log->print(properties.retain); // ?
  Log->print(" | len: "); // ?
  Log->print(len); // ?
  Log->print(" | index: "); // ?
  Log->print(index); // ?
  Log->print(" | total: "); // ?
  Log->println(total); // ?
  Log->print("[MQTT] Publish received - payload: "); // ?
  Log->println((char*)payload); // ?

  // ? total vs len -> what if chunked message arrive ???

  _handleMqttMessage(topic/*topic_to_process*/, (char*)payload, len, properties.retain, properties.dup); // ?
  // ! TODO - pass len -> calculate json size
}

void Network::_onMqttSubscribe(uint16_t &packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len) { // ?
  // Log->print("[MQTT] Subscribe acknowledged - packetId: ");
  // Log->println(packetId);
  // for (size_t i = 0; i < len; ++i) {
  //   Log->print("  qos: ");
  //   Log->println(static_cast<uint8_t>(codes[i]));
  // }
}

void Network::_onMqttUnsubscribe(uint16_t &packetId) { // ?
  // Log->print("[MQTT] Unsubscribe acknowledged - packetId: ");
  // Log->println(packetId);
}

void Network::_onMqttPublish(uint16_t &packetId) { // ?
  // Log->print("[MQTT] Publish acknowledged - packetId: ");
  // Log->println(packetId);
}

void Network::_dbCardMqttResponse(const char *job_id, const char *action, bool error, const char *message) { // done // TODO
  const size_t capacity = JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(1); // TODO - get proper size
  StaticJsonDocument<capacity> json_doc; // DynamicJsonDocument json_doc(1024);
  json_doc["timestamp"] = rtc->getUnixtime();
  json_doc["id"] = job_id;
  json_doc["action"] = action;
  if (error) {
    json_doc["status"] = "ERROR";
    json_doc["message"] = message;
  } else {
    json_doc["status"] = "SUCCESS";
    json_doc["message"] = nullptr;
  }
  size_t json_size = measureJson(json_doc);
  char data_out[json_size + 1]; // String data_out;
  serializeJson(json_doc, data_out, json_size + 1); // serializeJson(json_doc, data_out);
  // if (publishMqtt(mqtt_topic_database_card_response, data_out/*.c_str()*/, false, /*1*/0) == 0) { // !
  //   // failed to send mqtt
  // }
  publishMqtt(mqtt_topic_database_card_response, data_out/*.c_str()*/, false, /*1*/0);
}

void Network::_dbCardHandler(String payload) { // TODO - filesystem action response handle
  // action: ['CARD_ADD', 'CARD_REMOVE','CARDS_WIPE'] | CARDS_SYNC
  // job_id: String // ? - id of mongo document from pendingJobs
  // id: String
  // value: String
  // can_enter: bool
  // can_exit: bool
  // has_validity: bool
  // valid_since: int (timestamp)
  // valid_until: int (timestamp)
  // Log->println(payload); // TODO

  StaticJsonDocument<1024> json_doc; // ! TODO - calculate json size from provided len
  DeserializationError json_des_error = deserializeJson(json_doc, payload);
  if (!json_des_error) {
    // if (json_doc.containsKey("job_id")) { // ?
      String job_id = json_doc["job_id"]; // default to - ""
    // }
    if (json_doc.containsKey("action")) {
      String action = json_doc["action"]; // default to - ""
      if (action != "") {
        action.toUpperCase();
        if (action == "CARD_ADD") {

          // ! change check -

          if (json_doc.containsKey("id")) {
            String id = json_doc["id"]; // default to - ""
            id.trim();
            if (id.length() > 0) {
              if (json_doc.containsKey("value")) {
                String value = json_doc["value"]; // default to - ""
                value.trim();
                if (value.length() > 0) {
                  bool json_can_enter = json_doc["can_enter"]; // default to - 0
                  bool json_can_exit = json_doc["can_exit"]; // default to - 0
                  bool has_validity = json_doc["has_validity"]; // default to - 0
                  if (has_validity) {
                    int valid_since = json_doc["valid_since"]; // default to - 0
                    int valid_until = json_doc["valid_until"]; // default to - 0
                    if ((valid_since > 0) && (valid_until > 0) && (valid_until > valid_since)) {
                      const char * res = filesystem->cardAction(job_id, action, "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('" + id + "', '" + value + "', '" + json_can_enter + "', '" + json_can_exit + "', " + has_validity + ", " + valid_since + ", " + valid_until + ");");
                      // TODO - handle res
                    } else {
                      _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "Wrong until / since timestamps provided");
                    }
                  } else {
                    const char * res = filesystem->cardAction(job_id, action, "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('" + id + "', '" + value + "', '" + json_can_enter + "', '" + json_can_exit + "', " + has_validity + ", 946684800, 946684800);"); // NULL NULL
                    // TODO - handle res
                  }
                } else {
                  _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No VALUE provided");
                }
              } else {
                _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No VALUE provided");
              }
            } else {;
              _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No ID provided");
            }
          } else {
            _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No ID provided");
          }
        }
        if (action == "CARD_REMOVE") {
          if (json_doc.containsKey("id")) {
            String id = json_doc["id"]; // default to - ""
            id.trim();
            if (id.length() > 0) {
              if (json_doc.containsKey("value")) {
                String value = json_doc["value"]; // default to - ""
                value.trim();
                if (value.length() > 0) {
                  const char * res = filesystem->cardAction(job_id, action, "DELETE FROM cards WHERE id = '" + id + "' OR value = '" + value + "';");
                  // TODO - handle res
                } else {
                  _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No VALUE provided");
                }
              } else {
                _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No VALUE provided");
              }
            } else {
              _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No ID provided");
            }
          } else {
            _dbCardMqttResponse(job_id.c_str(), action.c_str(), true, "No ID provided");
          }
        }
        if (action == "CARDS_WIPE") { // ! too much pressure on device -> separate task
          filesystem->cardAction(job_id, action, "DELETE FROM cards;");
        }
        if (action == "CARDS_SYNC") { // TODO
          // TODO - this will come from backend when there is something in pending jobs
          // TODO - this should response in /db/card/response with status something like SYNC_READY | SYNC_IN_PROGRESS
          // TODO - make some kind of state machine to make better flow, when multiple SYNC request coming from backend
        }
      } else {
        _dbCardMqttResponse(job_id.c_str(), nullptr, true, "No ACTION provided");
      }
    } else {
      _dbCardMqttResponse(job_id.c_str(), nullptr, true, "No ACTION provided");
    }
  } else {
    _dbCardMqttResponse(nullptr, nullptr, true, json_des_error.c_str());
  }
}

void Network::_lockCommandHandler(String payload) { // !
  // action: ['PASS','BLOCK','BYPASS', 'IDLE]
  // job_id: String // ? - id of mongo document from pendingJobs
  // reader: int // id of reader which was used
  // value: String // SUCCESS, ERROR, ON, OFF
  StaticJsonDocument<1024> json_doc; // ! TODO - calculate json size from provided len
  DeserializationError json_des_error = deserializeJson(json_doc, payload);
  if (!json_des_error) {
    String job_id = json_doc["job_id"]; // default to - "" // ?
    if (json_doc.containsKey("action")) {
      String action = json_doc["action"]; // default to - ""
      if (action != "") {
        action.toUpperCase();
        if (action == "PASS") {
          if (json_doc.containsKey("reader")) {
            int reader = json_doc["reader"]; // default to - 0
            if (reader > 0) {
              if (json_doc.containsKey("value")) {
                String value = json_doc["value"]; // default to - ""
                value.trim();
                if (value.length() > 0) {
                  if (value == "SUCCESS") {
                    switch (reader) {
                      case 1:
                        _mqtt_lock_trigger = true;
                        _mqtt_lock_command = MqttLockCommand::PassSuccessReader1;
                        break;
                      case 2:
                        _mqtt_lock_trigger = true;
                        _mqtt_lock_command = MqttLockCommand::PassSuccessReader2;
                        break;
                    }
                  }
                  if (value == "ERROR") {
                    switch (reader) {
                      case 1:
                        _mqtt_lock_trigger = true;
                        _mqtt_lock_command = MqttLockCommand::PassErrorReader1;
                        break;
                      case 2:
                        _mqtt_lock_trigger = true;
                        _mqtt_lock_command = MqttLockCommand::PassErrorReader2;
                        break;
                    }
                  }
                } else {
                  switch (reader) {
                    case 1:
                      _mqtt_lock_trigger = true;
                      _mqtt_lock_command = MqttLockCommand::PassErrorReader1;
                      break;
                    case 2:
                      _mqtt_lock_trigger = true;
                      _mqtt_lock_command = MqttLockCommand::PassErrorReader2;
                      break;
                  }
                }
              } else {
                switch (reader) {
                  case 1:
                    _mqtt_lock_trigger = true;
                    _mqtt_lock_command = MqttLockCommand::PassErrorReader1;
                    break;
                  case 2:
                    _mqtt_lock_trigger = true;
                    _mqtt_lock_command = MqttLockCommand::PassErrorReader2;
                    break;
                }
              }
            } else {
              _mqtt_lock_trigger = true;
              _mqtt_lock_command = MqttLockCommand::Error;
            }
          } else {
            _mqtt_lock_trigger = true;
            _mqtt_lock_command = MqttLockCommand::Error;
          }
        }
        if (action == "BLOCK") {
          if (json_doc.containsKey("value")) {
            String value = json_doc["value"]; // default to - ""
            value.trim();
            if (value.length() > 0) {
              if (value == "ON") {
                _mqtt_lock_trigger = true;
                _mqtt_lock_command = MqttLockCommand::Block;
              }
              if (value == "OFF") {
                _mqtt_lock_trigger = true;
                _mqtt_lock_command = MqttLockCommand::Idle;
              }
            }
          }
        }
        if (action == "BYPASS") {
          if (json_doc.containsKey("value")) {
            String value = json_doc["value"]; // default to - ""
            value.trim();
            if (value.length() > 0) {
              if (value == "ON") {
                _mqtt_lock_trigger = true;
                _mqtt_lock_command = MqttLockCommand::Bypass;
              }
              if (value == "OFF") {
                _mqtt_lock_trigger = true;
                _mqtt_lock_command = MqttLockCommand::Idle;
              }
            }
          }
        }
        if (action == "IDLE") {
          _mqtt_lock_trigger = true;
          _mqtt_lock_command = MqttLockCommand::Idle;
        }
      } else {
        // TODO - No ACTION provided
      }
    } else {
      // TODO - No ACTION provided
    }
  } else {
    // TODO - deserialize error
  }
}

void Network::_rebootCommandHandler(String payload) { // TODO

}

void Network::_syncConfigHandler(String payload) { // TODO
  DynamicJsonDocument json_doc(12288); // ! TODO - calculate json size from provided len
  DeserializationError json_des_error = deserializeJson(json_doc, payload);
  if (!json_des_error) {
    const char * result = filesystem->handleConfigJson(json_doc);
    if (result == "OK" || result == "EXIT") {
      if (result == "OK") { // TODO
        filesystem->logConfigChange("SUCCESS"); // ? - go through publishMqttEventMessage
        filesystem->logReboot(); // ? - go through publishMqttEventMessage
        utils->printRestart();
        utils->swReboot();
      }
      if (result == "EXIT") { // no changes
        // TODO - do nothing - continue -> maybe rewrite "OK" if case to - if (result != "EXIT")
      }
    } else { // TODO
      String err = "ERROR: ";
      err + result;
      filesystem->logConfigChange(err); // ? - go through publishMqttEventMessage
    }
  } else {
    // TODO - deserialize error
  }
}

void Network::_handleMqttMessage(String topic, String payload, size_t len, bool retain, bool duplicate) {
  if (topic == _lwt_topic) { // !
    if (payload != _lwt_birth_payload && !retain /*&& !duplicate*/) { // if lwt incoming with status != online by accident, rebroadcast online status
      _publishBirth();
    }
  }
  if (topic == (_topic_base + mqtt_topic_database_card_request)) {
    if (payload.length() > 0) {
      _dbCardHandler(payload); // TODO - split into channels ?
    }
  }
  // if (topic == (utils->getTopicBase() + "/db/log/request")) { // TODO
  //   if (payload.length() > 0) {
  //     dbLog_handler(payload);
  //   }
  // }
  // TODO - subcribed topics handlers
  if (topic == (_topic_base + mqtt_topic_lock_command)) {
    if (payload.length() > 0) {
      _lockCommandHandler(payload);
    }
  }

  if (topic == (_topic_base + mqtt_topic_reboot_command)) { // TODO
    if (payload.length() > 0) {
      _rebootCommandHandler(payload);
    }
  }

  if (topic == (_topic_base + mqtt_topic_sync_config)) { // TODO
    if (payload.length() > 0) {
      _syncConfigHandler(payload);
    }
  }
}

void Network::_publishBirth() {
  // if (publishMqtt(mqtt_topic_status, _lwt_birth_payload.c_str(), true, _lwt_qos) == 0) { // !
  //   Log->println(F("[MQTT] Failed to publish birth message"));
  // }
  publishMqtt(mqtt_topic_status, _lwt_birth_payload.c_str(), true, _lwt_qos);
}

void Network::_publishInit() {
  publishMqtt(mqtt_topic_storage_percentage, String(utils->spiffsPercentageUsage()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_storage_total, String(utils->spiffsTotal()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_storage_used, String(utils->spiffsUsed()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_storage_free, String(utils->spiffsFree()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_rtc_warning, utils->rtcBatteryWarning() ? mqtt_payload_rtc_warning : mqtt_payload_rtc_normal, true, /*1*/0);
  publishMqtt(mqtt_topic_reboot_status, filesystem->getRebootNeeded() ? mqtt_payload_reboot_status_needed : mqtt_payload_reboot_status_ok, true, /*1*/0);
  publishMqtt(mqtt_topic_firmware, filesystem->getFirmwareVersion().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_hardware, filesystem->getHardwareVersion().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_boot_unix, String(filesystem->getLastBootTimeUnix()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_boot_timestamp, filesystem->getLastBootTimeTimestamp().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_mode, filesystem->getIpStatic() ? "static" : "dhcp", true, /*1*/0);
  publishMqtt(mqtt_topic_network_hostname, ETH.getHostname(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_ip, ETH.localIP().toString().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_mask, ETH.subnetMask().toString().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_cidr, String(ETH.subnetCIDR()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_gw, ETH.gatewayIP().toString().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_dns, ETH.dnsIP().toString().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_speed, String(ETH.linkSpeed()).c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_network_duplex, ETH.fullDuplex() ? "full" : "half", true, /*1*/0);

  _mqtt_init_trigger = true;
}

bool Network::mqttInit() {
 return _mqtt_init_trigger;
}

bool Network::mqttLockTrigger() {
  return _mqtt_lock_trigger;
}

MqttLockCommand Network::mqttLockCommand() {
  return _mqtt_lock_command;
}

void Network::mqttLockTriggerClear() {
  _mqtt_lock_trigger = false;
  _mqtt_lock_command = MqttLockCommand::NA;
}

void Network::setMqttInit(bool init) {
 _mqtt_init_trigger = init;
}

bool Network::mqttConnected() {
  if(_ssl_use) {
    return _mqttSecure->connected();
  } else {
    return _mqtt->connected();
  }
}

void Network::_update() {
  if(_ssl_use) {
    return _mqttSecure->loop();
  } else {
    return _mqtt->loop();
  }
}

bool Network::_connect() {
  if (_ssl_use) {
    return _mqttSecure->connect();
  } else {
    return _mqtt->connect();
  }
}

bool Network::_disconnect() {
  if (_ssl_use) {
    return _mqttSecure->disconnect();
  } else {
    return _mqtt->disconnect();
  }
}

void Network::handle() {
  if (_eth_connected) {
    _update();
    if (!mqttConnected()) {
      if (millis() - _mqtt_check_last_time >= _mqtt_check_period) {
        _mqtt_check_last_time = millis();
        _connect();
      }
    }
  }
  // _update();
}

// void Network::publishMqttEventMessage(String state, String initiator, bool has_value, String value, bool has_id, String id, bool has_status, String status, String level, String type) { // done // ?
void Network::publishMqttEventMessage(String state, String initiator, String value, String id, String status, String level, String type) { // done // ?
  // ! change check -

  // ? bool for save into db + bool for remove from db + rowid to remove
  if (mqttConnected()) {

    StaticJsonDocument<256> json_doc; // StaticJsonDocument<512> json_doc;
    json_doc["timestamp"] = rtc->getUnixtime();
    json_doc["initiator"] = initiator;
    json_doc["state"] = state;
    // if (has_value) {
    //   json_doc["value"] = value;
    // } else {
    //   json_doc["value"] = nullptr;
    // }
    // if (has_id) {
    //   json_doc["id"] = id;
    // } else {
    //   json_doc["id"] = nullptr;
    // }
    // if (has_status) {
    //   json_doc["status"] = status;
    // } else {
    //   json_doc["status"] = nullptr;
    // }

    if (value == nullptr) {
      json_doc["value"] = nullptr;
    } else {
      json_doc["value"] = value;
    }
    if (id == nullptr) {
      json_doc["id"] = nullptr;
    } else {
      json_doc["id"] = id;
    }
    if (status == nullptr) {
      json_doc["status"] = nullptr;
    } else {
      json_doc["status"] = status;
    }

    // json_doc["value"] = value;
    // json_doc["id"] = id;
    // json_doc["status"] = status;
    json_doc["level"] = level;
    json_doc["type"] = type;
    size_t json_size = measureJson(json_doc);
    char data_out[json_size + 1]; // String data_out;
    serializeJson(json_doc, data_out, json_size + 1); // serializeJson(json_doc, data_out);
    // if (publishMqtt(mqtt_topic_event, data_out/*.c_str()*/, false, /*1*/0) == 0) { // !
    //   return;
    // }
    publishMqtt(mqtt_topic_event, data_out/*.c_str()*/, false, /*1*/0);

    // !!!
    // filesystem->addEvent(state, initiator, has_value, value, has_id, id, has_status, status, level, type); 
    filesystem->addEvent(state, initiator, value, id, status, level, type); 
    // !!!
  } else {
    // filesystem->addEvent(state, initiator, has_value, value, has_id, id, has_status, status, level, type);
    filesystem->addEvent(state, initiator, value, id, status, level, type);
  }
}

uint16_t Network::subscribeMqtt(String topic, uint8_t qos) {
  if (_ssl_use) {
    return _mqttSecure->subscribe((_topic_base + topic).c_str(), qos);
  } else {
    return _mqtt->subscribe((_topic_base + topic).c_str(), qos);
  }
}

uint16_t Network::publishMqtt(String topic, const char *payload, boolean retained, uint8_t qos) {
  if (_ssl_use) {
    return _mqttSecure->publish((_topic_base + topic).c_str(), qos, retained, payload);
  } else {
    return _mqtt->publish((_topic_base + topic).c_str(), qos, retained, payload);
  }
}
