#include <Arduino.h>
#include "config.h"
#include "defaults.h"
#include "events.h"
#include "pins.h"
#include "prefs.h"
#include "topics.h"
#include "version.h"
// #include "webpage.h"
#include "Logger.h"
#include "Utils.h"
#include "StatusLed.h"
#include "Filesystem.h"
#include "Rtc.h"
#include "Oled.h"
#include "Mcp.h"
#include "Lock.h"
// #include "EnclosureTamper.h"
// #include "FireSensor.h"
// #include "ConfigButton.h"
#include "Reader.h"
#include "Button.h"
#include "Web.h"
#include "Network.h"
// #include "Buffer.h" // !
// #include "Alarm.h" // TODO

// ! on lock array make common wire optional to internal 12V by jumper

// ! split enter/exit site into channels -> on insert/replace card handle can_enter/can_exit based on channel

// ! internal buzzer
// ! external alarm siren/lamp

// ! external output for card success read for both readers -> eg. activation and deactivation of alarm

// ! sensor for poe power and external power

// ! when powered on but no ethernet cable connected -> readers are deleyed -> fast swiping fail card over both readers -> only first reader triggered, the second one is blocked
  // ! when ethernet cable connected and mqtt connected -> swiping fail card over both readers -> both readers triggered with minimal delay between reads (both fail | first fail & second panic)

// ! ! ! - https://www.freertos.org/RTOS-task-priority.html

// ! define YYSTACKDEPTH changed from 20 to 50 inside lib/SqliteDatabase/config_ext.h
// ! WT32-ETH01
  // ! GPIO12 hack needs to be applied

// ----------------------------------------------------------------------------------------------------------------

const int config_button_debounce = DEFAULT_CONFIG_BUTTON_DEBOUNCE;
unsigned long config_button_period = 5000;
bool config_button_pressed = false;
bool config_button_active = false;
int config_button_last_state = 0;
unsigned long config_button_last_time = 0;

Lock* lock = nullptr;  // !!!  - as single instance
Reader* reader_1 = nullptr;
Reader* reader_2 = nullptr;
Button* button_1 = nullptr;
Button* button_2 = nullptr;
// EnclosureTamper* enclosure_tamper = nullptr; // TODO - as single instance
// FireSensor* fire_sensor = nullptr; // TODO - as single instance
TaskHandle_t core_0_task_handle = nullptr;
TaskHandle_t core_1_task_handle = nullptr;
TaskHandle_t captive_task_handle = nullptr;

// ----------------------------------------------------------------------------------------------------------------

// void enter_config_mode(); // ? - config mode without reboot

void handle_on_boot_config_button() { // ? - config mode without reboot
  if (digitalRead(ESP_CONFIG_BUTTON) == LOW) {
    filesystem->switchToConfigMode();
    // enter_config_mode(); // ? - config mode without reboot
  }
}

void config_button_handler() { // debounce not working !!! // ? - config mode without reboot
  int config_button_current_state = digitalRead(ESP_CONFIG_BUTTON);
  if (config_button_current_state != config_button_last_state) {
    if ((millis() - config_button_last_time) >= config_button_debounce) {
      if (config_button_current_state == LOW) {
        if (!config_button_pressed) {
          config_button_pressed = true;
        }
      } else {
        if (config_button_pressed) {
          config_button_active = false;
          config_button_pressed = false;
        }
      }
      config_button_last_state = config_button_current_state;
      config_button_last_time = millis();
    }
  } else {
    if (config_button_current_state == LOW) {
      if  (!config_button_active) {
        if (config_button_pressed) {
          if ((millis() - config_button_last_time) > config_button_period) {
            config_button_active = true;
            lock->block();
            button_1->block();
            button_2->block();
            reader_1->block();
            reader_2->block();
            reader_1->buzzerOnOverride(100);
            reader_2->buzzerOnOverride(100);
            delay(200);
            filesystem->switchToConfigMode();
            // enter_config_mode(); // ? - config mode without reboot
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------------------------------------------

void readers_fatal_error(/*String*/const char* card_value, /*String*/const char* card_id, int reader_id) {
  switch (reader_id) {
    case 1:
      reader_1->errorSequence();
      // network->publishMqttEventMessage(event_error, event_reader_1, true, card_value, true, card_id, true, "Error while reading details from device database", event_level_error, event_type_access);
      network->publishMqttEventMessage(event_error, event_reader_1, card_value, card_id, "Error while reading details from device database", event_level_error, event_type_access);
      break;
    case 2:
      reader_2->errorSequence();
      // network->publishMqttEventMessage(event_error, event_reader_2, true, card_value, true, card_id, true, "Error while reading details from device database", event_level_error, event_type_access);
      network->publishMqttEventMessage(event_error, event_reader_2, card_value, card_id, "Error while reading details from device database", event_level_error, event_type_access);
      break;
  }
}

void readers_error(/*String*/const char* card_value, /*String*/const char* card_id, int reader_id, /*String*/const char* message) {
  switch (reader_id) {
    case 1:
      reader_1->error();
      // network->publishMqttEventMessage(event_error, event_reader_1, true, card_value, true, card_id, true, message, event_level_error, event_type_access);
      network->publishMqttEventMessage(event_error, event_reader_1, card_value, card_id, message, event_level_error, event_type_access);
      break;
    case 2:
      reader_2->error();
      // network->publishMqttEventMessage(event_error, event_reader_2, true, card_value, true, card_id, true, message, event_level_error, event_type_access);
      network->publishMqttEventMessage(event_error, event_reader_2, card_value, card_id, message, event_level_error, event_type_access);
      break;
  }
}

void readers_success(/*String*/const char* card_value, /*String*/const char* card_id, int reader_id) {
  switch (reader_id) {
    case 1:
      reader_1->success(true, -1, lock, reader_2);
      // network->publishMqttEventMessage(event_success, event_reader_1, true, card_value, true, card_id, true, event_ok, event_level_info, event_type_access);
      network->publishMqttEventMessage(event_success, event_reader_1, card_value, card_id, event_ok, event_level_info, event_type_access);
      break;
    case 2:
      reader_2->success(true, -1, lock, reader_1);
      // network->publishMqttEventMessage(event_success, event_reader_2, true, card_value, true, card_id, true, event_ok, event_level_info, event_type_access);
      network->publishMqttEventMessage(event_success, event_reader_2, card_value, card_id, event_ok, event_level_info, event_type_access);
      break;
  }
}

void check_card_direction_restrictions(Card card, int reader_id) { // ! change check -
  switch (reader_id) {
    case 1:
        if (card.can_enter == 1) {
          readers_success(card.value.c_str(), card.id.c_str(), reader_id);
        } else {
          readers_error(card.value.c_str(), card.id.c_str(), reader_id, "Restrictions violated: Not allowed");
        }
      break;
    case 2:
        if (card.can_exit == 1) {
          readers_success(card.value.c_str(), card.id.c_str(), reader_id);
        } else {
          readers_error(card.value.c_str(), card.id.c_str(), reader_id, "Restrictions violated: Not allowed");
        }
      break;
  }
}

void find_card(/*String*/const char* card_value, int reader_id) { // - possibly more restrictions (days in week, time windows in each day, days in year) ??
  int card_exist = filesystem->cardExistByValue(card_value);
  if (card_exist != -1) {
    if (card_exist == 1) { // card exist in database

      // ! change check -

      Card card = filesystem->getCardByValue(card_value);
      if (card.success /*&& card.id.length() > 0*/) {
        if (card.has_validity == 1) { // validity restrictions
          if (card.valid_since <= card.current_time) {
            if (card.valid_until >= card.current_time) {
              check_card_direction_restrictions(card, reader_id);
            } else {
              readers_error(card.value.c_str(), card.id.c_str(), reader_id, "Restrictions violated: Not longer valid");
            }
          } else {
            readers_error(card.value.c_str(), card.id.c_str(), reader_id, "Restrictions violated: Not yet valid");
          }
        } else { // no validity restrictions
          check_card_direction_restrictions(card, reader_id);
        }
      }
    } else { // card does not exist in database
      readers_error(card_value, nullptr, reader_id, "Not in device database");
    }
  } else { // error while reading card details from device database
    readers_fatal_error(card_value, nullptr, reader_id);
  }
}

void reader_state_changed(bool plugged, const char* param) {
  int reader_id = atoi(param);
  switch (reader_id) {
    case 1:
      if (plugged) {
        reader_1->setConnectionState(true);
        network->publishMqtt(mqtt_topic_reader_1_status, mqtt_payload_reader_1_connected, true, /*1*/0);
      } else {
        reader_1->setConnectionState(false);
        network->publishMqtt(mqtt_topic_reader_1_status, mqtt_payload_reader_1_disconnected, true, /*1*/0);
      }
      break;
    case 2:
      if (plugged) {
        reader_2->setConnectionState(true);
        network->publishMqtt(mqtt_topic_reader_2_status, mqtt_payload_reader_2_connected, true, /*1*/0);
      } else {
        reader_2->setConnectionState(false);
        network->publishMqtt(mqtt_topic_reader_2_status, mqtt_payload_reader_2_disconnected, true, /*1*/0);
      }
      break;
  }
}

void reader_receive_error(Wiegand::DataError error, uint8_t* data, uint8_t bits, const char* param) {
  int reader_id = atoi(param);
  switch (reader_id) {
    case 1:
      reader_1->processError(error, data, bits);
      break;
    case 2:
      reader_2->processError(error, data, bits);
      break;
  }
}

void reader_receive_success(uint8_t* data, uint8_t bits, const char* param) {
  int reader_id = atoi(param);
  switch (reader_id) {
    case 1:
      find_card(reader_1->processSuccess(data, bits).c_str(), reader_id);
      break;
    case 2:
      find_card(reader_2->processSuccess(data, bits).c_str(), reader_id);
      break;
  }
}

void mqtt_lock_request_handler(MqttLockCommand command) {
  switch(command) {
    case MqttLockCommand::NA:
      break;
    case MqttLockCommand::Idle:
      reader_1->normal();
      reader_2->normal();
      button_1->normal();
      button_2->normal();
      lock->normal();
      break;
    case MqttLockCommand::Block:
      reader_1->block();
      reader_2->block();
      button_1->block();
      button_2->block();
      lock->block();
      break;
    case MqttLockCommand::Bypass:
      reader_1->bypass();
      reader_2->bypass();
      button_1->bypass();
      button_2->bypass();
      lock->bypass();
      break;
    case MqttLockCommand::Error:
      reader_1->errorSequence();
      reader_2->errorSequence();
      break;
    case MqttLockCommand::PassSuccessReader1:
      reader_1->success(true, -1, lock, reader_2);
      break;
    case MqttLockCommand::PassSuccessReader2:
      reader_2->success(true, -1, lock, reader_1);
      break;
    case MqttLockCommand::PassErrorReader1:
      reader_1->error();
      break;
    case MqttLockCommand::PassErrorReader2:
      reader_2->error();
      break;
  }
}

// ----------------------------------------------------------------------------------------------------------------

void core_0_task(void * parameter) {
  for( ;; ) {
    network->handle();
    if (network->mqttInit()) {
      lock->publishMqttInit();
      // enclosure_tamper->publishMqttInit();
      // fire_sensor->publishMqttInit();
      reader_1->publishMqttInit();
      reader_2->publishMqttInit();
      network->setMqttInit(false);
    }
    if (network->mqttLockTrigger()) {
      mqtt_lock_request_handler(network->mqttLockCommand());
      network->mqttLockTriggerClear();
    }
    if (utils->rebootNededTrigger()) { // ! ?
      network->publishMqtt(mqtt_topic_reboot_status, mqtt_payload_reboot_status_needed, true, /*1*/0);
      // network->publishMqttEventMessage(event_reboot, event_power, false, "", false, "", true, event_reboot_pending, event_level_info, event_type_device);
      network->publishMqttEventMessage(event_reboot, event_power, "", "", event_reboot_pending, event_level_info, event_type_device);
      if (filesystem->getKeepWebserverOutsideConfigMode()) {
        web->sendRebootStatusToAll();
      }
      utils->setRebootNededTrigger(false);
    }
    config_button_handler();

    // unsigned int usage = uxTaskGetStackHighWaterMark(NULL); // !
    // Log->print("[DEBUG] core_0_task - stack usage: "); // !
    // Log->println(usage); // !
  }
}

void core_1_task(void * parameter) { // TODO - add enclosure_tamper_alarm into condition
  for( ;; ) {
    mcp->handle(lock, reader_1, reader_2, button_1, button_2/*, enclosure_tamper, fire_sensor*/); // ?
    lock->handleDoorSensor();
    // enclosure_tamper->handle();
    // fire_sensor->handle();
    button_1->handle(lock, button_2, reader_1, reader_2); // ?
    button_2->handle(lock, button_1, reader_1, reader_2); // ?
    reader_1->handle();
    reader_2->handle();

    // unsigned int usage = uxTaskGetStackHighWaterMark(NULL); // !
    // Log->print("[DEBUG] core_1_task - stack usage: "); // !
    // Log->println(usage); // !
  }
}

void normal_boot(bool coldBoot) { // ?
  if (filesystem->getKeepWebserverOutsideConfigMode()) {
    web->init(false);
  } else {
    web->disable();
  }
  lock->normal(); // ? what if mqtt lock commands ???
  button_1->normal(); // ? what if mqtt lock commands ???
  button_2->normal(); // ? what if mqtt lock commands ???
  reader_1->wiegand()->onReceive(reader_receive_success, "1");
  reader_1->wiegand()->onReceiveError(reader_receive_error, "1");
  reader_1->wiegand()->onStateChange(reader_state_changed, "1");
  reader_1->init(); // ? what if mqtt lock commands ???
  reader_2->wiegand()->onReceive(reader_receive_success, "2");
  reader_2->wiegand()->onReceiveError(reader_receive_error, "2");
  reader_2->wiegand()->onStateChange(reader_state_changed, "2");
  reader_2->init(); // ? what if mqtt lock commands ???
  if (coldBoot) {
    filesystem->logBoot(false);
  }
  oled->setup();
  // hwAlarm->check(); // TODO
  statusLed->setConfigMode(false);
  statusLed->setBooting(false);
  xTaskCreatePinnedToCore(core_0_task, "core_0_task", 6144, NULL, 0, &core_0_task_handle, 0);
  xTaskCreatePinnedToCore(core_1_task, "core_1_task", 8192, NULL, 2/*3*/, &core_1_task_handle, 1); // ?
  if (!coldBoot) {
    if (captive_task_handle != NULL) {
      vTaskDelete(captive_task_handle);
    }
  }
}

void exit_config_mode() { // ? check aswell preference_was_configured
  web->setConfigExitTrigger(false);
  if (filesystem->getConfigMode()) {
    utils->printSwitchFromConfigMode();
    filesystem->setConfigMode(false);
    normal_boot(false);
  }
}

void captive_task(void * parameter) {
  for( ;; ) {
    if (!filesystem->rsaKeyPairExist()) { // ! maybe start separated task for check and generate
      filesystem->generateRsaKeypair();
    }
    web->handleCaptive();
    if (web->configSaveDoneTrigger()) {
      reader_1->configSequenceEnd();
      reader_2->configSequenceEnd();
      web->setConfigSaveDoneTrigger(false);
    }
    if (web->configExitTrigger()) {
      reader_1->configSequenceEnd();
      reader_2->configSequenceEnd();
      exit_config_mode();
    }
    if (utils->rebootNededTrigger()) { // ! ?
      web->sendRebootStatusToAll();
      utils->setRebootNededTrigger(false);
    }
  }
}

void config_boot() {
  lock->block();
  // // ? - what to do with enclosure_tamper and fire_sensor in config mode ??
  button_1->block();
  button_2->block();
  reader_1->configSequence();
  reader_2->configSequence();
  web->init(true);
  filesystem->logBoot(true);
  oled->setup();
  statusLed->setConfigMode(true);
  statusLed->setBooting(false);
  xTaskCreatePinnedToCore(captive_task, "captive_task", /*16384*/20480, NULL, 1, &captive_task_handle, 1); // ! stack size
  if (core_1_task_handle != NULL) {
    vTaskDelete(core_1_task_handle);
  }
  if (core_0_task_handle != NULL) {
    vTaskDelete(core_0_task_handle);
  }
}

// void enter_config_mode() { // ? - config mode without reboot
//   reader_1->block();
//   reader_2->block();
//   reader_1->buzzerOnOverride(100);
//   reader_2->buzzerOnOverride(100);
//   utils->printSwitchToConfigMode();
//   filesystem->setConfigMode(true);
//   delay(200);
//   config_boot();
// }

void setup() { // ?
  Serial.begin(115200);
  Log = &Serial;
  // buffer->init(12288); // ?
  statusLed->init();
  statusLed->setBooting(true);
  utils->printInit();
  oled->init();
  mcp->init();
  rtc->init();
  filesystem->init();
  network->init();
  lock = new Lock(); // !!!
  // enclosure_tamper = new EnclosureTamper(); // ?
  // fire_sensor = new FireSensor(); // ?
  reader_1 = new Reader(1);
  reader_2 = new Reader(2);
  button_1 = new Button(1);
  button_2 = new Button(2);
  if (!filesystem->getConfigMode()) {
    handle_on_boot_config_button();
    normal_boot(true);
  } else {
    config_boot();
  }
}

void loop() {
  vTaskDelete(NULL);
}
