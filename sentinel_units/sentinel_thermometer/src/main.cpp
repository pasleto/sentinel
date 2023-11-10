#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "defaults.h"
#include "topics.h"
#include "Logger.h"
#include "Utils.h"
#include "Filesystem.h"
#include "Rtc.h"
#include "Web.h"
#include "Network.h"
#include "Thermometer.h"

// ! define YYSTACKDEPTH changed from 20 to 50 inside lib/SqliteDatabase/config_ext.h
// ! GPIO12 hack needs to be applied

// ----------------------------------------------------------------------------------------------------------------

const int config_button_debounce = DEFAULT_CONFIG_BUTTON_DEBOUNCE;
unsigned long config_button_period = 5000;
bool config_button_pressed = false;
bool config_button_active = false;
int config_button_last_state = 0;
unsigned long config_button_last_time = 0;

Thermometer* thermometer_1 = nullptr;
Thermometer* thermometer_2 = nullptr;

TaskHandle_t core_0_task_handle = nullptr;
TaskHandle_t core_1_task_handle = nullptr;
TaskHandle_t captive_task_handle = nullptr;

// ----------------------------------------------------------------------------------------------------------------

void handle_on_boot_config_button() {
  if (digitalRead(ESP_CONFIG_BUTTON) == LOW) {
    filesystem->switchToConfigMode();
  }
}

void config_button_handler() { // ? - debounce not working !!!
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
            filesystem->switchToConfigMode();
          }
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------------------------------------------

void core_0_task(void * parameter) {
  for( ;; ){
    network->handle();
    if (network->mqttInit()) {
      thermometer_1->publishMqttInit();
      thermometer_2->publishMqttInit();
      network->setMqttInit(false);
    }
    config_button_handler();
  }
}

void core_1_task(void * parameter) {
  for( ;; ){
    thermometer_1->handle(); // ?
    thermometer_2->handle(); // ?
  }
}

void captive_task(void * parameter) {
  for( ;; ) {
    web->handleCaptive();
    if (web->configSaveDoneTrigger()) {
      web->setConfigSaveDoneTrigger(false);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Log = &Serial;
  utils->printInit();
  rtc->init();
  filesystem->init();
  network->init();
  thermometer_1 = new Thermometer(1);
  thermometer_2 = new Thermometer(2);
  if (!filesystem->configMode()) {
    handle_on_boot_config_button();
    if (filesystem->keepWebserverOutsideConfigMode()) {
      web->init(false);
    }
    filesystem->logBoot(false);
    xTaskCreatePinnedToCore(core_0_task, "core_0_task", 8192, NULL, 0, &core_0_task_handle, 0); // 16384
    xTaskCreatePinnedToCore(core_1_task, "core_1_task", 8192, NULL, 2, &core_1_task_handle, 1); // 16384
  } else {
    web->init(true);
    filesystem->logBoot(true);
    xTaskCreatePinnedToCore(captive_task, "captive_task", 8192, NULL, 1, &captive_task_handle, 1); // 16384
  }
}

void loop() {
  vTaskDelete(NULL);
}
