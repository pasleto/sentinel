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
  private:
    void _initCaptive();
    AsyncWebServer* _server = nullptr;
    String _password;
    // String _username;
    // const char* _realm;
    // bool _is_digest;
    DNSServer* _dns_server = nullptr;
    String _hostname;
    IPAddress _ap_ip;
    IPAddress _ap_mask;
};

extern Web* web;
