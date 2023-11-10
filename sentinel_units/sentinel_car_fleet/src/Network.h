#pragma once

#include <Arduino.h>
#include "config.h"
#include <WiFi.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SSLClient.h>
#include <Ticker.h>
#include "Rtc.h"

class Network {
  public:
    Network();
    void init();
    void handle();
    // void clearWifi();
    void gpsHanle();
    Ticker getLedTicker();
    void ledSetupSequence();
    void ledPrebootSequence();
    void ledBootDoneSequence();
    bool publishMqtt(String topic, const char *payload, boolean retained);
    void subscribeMqtt(String topic);
    bool mqttConnected();
    String getDataJson(bool gps_signal);
    void getGpsPosition();
    void publishRealtimeGpsData(String data_out);
    bool sendSMS(String recipient, String message);
  private:
    void _setupModem();
    void _connect();
    void _publishBirth();
    void _publishInit();
    void _handleMqttMessage(String topic, String payload);
    Ticker _led_ticker;
    // bool _is_network_connected;
    // bool _is_gprs_connected;
    String _hostname;
    // String _ntp_server; // ?
    // String _timezone; // ?
    // String _timezone_name; // ?
    bool _ssl_use;
    String _ssl_root_ca;
    bool _ssl_validate_client; // TODO
    String _ssl_client_certificate; // TODO
    String _ssl_client_private_key; // TODO
    int _mqtt_check_period;
    unsigned long _mqtt_check_last_time;
    String _mqtt_server;
    int _mqtt_port;
    bool _mqtt_credentials;
    String _mqtt_user;
    String _mqtt_pass;
    String _topic_base;
    String _client_id;
    // bool _mqtt_init_trigger;
    unsigned long _gps_last_time;
    float _gps_last_lat;
    float _gps_last_lon;
    float _gps_last_alt;
    float _gps_last_speed;
    float _gps_last_accuracy;
    DateTime _gps_last_datetime;
    int _gps_period;
    int _gnss_mode;
    String _gprs_apn;
    String _gprs_user;
    String _gprs_pass;
    TinyGsm* _modem;
    TinyGsmClient* _client;
    SSLClient* _ssl_client;
    PubSubClient* _mqtt;
};

extern Network* network;
