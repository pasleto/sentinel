#pragma once

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

class Web {
  public:
    Web();
    void init(bool startCaptive);
    void handleCaptive();
    bool configSaveDoneTrigger();
    void setConfigSaveDoneTrigger(bool value);
    bool configExitTrigger();
    void setConfigExitTrigger(bool value);
    void wsClientSend(char *message); // !
    void disable();
    AsyncWebSocket* websocket();
    void sendRebootStatusToAll();
  private:
    void _initCaptive();
    void _destroyCaptive();
    bool _ON_ETH(IPAddress ipaddress);
    bool _ON_CAPTIVE(IPAddress ipaddress);
    void _sendConnectionType(AsyncWebSocketClient *client);
    void _sendRebootStatus(AsyncWebSocketClient *client);
    AsyncWebServer* _server = nullptr;
    DNSServer* _dns_server = nullptr;
    AsyncWebSocket* _ws = nullptr;
    String _hostname;
    String _password;
    String _username;
    const char* _realm;
    bool _is_digest;
    IPAddress _ap_ip;
    IPAddress _ap_mask;
    uint32_t _ws_client_id;
    bool _config_save_done_trigger;
    bool _config_exit_trigger;
    void _initWebSocket();
    void _handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
    void _handleWebSocketMessageData(DynamicJsonDocument json, AsyncWebSocketClient *client);
    bool _client_connected;
    String _socket_data; // ? - how to replace with char
};

extern Web* web;
