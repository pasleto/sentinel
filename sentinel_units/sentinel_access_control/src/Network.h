#pragma once

#include <ETH.h>
#include <ESPmDNS.h>
#include <NetBIOS.h>
#include <espMqttClient.h>
#include <WiFiClient.h>
#include <WifiClientSecure.h>
#include <Ticker.h>

enum class MqttLockCommand { // TODO
  NA,
  Idle,
  Block,
  Bypass,
  Error,
  PassErrorReader1,
  PassErrorReader2,
  PassSuccessReader1,
  PassSuccessReader2,
};

class Network {
  public:
    Network();
    void init();
    void handle();
    // void publishMqttEventMessage(String state, String initiator, bool has_value, String value, bool has_id, String id, bool has_status, String status, String level, String type);
    void publishMqttEventMessage(String state, String initiator, String value, String id, String status, String level, String type);
    uint16_t publishMqtt(String topic, const char *payload, boolean retained, uint8_t qos);
    uint16_t subscribeMqtt(String topic, uint8_t qos);
    bool mqttConnected();
    bool mqttInit();
    bool mqttLockTrigger();
    MqttLockCommand mqttLockCommand();
    void mqttLockTriggerClear();
    void setMqttInit(bool init);
  private:
    bool _connect();
    bool _disconnect();
    void _update();
    void _publishBirth();
    void _publishInit();
    void _handleMqttMessage(String topic, String payload, size_t len, bool retain, bool duplicate);
    void _dbCardHandler(String payload);
    void _lockCommandHandler(String payload);
    void _rebootCommandHandler(String payload); // TODO
    void _syncConfigHandler(String payload); // TODO
    void _dbCardMqttResponse(const char *job_id, const char *action, bool error, const char *message);
    void _onMqttConnect(const bool &sessionPresent);
    void _onMqttDisconnect(const espMqttClientTypes::DisconnectReason& reason);
    const char* _mqttDisconnectReason(const espMqttClientTypes::DisconnectReason& reason);
    void _onMqttMessage(const espMqttClientTypes::MessageProperties& properties, const char* topic, byte* payload, size_t len, size_t index, size_t total);
    void _onMqttSubscribe(uint16_t &packetId, const espMqttClientTypes::SubscribeReturncode* codes, size_t len);
    void _onMqttUnsubscribe(uint16_t &packetId);
    void _onMqttPublish(uint16_t &packetId);
    Ticker _ntp_ticker;
    espMqttClient* _mqtt = nullptr;
    espMqttClientSecure* _mqttSecure = nullptr;
    bool _eth_connected;
    String _hostname;
    bool _ip_static;
    String _ip_static_ip;
    String _ip_static_mask;
    String _ip_static_gw;
    String _ip_static_dns;
    String _ntp_server;
    String _timezone;
    bool _ssl_use;
    String _ssl_root_ca;
    bool _ssl_validate_client; // TODO
    String _ssl_client_certificate; // TODO
    String _ssl_client_private_key; // TODO
    String _mqtt_server;
    int _mqtt_port;
    bool _mqtt_credentials;
    String _mqtt_user;
    String _mqtt_pass;
    int _mqtt_check_period;
    unsigned long _mqtt_check_last_time;
    String _topic_base;
    String _client_id;
    bool _mqtt_init_trigger;
    String _lwt_topic;
    String _lwt_payload;
    String _lwt_birth_payload;
    int _lwt_qos;
    bool _mqtt_lock_trigger;
    MqttLockCommand _mqtt_lock_command;
};

extern Network* network;
