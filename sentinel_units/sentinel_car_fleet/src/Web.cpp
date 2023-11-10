#include "Web.h"
#include "Utils.h"
#include "Filesystem.h"
#include "Logger.h"
#include "webpage.h"
#include "pref.h"
#include "defaults.h"
#include "config.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

Web* web = new Web();

Web::Web() {
  _server = new AsyncWebServer(80);
  _dns_server = new DNSServer();
  // _ap_ip = IPAddress(192,168,11,1);
  // _ap_mask = IPAddress(255,255,255,0);
  _ap_ip = IPAddress(4,3,2,1);
  _ap_mask = IPAddress(255,255,255,240);
}

void Web::init(bool startCaptive) {
  _hostname = filesystem->preferences()->getString(preference_hostname, utils->defaultHostname());
  _password = filesystem->preferences()->getString(preference_device_password, DEFAULT_DEVICE_ADMIN_PASSWORD);
  // _username = filesystem->preferences()->getString(preference_device_username, DEFAULT_DEVICE_ADMIN_USERNAME);
  // _realm = "Login Required";
  // _is_digest = true;

  if (startCaptive) {
    _initCaptive();
  }

  _server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "image/x-icon", favicon, sizeof(favicon));
  }); // public

  _server->on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/css", main_css);
  }); // public

  _server->on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/javascript", main_js);
  }); // public

  _server->addHandler(new AsyncCallbackJsonWebHandler("/config", [&](AsyncWebServerRequest* request, JsonVariant& json) {
    if (not json.is<JsonObject>()) {
      request->send(500, "text/plain", "ERROR: Not an JSON object!");
      return;
    }
    auto&& data = json.as<JsonObject>();
    const char * result = filesystem->handleConfigJson(data);
    if (result != "OK") {
      String err = "ERROR: ";
      err + result;
      request->send(500, "text/plain", err);
      filesystem->logConfigChange(err);
    } else {
      request->send(200, "text/plain", "SUCCESS: Successfully saved! Rebooting!");
      filesystem->logConfigChange("SUCCESS");
      utils->playConfigSaveBeep();
      filesystem->logReboot();
      utils->printRestart();
      utils->swReboot();
    }
  })); // protected

  _server->on("/config", HTTP_GET, [&](AsyncWebServerRequest *request){
    DynamicJsonDocument json_doc(12288); // 8192
    json_doc["config_mode"] = filesystem->preferences()->getBool(preference_config_mode); // ?
    json_doc["was_configured"] = filesystem->preferences()->getBool(preference_was_configured); // ?
    json_doc["module_type"] = MODULE_TYPE;
    json_doc["firmware"] =  filesystem->preferences()->getString(preference_firmware_version); // MODULE_FIRMWARE_VERSION;
    // json_doc["hardware"] = MODULE_HARDWARE_VERSION;
    json_doc["unit_id"] = utils->getMacNag();
    json_doc["mac_address"] = utils->getMac();
    json_doc["hostname"] = filesystem->preferences()->getString(preference_hostname);
    json_doc["mqtt_server"] = filesystem->preferences()->getString(preference_mqtt_server); // mqtt_server;
    json_doc["mqtt_port"] = filesystem->preferences()->getInt(preference_mqtt_port); // mqtt_port;
    json_doc["mqtt_credentials"] = filesystem->preferences()->getBool(preference_mqtt_credentials_set); // mqtt_credentials;
    json_doc["mqtt_user"] = filesystem->preferences()->getString(preference_mqtt_username); // mqtt_user;
    json_doc["mqtt_pass_set"] = (filesystem->preferences()->getString(preference_mqtt_password) != DEFAULT_MQTT_PASS && filesystem->preferences()->getString(preference_mqtt_password).length() > 0);
    json_doc["ssl_use"] = filesystem->preferences()->getBool(preference_ssl_use); // ssl_use;
    json_doc["ssl_root_ca"] = filesystem->preferences()->getString(preference_ssl_root_ca); // ssl_root_ca;
    json_doc["ssl_validate_client"] = filesystem->preferences()->getBool(preference_ssl_validate_client); // ssl_validate_client;
    json_doc["ssl_client_certificate"] = filesystem->preferences()->getString(preference_ssl_client_certificate); // ssl_client_certificate;
    json_doc["ssl_client_private_key"] = filesystem->preferences()->getString(preference_ssl_client_private_key); // ssl_client_private_key;
    // json_doc["ntp"] = filesystem->preferences()->getString(preference_ntp_server); // ntp_server; // !
    json_doc["gnss_mode"] = filesystem->preferences()->getInt(preference_gnss_mode);
    json_doc["gprs_apn"] = filesystem->preferences()->getString(preference_gprs_apn);
    json_doc["gprs_credentials"] = filesystem->preferences()->getBool(preference_gprs_credentials_set);
    json_doc["gprs_user"] = filesystem->preferences()->getString(preference_gprs_username);;
    json_doc["gprs_pass_set"] = (filesystem->preferences()->getString(preference_gprs_password) != DEFAULT_GPRS_PASS && filesystem->preferences()->getString(preference_gprs_password).length() > 0);
    String data_out;
    serializeJson(json_doc, data_out);
    request->send(200, "application/json", data_out);
  }); // protected

  _server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", main_html);
  }); // protected

  _server->onNotFound([&](AsyncWebServerRequest *request){
    request->redirect("http://" + _ap_ip.toString() + "/");
  }); // protected

  _server->begin();
}

void Web::_initCaptive() { // !
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(_ap_ip, _ap_ip, _ap_mask);
  WiFi.softAP(_hostname.c_str(), _password.c_str(), 1, false, 1 /*2*/); // ! password must be atleast 8 characters
  _dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  _dns_server->start(53, "*", _ap_ip);
  utils->printCaptivePortal(WiFi.softAPIP().toString());
}

void Web::handleCaptive() {
  _dns_server->processNextRequest();
}
