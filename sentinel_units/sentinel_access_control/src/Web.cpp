#include "Web.h"
#include "Utils.h"
#include "Filesystem.h"
#include "Logger.h"
#include "StatusLed.h"
#include "config.h"
#include "webpage.h"
#include <ETH.h>

#include "mbedtls/error.h"
#include "mbedtls/base64.h"
#include "CryptoHandler.h"

Web* web = new Web();

Web::Web() {
  _server = new AsyncWebServer(80);
  _dns_server = new DNSServer();
  _ws = new AsyncWebSocket("/ws");
  _ap_ip = IPAddress(4,3,2,1);
  _ap_mask = IPAddress(255,255,255,240);
}

void Web::init(bool startCaptive) {
  _config_save_done_trigger = false; // ?
  _config_exit_trigger = false; // ?
  _ws_client_id = 0;
  _hostname = filesystem->getHostname();
  _password = filesystem->getDevicePassword();
  _username = filesystem->getDeviceUsername();
  _realm = "Login Required";
  _is_digest = true;
  _client_connected = false;
  _socket_data = "";

  if (startCaptive) {
    _initCaptive();
  } else {
    _destroyCaptive();
  }

  _initWebSocket();

  _server->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "image/x-icon", favicon, sizeof(favicon));
  }); // public

  _server->on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/css", main_css);
  }); // public

  _server->on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/javascript", main_js);
  }); // public

  _server->on("/info", HTTP_GET, [&](AsyncWebServerRequest *request) { // TODO
    // if (!filesystem->getWasConfigured()) { // TODO - disabled for testing
      if (filesystem->rsaKeyPairExist()) {
        DynamicJsonDocument json_out(2048);
        json_out["module_type"] = MODULE_TYPE;
        json_out["firmware"] = filesystem->getFirmwareVersion();
        json_out["hardware"] = filesystem->getHardwareVersion();
        json_out["unit_id"] = utils->getMacNag();
        json_out["mac_address"] = utils->getMac();
        json_out["public_key"] = filesystem->getRsaPubKey();
        size_t json_size = measureJson(json_out);
        char msg_out[json_size + 1];
        serializeJson(json_out, msg_out, json_size + 1);
        request->send(200, "application/json", msg_out);
      } else {
        request->send(404);
      }
    // } else { // TODO - disabled for testing
    //   request->send(403);
    // }
  });

  _server->on("/ram", HTTP_GET, [&](AsyncWebServerRequest *request) { // ? remove
    request->send(200, "text/plain", String(ESP.getFreeHeap()) + " / " + String(ESP.getHeapSize()));
  });

  _server->addHandler(new AsyncCallbackJsonWebHandler("/test", [&](AsyncWebServerRequest* request, JsonVariant& json) { // TODO
    if (not json.is<JsonObject>()) {
      request->send(400);
      return;
    }
    auto&& data = json.as<JsonObject>();
    if (data.size() > 0) {
      if (data.containsKey("data")) {
        String json_data = data["data"]; // default to ""
        json_data.trim();
        if (json_data.length() > 0) {

          Log->println("[DEBUG] /test - data: ");
          Log->println(json_data);


          request->send(200);

        } else {
          request->send(400);
        }
      } else {
        request->send(400);
      }
    } else {
      request->send(400);
    }
  }));

  _server->addHandler(new AsyncCallbackJsonWebHandler("/ecb", [&](AsyncWebServerRequest* request, JsonVariant& json) { // TODO
    // if (!filesystem->getWasConfigured()) { // TODO - disabled for testing
      if (not json.is<JsonObject>()) {
        request->send(400);
        return;
      }
      auto&& data = json.as<JsonObject>();
      if (data.size() > 0) {
        if (data.containsKey("key")) {
          String json_key = data["key"]; // default to ""
          json_key.trim();
          if (json_key.length() > 0) {
            if (data.containsKey("data")) {
              String json_data = data["data"]; // default to ""
              json_data.trim();
              if (json_data.length() > 0) {
                String privKey = filesystem->getRsaPrivKey();
                size_t key_base64_buf_len = 0;
                mbedtls_base64_decode(NULL, 0, &key_base64_buf_len, (unsigned char*)json_key.c_str(), json_key.length());
                unsigned char key_base64_buf[key_base64_buf_len];
                if (mbedtls_base64_decode(key_base64_buf, key_base64_buf_len, &key_base64_buf_len, (unsigned char*)json_key.c_str(), json_key.length()) == 0) {
                  RsaResponse rsa_dec_res = cryptoHandler->rsa_pk_decrypt(privKey.c_str(), (char*)key_base64_buf, key_base64_buf_len);
                  if (rsa_dec_res.success) {
                    size_t data_base64_buf_len = 0;
                    mbedtls_base64_decode(NULL, 0, &data_base64_buf_len, (unsigned char*)json_data.c_str(), json_data.length());
                    unsigned char data_base64_buf[data_base64_buf_len];
                    if (mbedtls_base64_decode(data_base64_buf, data_base64_buf_len, &data_base64_buf_len, (unsigned char*)json_data.c_str(), json_data.length()) == 0) {
                      String aes_data = cryptoHandler->aes_ecb_decrypt(data_base64_buf, data_base64_buf_len, (char*)rsa_dec_res.data, rsa_dec_res.len);
                      Log->println("[DEBUG] ECB AES Data: "); // !
                      Log->println(aes_data); // !
                      DynamicJsonDocument json_doc(1.5 * aes_data.length());
                      DeserializationError json_des_error = deserializeJson(json_doc, aes_data);
                      if (!json_des_error) {
                        
                        
                        // TODO - continue
                        
                        
                        Log->println(); // !
                        request->send(200);
                        
                        
                      } else {
                        request->send(400);
                      }
                    } else {
                      request->send(400);
                    }
                  } else {
                    request->send(400);
                  }
                } else {
                  request->send(400);
                }
              } else {
                request->send(400);
              }
            } else {
              request->send(400);
            }
          } else {
            request->send(400);
          }
        } else {
          request->send(400);
        }
      } else {
        request->send(400);
      }
    // } else { // TODO - disabled for testing
    //   request->send(403);
    // }
  }));

  _server->addHandler(new AsyncCallbackJsonWebHandler("/cbc", [&](AsyncWebServerRequest* request, JsonVariant& json) { // TODO
    // if (!filesystem->getWasConfigured()) { // TODO - disabled for testing
      if (not json.is<JsonObject>()) {
        request->send(400);
        return;
      }
      auto&& data = json.as<JsonObject>();
      if (data.size() > 0) {
        if (data.containsKey("key")) {
          String json_key = data["key"]; // default to ""
          json_key.trim();
          if (json_key.length() > 0) {
            if (data.containsKey("iv")) {
              String json_iv = data["iv"]; // default to ""
              json_iv.trim();
              if (json_iv.length() > 0) {
                if (data.containsKey("data")) {
                  String json_data = data["data"]; // default to ""
                  json_data.trim();
                  if (json_data.length() > 0) {
                    String privKey = filesystem->getRsaPrivKey();
                    size_t key_base64_buf_len = 0;
                    mbedtls_base64_decode(NULL, 0, &key_base64_buf_len, (unsigned char*)json_key.c_str(), json_key.length());
                    unsigned char key_base64_buf[key_base64_buf_len];
                    if (mbedtls_base64_decode(key_base64_buf, key_base64_buf_len, &key_base64_buf_len, (unsigned char*)json_key.c_str(), json_key.length()) == 0) {
                      RsaResponse rsa_key_dec_res = cryptoHandler->rsa_pk_decrypt(privKey.c_str(), (char*)key_base64_buf, key_base64_buf_len);
                      if (rsa_key_dec_res.success) {
                        size_t iv_base64_buf_len = 0;
                        mbedtls_base64_decode(NULL, 0, &iv_base64_buf_len, (unsigned char*)json_iv.c_str(), json_iv.length());
                        unsigned char iv_base64_buf[iv_base64_buf_len];
                        if (mbedtls_base64_decode(iv_base64_buf, iv_base64_buf_len, &iv_base64_buf_len, (unsigned char*)json_iv.c_str(), json_iv.length()) == 0) {
                          RsaResponse rsa_iv_dec_res = cryptoHandler->rsa_pk_decrypt(privKey.c_str(), (char*)iv_base64_buf, iv_base64_buf_len);
                          if (rsa_iv_dec_res.success) {
                            size_t data_base64_buf_len = 0;
                            mbedtls_base64_decode(NULL, 0, &data_base64_buf_len, (unsigned char*)json_data.c_str(), json_data.length());
                            unsigned char data_base64_buf[data_base64_buf_len];
                            if (mbedtls_base64_decode(data_base64_buf, data_base64_buf_len, &data_base64_buf_len, (unsigned char*)json_data.c_str(), json_data.length()) == 0) {
                              String aes_data = cryptoHandler->aes_cbc_decrypt((char*)rsa_key_dec_res.data, rsa_key_dec_res.len, (char*)rsa_iv_dec_res.data, data_base64_buf_len, data_base64_buf);
                              Log->println("[DEBUG] CBC AES Data: "); // !
                              Log->println(aes_data); // !
                              DynamicJsonDocument json_doc(1.5 * aes_data.length());
                              DeserializationError json_des_error = deserializeJson(json_doc, aes_data);
                              if (!json_des_error) {
                                
                                
                                // TODO - continue
                                
                                
                                Log->println(); // !
                                request->send(200);
                                
                                
                              } else {
                                request->send(400);
                              }
                            } else {
                              request->send(400);
                            }
                          } else {
                            request->send(400);
                          }
                        } else {
                          request->send(400);
                        }
                      } else {
                        request->send(400);
                      }
                    } else {
                      request->send(400);
                    }
                  } else {
                    request->send(400);
                  }
                } else {
                  request->send(400);
                }
              } else {
                request->send(400);
              }
            } else {
              request->send(400);
            }
          } else {
            request->send(400);
          }
        } else {
          request->send(400);
        }
      } else {
        request->send(400);
      }
    // } else { // TODO - disabled for testing
    //   request->send(403);
    // }
  }));

  _server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
    if (!_client_connected) {
      if (_ON_ETH(request->client()->localIP())) {
        if(!request->authenticate(_username.c_str(), _password.c_str())) {
          return request->requestAuthentication(_realm, _is_digest);
        }
      }
      request->send_P(200, "text/html", main_html);
    } else {
      request->send_P(200, "text/html", full_html);
    }
  }); // protected

  _server->onNotFound([&](AsyncWebServerRequest *request) {
    if (_ON_ETH(request->client()->localIP())) {
      request->redirect("/");
    } else {
      request->redirect("http://" + _ap_ip.toString() + "/");
    }
  }); // redirect

  _server->begin();
}

void Web::disable() {
  if (_ws->enabled()) {
    _ws->enable(false);
  }
  _ws->closeAll();
  _server->end();
  _destroyCaptive();
}

AsyncWebSocket* Web::websocket() {
  return _ws;
}

void Web::_handleWebSocketMessageData(DynamicJsonDocument json_doc, AsyncWebSocketClient *client) { // ! ?
  if (json_doc.size() > 0) {
    if (json_doc.containsKey("topic")) {
      String json_topic = json_doc["topic"];
      json_topic.trim();
      if (json_topic.length() > 0) {
        // TODO
        Log->print(F("[WS] Received Message: "));
        Log->println(json_topic);
        // TODO
        if (json_topic.equalsIgnoreCase("CONFIG-GET-REQ")) {
          DynamicJsonDocument data_json = filesystem->getConfigJson();
          size_t data_json_size = measureJson(data_json);
          DynamicJsonDocument json_out(data_json_size + 48);
          json_out["topic"] = "CONFIG-GET-RES";
          json_out["data"] = data_json; // filesystem->getConfigJson();
          size_t json_size = measureJson(json_out);
          char msg_out[json_size + 1];
          serializeJson(json_out, msg_out, json_size + 1);
          client->text(msg_out);
        }
        if (json_topic.equalsIgnoreCase("LOG-COUNT-REQ")) { // ? refactor
          int count = filesystem->getLogsCount();
          if (count != -1) {
            if (json_doc.containsKey("init")) {
              bool json_init = json_doc["init"]; // default to false
              if (json_init) {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "LOG-COUNT-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = false;
                data["count"] = count;
                data["init"] = true;
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              } else {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "LOG-COUNT-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = false;
                data["count"] = count;
                data["init"] = false;
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              }
            } else {
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "LOG-COUNT-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["count"] = count;
              data["init"] = false;
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
            }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "LOG-COUNT-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = "Unable to fetch records count from database";
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("DB-COUNT-REQ")) { // ? refactor
          int count = filesystem->getAllCardsCount();
          if (count != -1) {
            if (json_doc.containsKey("init")) {
              bool json_init = json_doc["init"]; // default to false
              if (json_init) {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "DB-COUNT-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = false;
                data["count"] = count;
                data["init"] = true;
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              } else {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "DB-COUNT-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = false;
                data["count"] = count;
                data["init"] = false;
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              }
            } else {
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "DB-COUNT-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["count"] = count;
              data["init"] = false;
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
            }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "DB-COUNT-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = "Unable to fetch records count from database";
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("STORAGE-USAGE-REQ")) {
          JsonVariant /*DynamicJsonDocument*/ data_json = utils->filesystemUsageJson(); // ?
          StaticJsonDocument<128> json_out;
          json_out["topic"] = "STORAGE-USAGE-RES";
          json_out["data"] = data_json; // utils->filesystemUsageJson(); // ?
          size_t json_size = measureJson(json_out);
          char msg_out[json_size + 1];
          serializeJson(json_out, msg_out, json_size + 1);
          client->text(msg_out);
        }
        if (json_topic.equalsIgnoreCase("LOG-DATA-REQ")) { // ! change check -
          int count = 5; // items per page
          if (json_doc.containsKey("page")) {
            int json_page = json_doc["page"]; // default to 0
            DynamicJsonDocument json_out(2048);
            json_out["topic"] = "LOG-DATA-RES";
            JsonArray data = json_out.createNestedArray("data");
            for (size_t i = 0; i < count; i++) {
              Event event = filesystem->getPagedEvent(count, json_page, i);
              if (event.rowid != -1) {
                JsonObject event_object = data.createNestedObject();
                event_object["rowid"] = event.rowid;
                event_object["timestamp"] = event.timestamp;
                event_object["state"] = event.state;
                event_object["initiator"] = event.initiator;
                event_object["value"] = event.value;
                event_object["id"] = event.id;
                event_object["status"] = event.status;
                event_object["level"] = event.level;
                event_object["type"] = event.type;
              }
            }
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "LOG-DATA-RES";
            JsonArray data = json_out.createNestedArray("data");
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("DB-DATA-REQ")) { // ! change check -
          int count = 5; // items per page
          if (json_doc.containsKey("page")) {
            int json_page = json_doc["page"]; // default to 0
            DynamicJsonDocument json_out(2048);
            json_out["topic"] = "DB-DATA-RES";
            JsonArray data = json_out.createNestedArray("data");
            for (size_t i = 0; i < count; i++) {
              Card card = filesystem->getPagedCard(count, json_page, i);
              if (card.success /*&& card.id.length() > 0*/) {
                JsonObject card_object = data.createNestedObject();
                card_object["id"] = card.id;
                card_object["value"] = card.value;
                card_object["can_enter"] = card.can_enter;
                card_object["can_exit"] = card.can_exit;
                card_object["has_validity"] = card.has_validity;
                card_object["valid_since"] = card.valid_since;
                card_object["valid_until"] = card.valid_until;
              }
            }
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "DB-DATA-RES";
            JsonArray data = json_out.createNestedArray("data");
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("LOG-REMOVE-REQ")) { // TODO -> filesystem->eventAction should return value
          String job_id = "";
          String action = "EVENT_REMOVE";
          if (json_doc.containsKey("rowid")) {
            int json_rowid = json_doc["rowid"]; // default to 0
            // if (json_id > 0) { // ? - what if event_id == 0 -> is it even possible ???
              const char * result = filesystem->eventAction(job_id, action, "DELETE FROM events WHERE rowid = " + String(json_rowid) + ";");
              if (result != "OK") { // ???
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "LOG-REMOVE-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = true;
                data["message"] = result;
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              } else { // ???
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "LOG-REMOVE-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = false;
                data["message"] = "Event removed from database";
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              }
            // } else {
            //   StaticJsonDocument<128> json_out;
            //   json_out["topic"] = "LOG-REMOVE-RES";
            //   JsonObject data = json_out.createNestedObject("data");
            //   data["error"] = true;
            //   data["message"] = "No ROWID provided";
            //   size_t json_size = measureJson(json_out);
            //   char msg_out[json_size + 1];
            //   serializeJson(json_out, msg_out, json_size + 1);
            //   client->text(msg_out);
            // }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "LOG-REMOVE-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = "No ID provided";
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("LOG-REMOVE-ALL-REQ")) { // ! !
          xTaskCreatePinnedToCore([](void * parameter){
            String job_id = "";
            String action = "EVENTS_WIPE";
            const char * result = filesystem->eventAction(job_id, action, "DELETE FROM events;"); // ?
            if (result != "OK") { // ???
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "LOG-REMOVE-ALL-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = true;
              data["message"] = result;
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              web->wsClientSend(msg_out);
            } else { // ???
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "LOG-REMOVE-ALL-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["message"] = "All events removed from database";
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              web->wsClientSend(msg_out);
            }
            vTaskDelete(NULL);
          }, "delete_all_events_web_task", 8192, NULL, 2, nullptr, 1); // ? stack
        }
        if (json_topic.equalsIgnoreCase("DB-ADD-REQ")) { // TODO -> filesystem->cardAction should return value
          
          // ! change check -
          
          String job_id = "";
          String action = "CARD_ADD";
          if (json_doc.containsKey("id")) {
            String json_id = json_doc["id"]; // default to - ""
            json_id.trim();
            if (json_id.length() > 0) {
              if (json_doc.containsKey("value")) {
                String json_value = json_doc["value"]; // default to - ""
                json_value.trim();
                if (json_value.length() > 0) {
                  bool json_can_enter = json_doc["can_enter"]; // default to - 0
                  bool json_can_exit = json_doc["can_exit"]; // default to - 0
                  bool json_has_validity = json_doc["has_validity"]; // default to - 0
                  if (json_has_validity) {
                    int json_valid_since = json_doc["valid_since"]; // default to - 0
                    int json_valid_until = json_doc["valid_until"]; // default to - 0
                    if ((json_valid_since > 0) && (json_valid_until > 0) && (json_valid_until > json_valid_since)) {
                      const char * result = filesystem->cardAction(job_id, action, "REPLACE INTO cards(id, value, can_enter, can_exit, has_validity, valid_since, valid_until) VALUES('" + json_id + "', '" + json_value + "', '" + json_can_enter + "', '" + json_can_exit + "', " + json_has_validity + ", " + json_valid_since + ", " + json_valid_until + ");");
                      if (result != "OK") { // ???
                        StaticJsonDocument<128> json_out;
                        json_out["topic"] = "DB-ADD-RES";
                        JsonObject data = json_out.createNestedObject("data");
                        data["error"] = true;
                        data["message"] = result;
                        if (json_doc.containsKey("new")) {
                          data["new"] = json_doc["new"];
                        }
                        size_t json_size = measureJson(json_out);
                        char msg_out[json_size + 1];
                        serializeJson(json_out, msg_out, json_size + 1);
                        client->text(msg_out);
                      } else { // ???
                        StaticJsonDocument<128> json_out;
                        json_out["topic"] = "DB-ADD-RES";
                        JsonObject data = json_out.createNestedObject("data");
                        data["error"] = false;
                        data["message"] = "Card added to database";
                        if (json_doc.containsKey("new")) {
                          data["new"] = json_doc["new"];
                        }
                        size_t json_size = measureJson(json_out);
                        char msg_out[json_size + 1];
                        serializeJson(json_out, msg_out, json_size + 1);
                        client->text(msg_out);
                      }
                    } else {
                      StaticJsonDocument<128> json_out;
                      json_out["topic"] = "DB-ADD-RES";
                      JsonObject data = json_out.createNestedObject("data");
                      data["error"] = true;
                      data["message"] = "Wrong UNTIL / SINCE timestamps provided";
                      if (json_doc.containsKey("new")) {
                        data["new"] = json_doc["new"];
                      }
                      size_t json_size = measureJson(json_out);
                      char msg_out[json_size + 1];
                      serializeJson(json_out, msg_out, json_size + 1);
                      client->text(msg_out);
                    }
                  } else {
                    const char * result = filesystem->cardAction(job_id, action, "REPLACE INTO cards(id, value, has_validity, valid_since, valid_until) VALUES('" + json_id + "', '" + json_value + "', " + json_has_validity + ", 946684800, 946684800);"); // NULL NULL
                    if (result != "OK") { // ???
                      StaticJsonDocument<128> json_out;
                      json_out["topic"] = "DB-ADD-RES";
                      JsonObject data = json_out.createNestedObject("data");
                      data["error"] = true;
                      data["message"] = result;
                      if (json_doc.containsKey("new")) {
                        data["new"] = json_doc["new"];
                      }
                      size_t json_size = measureJson(json_out);
                      char msg_out[json_size + 1];
                      serializeJson(json_out, msg_out, json_size + 1);
                      client->text(msg_out);
                    } else { // ???
                      StaticJsonDocument<128> json_out;
                      json_out["topic"] = "DB-ADD-RES";
                      JsonObject data = json_out.createNestedObject("data");
                      data["error"] = false;
                      data["message"] = "Card added to database";
                      if (json_doc.containsKey("new")) {
                        data["new"] = json_doc["new"];
                      }
                      size_t json_size = measureJson(json_out);
                      char msg_out[json_size + 1];
                      serializeJson(json_out, msg_out, json_size + 1);
                      client->text(msg_out);
                    }
                  }
                } else {
                  StaticJsonDocument<128> json_out;
                  json_out["topic"] = "DB-ADD-RES";
                  JsonObject data = json_out.createNestedObject("data");
                  data["error"] = true;
                  data["message"] = "No VALUE provided";
                  if (json_doc.containsKey("new")) {
                    data["new"] = json_doc["new"];
                  }
                  size_t json_size = measureJson(json_out);
                  char msg_out[json_size + 1];
                  serializeJson(json_out, msg_out, json_size + 1);
                  client->text(msg_out);
                }
              } else {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "DB-ADD-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = true;
                data["message"] = "No VALUE provided";
                if (json_doc.containsKey("new")) {
                  data["new"] = json_doc["new"];
                }
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              }
            } else {
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "DB-ADD-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = true;
              data["message"] = "No ID provided";
              if (json_doc.containsKey("new")) {
                data["new"] = json_doc["new"];
              }
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
            }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "DB-ADD-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = "No ID provided";
            if (json_doc.containsKey("new")) {
              data["new"] = json_doc["new"];
            }
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("DB-REMOVE-REQ")) { // TODO -> filesystem->cardAction should return value
          String job_id = "";
          String action = "CARD_REMOVE";
          if (json_doc.containsKey("id")) {
            String json_id = json_doc["id"]; // default to - ""
            json_id.trim();
            if (json_id.length() > 0) {
              if (json_doc.containsKey("value")) {
                String json_value = json_doc["value"]; // default to - ""
                json_value.trim();
                if (json_value.length() > 0) {
                  const char * result = filesystem->cardAction(job_id, action, "DELETE FROM cards WHERE id = '" + json_id + "' OR value = '" + json_value + "';");
                  if (result != "OK") { // ???
                    StaticJsonDocument<128> json_out;
                    json_out["topic"] = "DB-REMOVE-RES";
                    JsonObject data = json_out.createNestedObject("data");
                    data["error"] = true;
                    data["message"] = result;
                    size_t json_size = measureJson(json_out);
                    char msg_out[json_size + 1];
                    serializeJson(json_out, msg_out, json_size + 1);
                    client->text(msg_out);
                  } else { // ???
                    StaticJsonDocument<128> json_out;
                    json_out["topic"] = "DB-REMOVE-RES";
                    JsonObject data = json_out.createNestedObject("data");
                    data["error"] = false;
                    data["message"] = "Card removed from database";
                    size_t json_size = measureJson(json_out);
                    char msg_out[json_size + 1];
                    serializeJson(json_out, msg_out, json_size + 1);
                    client->text(msg_out);
                  }
                } else {
                  StaticJsonDocument<128> json_out;
                  json_out["topic"] = "DB-REMOVE-RES";
                  JsonObject data = json_out.createNestedObject("data");
                  data["error"] = true;
                  data["message"] = "No VALUE provided";
                  size_t json_size = measureJson(json_out);
                  char msg_out[json_size + 1];
                  serializeJson(json_out, msg_out, json_size + 1);
                  client->text(msg_out);
                }
              } else {
                StaticJsonDocument<128> json_out;
                json_out["topic"] = "DB-REMOVE-RES";
                JsonObject data = json_out.createNestedObject("data");
                data["error"] = true;
                data["message"] = "No VALUE provided";
                size_t json_size = measureJson(json_out);
                char msg_out[json_size + 1];
                serializeJson(json_out, msg_out, json_size + 1);
                client->text(msg_out);
              }
            } else {
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "DB-REMOVE-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = true;
              data["message"] = "No ID provided";
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
            }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "DB-REMOVE-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = "No ID provided";
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
        if (json_topic.equalsIgnoreCase("DB-REMOVE-ALL-REQ")) { // ! !
          xTaskCreatePinnedToCore([](void * parameter){
            String job_id = "";
            String action = "CARDS_WIPE";
            const char * result = filesystem->cardAction(job_id, action, "DELETE FROM cards;");
            if (result != "OK") { // ???
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "DB-REMOVE-ALL-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = true;
              data["message"] = result;
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              web->wsClientSend(msg_out);
            } else { // ???
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "DB-REMOVE-ALL-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["message"] = "All cards removed from database";
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              web->wsClientSend(msg_out);
            }
            vTaskDelete(NULL);
          }, "delete_all_cards_web_task", 8192, NULL, 2, nullptr, 1); // ? stack
        }
        if (json_topic.equalsIgnoreCase("CONFIG-SET-REQ")) { // ! modify -> if reboot is needed, if from WIFI AP, etc.
          JsonObject json_data = json_doc["data"];
          const char * result = filesystem->handleConfigJson(json_data); // !
          if (result == "OK" || result == "EXIT") {
            if (result == "OK") {
              filesystem->setConfigMode(false);
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "CONFIG-SET-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["message"] = "Successfully saved! Rebooting!";
              data["reboot"] = true; // !
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
              filesystem->logConfigChange("SUCCESS");
              _config_save_done_trigger = true;
              filesystem->logReboot();
              utils->printRestart();
              utils->swReboot();
            }
            if (result == "EXIT") {
              StaticJsonDocument<128> json_out;
              json_out["topic"] = "CONFIG-SET-RES";
              JsonObject data = json_out.createNestedObject("data");
              data["error"] = false;
              data["message"] = "No changes. Exiting!";
              data["reboot"] = false; // !
              size_t json_size = measureJson(json_out);
              char msg_out[json_size + 1];
              serializeJson(json_out, msg_out, json_size + 1);
              client->text(msg_out);
              _config_exit_trigger = true;
            }
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "CONFIG-SET-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["error"] = true;
            data["message"] = result;
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
            filesystem->logConfigChange("ERROR: " + String(result));
          }
        }
        if (json_topic.equalsIgnoreCase("REBOOT-REQ")) { // !
          filesystem->logReboot();
          utils->printRestart();
          utils->swReboot();
        }
        if (json_topic.equalsIgnoreCase("CLOSE-CONNECTION-REQ")) {
          _client_connected = false;
          // client->close();
        }
      } else {
        StaticJsonDocument<128> json_out;
        json_out["topic"] = "ERROR-RES";
        JsonObject data = json_out.createNestedObject("data");
        data["type"] = "JsonInvalid";
        data["message"] = "TOPIC has invalid value";
        size_t json_size = measureJson(json_out);
        char msg_out[json_size + 1];
        serializeJson(json_out, msg_out, json_size + 1);
        client->text(msg_out);
      }
    } else {
      StaticJsonDocument<128> json_out;
      json_out["topic"] = "ERROR-RES";
      JsonObject data = json_out.createNestedObject("data");
      data["type"] = "JsonInvalid";
      data["message"] = "TOPIC is missing";
      size_t json_size = measureJson(json_out);
      char msg_out[json_size + 1];
      serializeJson(json_out, msg_out, json_size + 1);
      client->text(msg_out);
    }
  } else {
    StaticJsonDocument<128> json_out;
    json_out["topic"] = "ERROR-RES";
    JsonObject data = json_out.createNestedObject("data");
    data["type"] = "JsonInvalid";
    data["message"] = "Empty JSON";
    size_t json_size = measureJson(json_out);
    char msg_out[json_size + 1];
    serializeJson(json_out, msg_out, json_size + 1);
    client->text(msg_out);
  }
}

void Web::_handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) { // the whole message is in a single frame and we got all of it's data
    data[len] = 0;
    DynamicJsonDocument json_doc(len + 1);
    DeserializationError json_des_error = deserializeJson(json_doc, (char*)data);
    if (!json_des_error) {
      _handleWebSocketMessageData(json_doc, client);
    } else {
      StaticJsonDocument<128> json_out;
      json_out["topic"] = "ERROR-RES";
      JsonObject data = json_out.createNestedObject("data");
      data["type"] = "DeserializationError";
      data["message"] = json_des_error.c_str();
      size_t json_size = measureJson(json_out);
      char msg_out[json_size + 1];
      serializeJson(json_out, msg_out, json_size + 1);
      client->text(msg_out);
    }
  } else { // the message is splitted into chunks and we got only part of it's data
    if (info->opcode == WS_TEXT) {
      if (info->index == 0) {
        if (info->num == 0) { // Chunked Message - start
          _socket_data = "";
        }
      }
      data[len] = 0;
      _socket_data += (char*)data;
      if ((info->index + len) == info->len) {
        if (info->final) { // Chunked Message - end
          DynamicJsonDocument json_doc(1.5 * _socket_data.length()); // 1.75 * _socket_data.length()
          DeserializationError json_des_error = deserializeJson(json_doc, _socket_data);
          _socket_data = "";
          if (!json_des_error) {
            _handleWebSocketMessageData(json_doc, client);
          } else {
            StaticJsonDocument<128> json_out;
            json_out["topic"] = "ERROR-RES";
            JsonObject data = json_out.createNestedObject("data");
            data["type"] = "DeserializationError";
            data["message"] = json_des_error.c_str();
            size_t json_size = measureJson(json_out);
            char msg_out[json_size + 1];
            serializeJson(json_out, msg_out, json_size + 1);
            client->text(msg_out);
          }
        }
      }
    }
  }
}

void Web::_sendRebootStatus(AsyncWebSocketClient *client) {
  StaticJsonDocument<128> json_out;
  json_out["topic"] = "REBOOT-STATUS-RES";
  json_out["status"] = filesystem->getRebootNeeded();
  json_out["timestamp"] = filesystem->getRebootNeededTimestamp();
  size_t json_size = measureJson(json_out);
  char msg_out[json_size + 1];
  serializeJson(json_out, msg_out, json_size + 1);
  client->text(msg_out);
}

void Web::sendRebootStatusToAll() {
  StaticJsonDocument<128> json_out;
  json_out["topic"] = "REBOOT-STATUS-RES";
  json_out["status"] = filesystem->getRebootNeeded();
  json_out["timestamp"] = filesystem->getRebootNeededTimestamp();
  size_t json_size = measureJson(json_out);
  char msg_out[json_size + 1];
  serializeJson(json_out, msg_out, json_size + 1);
  _ws->textAll(msg_out);
}

void Web::_sendConnectionType(AsyncWebSocketClient *client) {
  bool eth = _ON_ETH(client->client()->localIP());
  StaticJsonDocument<128> json_out;
  json_out["topic"] = "CONNECTION-TYPE-RES";
  json_out["ethernet"] = eth;
  json_out["accesspoint"] = !eth;
  size_t json_size = measureJson(json_out);
  char msg_out[json_size + 1];
  serializeJson(json_out, msg_out, json_size + 1);
  client->text(msg_out);
}

void Web::_initWebSocket() {
  _ws->onEvent([&](AsyncWebSocket *socket, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    switch (type) {
      case WS_EVT_CONNECT:
        _client_connected = true;
        _ws_client_id = client->id();
        statusLed->setApConnected(true); // !
        _sendConnectionType(client);
        _sendRebootStatus(client);
        // _ws->cleanupClients(); // ?
        break;
      case WS_EVT_DATA:
        _handleWebSocketMessage(client, arg, data, len);
        break;
      case WS_EVT_DISCONNECT:
        // _ws->cleanupClients(); // ?
        _client_connected = false;
        _ws_client_id = 0;
        statusLed->setApConnected(false); // !
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
    }
  });
  _server->addHandler(_ws);
  if (!_ws->enabled()) {
    _ws->enable(true);
  }
}

bool Web::_ON_ETH(IPAddress ipaddress) {
  return !_ON_CAPTIVE(ipaddress);
}

bool Web::_ON_CAPTIVE(IPAddress ipaddress) {
  return WiFi.softAPIP() == ipaddress;
}

void Web::_destroyCaptive() {
  WiFi.enableAP(false);
  WiFi.softAPdisconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void Web::_initCaptive() {
  WiFi.mode(WIFI_AP); // brownout -> 100 uF capacitor on 5V pin
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

bool Web::configExitTrigger() {
  return _config_exit_trigger;
}

void Web::setConfigExitTrigger(bool value) {
  _config_exit_trigger = value;
}

void Web::wsClientSend(char *message) { // !
  return _ws->text(_ws_client_id, message);
}
