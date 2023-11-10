#pragma once

#define event_reader_1                                  "READER_1"
#define event_reader_2                                  "READER_2"
#define event_reader_1_tamper_current                   "READER_1_TAMPER_EVENT"
#define event_reader_1_tamper_saved                     "READER_1_TAMPER"
#define event_reader_2_tamper_current                   "READER_2_TAMPER_EVENT"
#define event_reader_2_tamper_saved                     "READER_2_TAMPER"
#define event_reader_1_tamper_state_normal              "NORMAL"
#define event_reader_1_tamper_state_alarm               "ALARM"
#define event_reader_2_tamper_state_normal              "NORMAL"
#define event_reader_2_tamper_state_alarm               "ALARM"
#define event_button_1                                  "BUTTON_1"
#define event_button_2                                  "BUTTON_2"
#define event_button_1_state_trigger                    "TRIGGER"
#define event_button_2_state_trigger                    "TRIGGER"
#define event_door_sensor                               "DOOR"
#define event_door_sensor_state_open                    "OPEN"
#define event_door_sensor_state_closed                  "CLOSED"
// #define event_door_alarm                                "DOOR_ALARM"
#define event_door_alarm_current                        "DOOR_ALARM_EVENT" // TODO
#define event_door_alarm_saved                          "DOOR_ALARM" // TODO
#define event_door_alarm_state_on                       "ON"
#define event_door_alarm_state_off                      "OFF"
#define event_external_alarm                            "ALARM" // TODO
#define event_external_alarm_state_on                   "ON" // TODO
#define event_external_alarm_state_off                  "OFF" // TODO
#define event_enclosure_tamper_current                  "ENCLOSURE_TAMPER_EVENT"  // ?
#define event_enclosure_tamper_saved                    "ENCLOSURE_TAMPER"  // ?
#define event_enclosure_tamper_state_normal             "NORMAL"  // ?
#define event_enclosure_tamper_state_alarm              "ALARM" // ?
#define event_fire_sensor_current                       "FIRE_SENSOR_EVENT" // ?
#define event_fire_sensor_saved                         "FIRE_SENSOR" // ?
#define event_fire_sensor_state_normal                  "NORMAL"  // ?
#define event_fire_sensor_state_alarm                   "ALARM" // ?

#define event_rtc                                       "RTC"
#define event_rtc_message                               "RTC Lost Power - Battery Error"
#define event_error                                     "ERROR"
#define event_boot                                      "BOOT"
#define event_reboot                                    "REBOOT"
#define event_power                                     "POWER"
#define event_reboot_initiated                          "Initiated"
#define event_reboot_pending                            "Pending"
#define event_update                                    "UPDATE"
#define event_firmware                                  "FIRMWARE"
#define event_config                                    "CONFIG"
#define event_change                                    "CHANGE"

#define event_success                                   "SUCCESS"
#define event_ok                                        "OK"

#define event_level_info                                "INFO"
#define event_level_warning                             "WARNING"
#define event_level_error                               "ERROR"
#define event_type_device                               "DEVICE"
#define event_type_access                               "ACCESS"
