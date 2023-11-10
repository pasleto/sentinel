#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "defaults.h"
#include "topics.h"

#include "Logger.h"
#include "Utils.h"
#include "Filesystem.h"
#include "Rtc.h"
#include "Mpu.h"
#include "Obd.h"
#include "Reader.h"
#include "Web.h"
#include "Network.h"

// ! define YYSTACKDEPTH changed from 20 to 50 inside lib/SqliteDatabase/config_ext.h
// ! - 4MB vs 16MB board -> change configuration of platformio.ini


// ! - fix card reader fake data incoming on power the reader / boot
// ! - ledc: ledc_get_duty(739): LEDC is not initialized -> while calling tone sometimes


// ----------------------------------------------------------------------------------------------------------------

const int config_button_debounce = DEFAULT_CONFIG_BUTTON_DEBOUNCE;
unsigned long config_button_period = 5000;
bool config_button_pressed = false;
bool config_button_active = false;
int config_button_last_state = 0;
unsigned long config_button_last_time = 0;

// ----------------------------------------------------------------------------------------------------------------

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
  for( ;; ) {
    network->handle(); // ?
    config_button_handler(); // ?
  }
}

void core_1_task(void * parameter) {
  for( ;; ) {
    // network->handle(); // ?
    network->gpsHanle();
    obd->handle();
  }
}

void captive_task(void * parameter) {
  for( ;; ) {
    web->handleCaptive();
  }
}

void setup() {
  Serial.begin(115200); // Serial.begin(9600);
  Log = &Serial;
  utils->printInit();
  rtc->init();
  filesystem->init();
  if (!filesystem->configMode()) {
    handle_on_boot_config_button();
    network->ledPrebootSequence();
  //   network->clearWifi();
    network->init();
    if (filesystem->hwRebootRequest()) {
      filesystem->setHwRebootRequest(false);
      utils->printHardwareRestart(); // ? - + log hw reboot
      utils->playHwRebootBeep();
      utils->hwReboot();
    } else {
      network->ledBootDoneSequence();
      mpu->init();
      obd->init();
      reader->init();
      filesystem->logBoot(false);
      xTaskCreatePinnedToCore(core_0_task, "core_0_task", 8192, NULL, 0, &core_0_task_handle, 0); // 16384
      xTaskCreatePinnedToCore(core_1_task, "core_1_task", 8192, NULL, 2, &core_1_task_handle, 1); // 16384
    }
  } else {
    utils->playConfigEntryBeep();
    network->ledSetupSequence();
    web->init(true);
    filesystem->logBoot(true);
    xTaskCreatePinnedToCore(captive_task, "captive_task", 8192, NULL, 2, &captive_task_handle, 1); // 16384
  }
}

void loop() {
  vTaskDelete(NULL);
}
