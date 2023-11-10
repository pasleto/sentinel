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
#include <ETH.h>

Web* web = new Web();

Web::Web() {
  _server = new AsyncWebServer(80);
  _dns_server = new DNSServer();
  // _ap_ip = IPAddress(192,168,11,1);
  // _ap_mask = IPAddress(255,255,255,0);
  _ap_ip = IPAddress(4,3,2,1);
  _ap_mask = IPAddress(255,255,255,240);
  _config_save_done_trigger = false;
}

void Web::init(bool startCaptive) {
  _hostname = filesystem->preferences()->getString(preference_hostname, utils->defaultHostname());
  _password = filesystem->preferences()->getString(preference_device_password, DEFAULT_DEVICE_ADMIN_PASSWORD);
  _username = filesystem->preferences()->getString(preference_device_username, DEFAULT_DEVICE_ADMIN_USERNAME);
  _realm = "Login Required";
  _is_digest = true;

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
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
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
      _config_save_done_trigger = true;
      filesystem->logReboot();
      utils->printRestart();
      utils->swReboot();
    }
  })); // protected

  _server->on("/config", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    DynamicJsonDocument json_doc(12288); // 8192
    json_doc["config_mode"] = filesystem->preferences()->getBool(preference_config_mode);
    json_doc["was_configured"] = filesystem->preferences()->getBool(preference_was_configured);
    json_doc["module_type"] = MODULE_TYPE;
    json_doc["firmware"] =  filesystem->preferences()->getString(preference_firmware_version);
    // json_doc["hardware"] = MODULE_HARDWARE_VERSION;
    json_doc["unit_id"] = utils->getMacNag();
    json_doc["mac_address"] = utils->getMac();
    json_doc["hostname"] = filesystem->preferences()->getString(preference_hostname);
    json_doc["keep_webserver"] = filesystem->preferences()->getBool(preference_keep_webserver);
    json_doc["mqtt_server"] = filesystem->preferences()->getString(preference_mqtt_server);
    json_doc["mqtt_port"] = filesystem->preferences()->getInt(preference_mqtt_port);
    json_doc["mqtt_credentials"] = filesystem->preferences()->getBool(preference_mqtt_credentials_set);
    json_doc["mqtt_user"] = filesystem->preferences()->getString(preference_mqtt_username);
    json_doc["mqtt_pass_set"] = (filesystem->preferences()->getString(preference_mqtt_password) != DEFAULT_MQTT_PASS && filesystem->preferences()->getString(preference_mqtt_password).length() > 0);
    json_doc["ssl_use"] = filesystem->preferences()->getBool(preference_ssl_use);
    json_doc["ssl_root_ca"] = filesystem->preferences()->getString(preference_ssl_root_ca);
    json_doc["ssl_validate_client"] = filesystem->preferences()->getBool(preference_ssl_validate_client);
    json_doc["ssl_client_certificate"] = filesystem->preferences()->getString(preference_ssl_client_certificate);
    json_doc["ssl_client_private_key"] = filesystem->preferences()->getString(preference_ssl_client_private_key);
    json_doc["ntp"] = filesystem->preferences()->getString(preference_ntp_server);
    json_doc["ip_static"] = filesystem->preferences()->getBool(preference_ip_static);
    json_doc["ip_static_ip"] = filesystem->preferences()->getString(preference_ip_static_ip);
    json_doc["ip_static_mask"] = filesystem->preferences()->getString(preference_ip_static_mask);
    json_doc["ip_static_gw"] = filesystem->preferences()->getString(preference_ip_static_gw);
    json_doc["ip_static_dns"] = filesystem->preferences()->getString(preference_ip_static_dns);
    json_doc["storage_usage"] = utils->spiffsPercentageUsage();
    json_doc["rtc_battery_warning"] = utils->rtcBatteryWarning(); // TODO
    // json_doc["thermometer_1"] = filesystem->preferences()->getBool(preference_thermometer_1_enabled); // TODO
    // json_doc["thermometer_2"] = filesystem->preferences()->getBool(preference_thermometer_2_enabled); // TODO
    json_doc["thermometer_1_temperature"] = filesystem->preferences()->getFloat(preference_thermometer_1_temperature); // TODO
    json_doc["thermometer_1_humidity"] = filesystem->preferences()->getFloat(preference_thermometer_1_humidity); // TODO
    json_doc["thermometer_2_temperature"] = filesystem->preferences()->getFloat(preference_thermometer_2_temperature); // TODO
    json_doc["thermometer_2_humidity"] = filesystem->preferences()->getFloat(preference_thermometer_2_humidity); // TODO
    String data_out;
    serializeJson(json_doc, data_out);
    request->send(200, "application/json", data_out);
  }); // protected

  _server->on("/storage", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    request->send(200, "application/json", utils->filesystemUsageJson());
  }); // protected

  _server->on("/log/count", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    int count = filesystem->getLogsCount();
    if (count != -1) {
      DynamicJsonDocument json_doc(128);
      json_doc["count"] =  count;
      String data_out;
      serializeJson(json_doc, data_out);
      request->send(200, "application/json", data_out);
    } else {
      request->send(500, "text/plain", "ERROR: Unable to fetch records count from database!");
    }
  }); // protected

  _server->on("/log/data", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    int count = 5; // items per page
    if (request->hasParam("page")) {
      String page_param = request->getParam("page")->value();
      int page = page_param.toInt();
      // ? - calculate number of pages -> return error is overflow
      DynamicJsonDocument json_doc(12288); // TODO
      JsonArray data = json_doc.createNestedArray("data");
      for (size_t i = 0; i < count; i++) {
        Event event = filesystem->getPagedEvent(count, page, i);
        if (event.id != -1) {
          JsonObject event_object = data.createNestedObject();
          event_object["id"] = event.id;
          event_object["timestamp"] = event.timestamp;
          event_object["state"] = event.state;
          event_object["initiator"] = event.initiator;
          event_object["measurement"] = event.measurement;
          event_object["status"] = event.status;
        }
      }
      String data_out;
      serializeJson(json_doc, data_out);
      request->send(200, "application/json", data_out);
    } else {
      DynamicJsonDocument json_doc(128);
      JsonArray data = json_doc.createNestedArray("data");
      String data_out;
      serializeJson(json_doc, data_out);
      request->send(200, "application/json", data_out);
    }
  }); // protected

  _server->addHandler(new AsyncCallbackJsonWebHandler("/log/remove", [&](AsyncWebServerRequest* request, JsonVariant& json) {
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    if (not json.is<JsonObject>()) {
      request->send(500, "text/plain", "ERROR: Not an JSON object!");
      return;
    }
    auto&& data = json.as<JsonObject>();
    String job_id = "";
    String action = "EVENT_REMOVE";
    if (data.containsKey("id")) {
      int id = data["id"]; // default to 0
      if (id > 0) {
        /*const char * result =*/ filesystem->eventAction(job_id, action, "DELETE FROM events WHERE rowid = " + String(id) + ";"); // ?
        // if (result != "OK") {
        //   String err = "ERROR: ";
        //   err + result;
        //   request->send(500, "text/plain", err);
        // } else {
        //   request->send(200, "text/plain", "SUCCESS: Event removed");
        // }
        request->send(200, "text/plain", "SUCCESS: Event removed");
      } else {
        request->send(500, "text/plain", "ERROR: No ID provided");
      }
    } else {
      request->send(500, "text/plain", "ERROR: No ID provided");
    }
  })); // protected

  _server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      if(!request->authenticate(_username.c_str(), _password.c_str())) {
        return request->requestAuthentication(_realm, _is_digest);
      }
    }
    request->send_P(200, "text/html", main_html);
  }); // protected

  _server->onNotFound([&](AsyncWebServerRequest *request){
    if (_ON_ETH(request)) {
      request->redirect("/");
    } else {
      request->redirect("http://" + _ap_ip.toString() + "/");
    }
  }); // redirect

  _server->begin();
}

bool Web::_ON_ETH(AsyncWebServerRequest *request) {
  // return ETH.localIP() == request->client()->localIP();
  return !_ON_CAPTIVE(request);
}

bool Web::_ON_CAPTIVE(AsyncWebServerRequest *request) {
  // return ETH.localIP() != request->client()->localIP();
  return WiFi.softAPIP() == request->client()->localIP();
}

void Web::_initCaptive() {
  WiFi.mode(WIFI_AP); // ? - brownout -> 100 uF capacitor on 5V pin
  WiFi.softAPConfig(_ap_ip, _ap_ip, _ap_mask);
  WiFi.softAP(_hostname.c_str(), _password.c_str(), 1, false, 1 /*2*/);
  _dns_server->setErrorReplyCode(DNSReplyCode::NoError);
  _dns_server->start(53, "*", _ap_ip);
  utils->printCaptivePortal(WiFi.softAPIP().toString());
}

void Web::handleCaptive() {
  _dns_server->processNextRequest();
}

bool Web::configSaveDoneTrigger() {
  return _config_save_done_trigger;
}

void Web::setConfigSaveDoneTrigger(bool value) {
  _config_save_done_trigger = value;
}
