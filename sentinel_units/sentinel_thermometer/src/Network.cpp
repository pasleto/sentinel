#include "Network.h"
#include "Filesystem.h"
#include "Utils.h"
#include "Rtc.h"
#include "Logger.h"
#include "pins.h"
#include "pref.h"
#include "defaults.h"
#include "config.h"
#include "topics.h"

#define EMC_ALLOW_NOT_CONNECTED_PUBLISH 0 // ?
#define EMC_RX_BUFFER_SIZE 12288 // ?
#define EMC_TX_BUFFER_SIZE 12288 // ?
// ? - EMC_RX_BUFFER_SIZE 1440
// ? - EMC_TX_BUFFER_SIZE 1440
// ? - EMC_MIN_FREE_MEMORY 4096
// ? - EMC_ALLOW_NOT_CONNECTED_PUBLISH 1
// ? - EMC_TASK_STACK_SIZE 5120

Network* network = new Network();

Network::Network() {}

void Network::init() {
  _eth_connected = false;
  _hostname = filesystem->preferences()->getString(preference_hostname, utils->defaultHostname());
  _ip_static = filesystem->preferences()->getBool(preference_ip_static, DEFAULT_DHCP_STATIC);
  _ip_static_ip = filesystem->preferences()->getString(preference_ip_static_ip, DEFAULT_DHCP_IP);
  _ip_static_mask = filesystem->preferences()->getString(preference_ip_static_mask, DEFAULT_DHCP_MASK);
  _ip_static_gw = filesystem->preferences()->getString(preference_ip_static_gw, DEFAULT_DHCP_GW);
  _ip_static_dns = filesystem->preferences()->getString(preference_ip_static_dns, DEFAULT_DHCP_DNS);
  _ntp_server = filesystem->preferences()->getString(preference_ntp_server, DEFAULT_NTP_SERVER);
  _timezone = filesystem->preferences()->getString(preference_timezone, DEFAULT_TIMEZONE);
  _timezone_name = filesystem->preferences()->getString(preference_timezone_name, DEFAULT_TIMEZONE_NAME);
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
  _mqtt_init_trigger = false;

  _lwt_topic = _topic_base + mqtt_topic_status;
  _lwt_payload = mqtt_payload_status_lwt;
  _lwt_birth_payload = mqtt_payload_status_birth;
  _lwt_qos = 0;
  _led_network_connected_pin = ESP_LED_NETWORK;
  _led_mqtt_connected_pin = ESP_LED_MQTT;
  pinMode(ESP_LED_NETWORK, OUTPUT);
  pinMode(ESP_LED_MQTT, OUTPUT);
  digitalWrite(ESP_LED_NETWORK, HIGH); // OFF
  digitalWrite(ESP_LED_MQTT, HIGH); // OFF

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
      case ARDUINO_EVENT_ETH_CONNECTED:
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
        digitalWrite(_led_network_connected_pin, LOW); // ON
        // _eth_connected = true;
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
        digitalWrite(_led_mqtt_connected_pin, HIGH); // OFF
        _ntp_ticker.detach();
        digitalWrite(_led_network_connected_pin, HIGH); // OFF
        break;
      case ARDUINO_EVENT_ETH_STOP:
        _eth_connected = false;
        _disconnect();
        digitalWrite(_led_mqtt_connected_pin, HIGH); // OFF
        _ntp_ticker.detach();
        digitalWrite(_led_network_connected_pin, HIGH); // OFF
        break;
      default:
        break;
    }
  });

  ETH.begin(ESP_ETH_PHY_ADDR, ESP_ETH_PHY_POWER, ESP_ETH_PHY_MDC, ESP_ETH_PHY_MDIO, ESP_ETH_PHY_TYPE, ESP_ETH_CLK_MODE);

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
  digitalWrite(_led_mqtt_connected_pin, LOW); // ON
  _publishBirth();
  _publishInit();
  subscribeMqtt(mqtt_topic_status, _lwt_qos);
  // TODO
}

void Network::_onMqttDisconnect(const espMqttClientTypes::DisconnectReason &reason) { // TODO - on disconnect - try to connect -> start reconnect sequence -> redo
  Log->print(F("[MQTT] Disconnected - Reason: "));
  Log->println(_mqttDisconnectReason(reason));
  digitalWrite(_led_mqtt_connected_pin, HIGH); // OFF
}

void Network::_onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, byte* payload, size_t len, size_t index, size_t total) {
  String topic_to_process = topic;
  payload[len] = '\0';
  String payload_to_process = (char*)payload;

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
  Log->println(payload_to_process); // ?

  _handleMqttMessage(topic_to_process, payload_to_process, properties.retain, properties.dup); // ?
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

void Network::_handleMqttMessage(String topic, String payload, bool retain, bool duplicate) {
  if (topic == _lwt_topic) { // !
    if (payload != _lwt_birth_payload && !retain /*&& !duplicate*/) { // if lwt incoming with status != online by accident, rebroadcast online status
      _publishBirth();
    }
  }
  // TODO
}

void Network::_publishBirth() {
  // if (!_mqtt.publish((_topic_base + mqtt_topic_status).c_str(), MQTT_PAYLOAD_AVAILABILITY_BIRTH, true)) {
  //   Log->println(F("[MQTT] Failed to publish birth message"));
  // }
  // !
  // if (!publishMqtt(mqtt_topic_status, MQTT_PAYLOAD_AVAILABILITY_BIRTH, true)) {
  //   Log->println(F("[MQTT] Failed to publish birth message"));
  // }
  publishMqtt(mqtt_topic_status, _lwt_birth_payload.c_str(), true, _lwt_qos);
}

void Network::_publishInit() {
  publishMqtt(mqtt_topic_storage, utils->filesystemUsageJson().c_str(), true, /*1*/0);
  publishMqtt(mqtt_topic_boot, String(filesystem->preferences()->getInt(preference_last_boot_time)).c_str(), true, /*1*/0);
  _mqtt_init_trigger = true;
}

bool Network::mqttInit() {
 return _mqtt_init_trigger;
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

void Network::publishMqttEventMessage(String state, String initiator, bool has_measurement, double measurement, bool has_status, String status) { // TODO
  // ? bool for save into db + bool for remove from db + rowid to remove
  if (mqttConnected()) {
    StaticJsonDocument<512> json_doc; // DynamicJsonDocument json_doc(512);
    json_doc["timestamp"] = rtc->getUnixtime();
    json_doc["initiator"] = initiator;
    json_doc["state"] = state;
    if (has_measurement) {
      json_doc["measurement"] = measurement;
    } else {
      json_doc["measurement"] = nullptr;
    }
    if (has_status) {
      json_doc["status"] = status;
    } else {
      json_doc["status"] = nullptr;
    }
    String data_out;
    serializeJson(json_doc, data_out);

    // !
    // if (!publishMqtt("/event", data_out.c_str(), false, 1)) {
    //   return;
    // }
    publishMqtt(mqtt_topic_event, data_out.c_str(), false, /*1*/0);

  } else {
    filesystem->addEvent(state, initiator, has_measurement, measurement, has_status, status);
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
