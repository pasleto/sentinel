#pragma once

#include <Preferences.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

class Web {
  public:
    Web();
    void init(bool startCaptive);
    void handleCaptive();
    bool configSaveDoneTrigger();
    void setConfigSaveDoneTrigger(bool value);
  private:
    void _initCaptive();
    bool _ON_ETH(AsyncWebServerRequest *request);
    bool _ON_CAPTIVE(AsyncWebServerRequest *request);
    AsyncWebServer* _server = nullptr;
    String _password;
    String _username;
    const char* _realm;
    bool _is_digest;
    DNSServer* _dns_server = nullptr;
    String _hostname;
    IPAddress _ap_ip;
    IPAddress _ap_mask;
    bool _config_save_done_trigger;
};

extern Web* web;
