#pragma once

#define mqtt_topic_status                               "/status"
#define mqtt_payload_status_birth                       "ONLINE"
#define mqtt_payload_status_lwt                         "OFFLINE"
#define mqtt_topic_door_sensor                          "/sensor/door/status"
#define mqtt_payload_door_sensor_open                   "OPEN"
#define mqtt_payload_door_sensor_closed                 "CLOSED"
#define mqtt_payload_door_sensor_na                     "null"
#define mqtt_topic_door_alarm_current                   "/sensor/door/alarm/current" // TODO
#define mqtt_topic_door_alarm_saved                     "/sensor/door/alarm/saved" // TODO
#define mqtt_payload_door_alarm_on                      "ON"
#define mqtt_payload_door_alarm_off                     "OFF"
#define mqtt_payload_door_alarm_na                      "null"
#define mqtt_topic_external_alarm                       "/sensor/alarm" // TODO
#define mqtt_payload_external_alarm_on                  "ON" // TODO
#define mqtt_payload_external_alarm_off                 "OFF" // TODO
#define mqtt_payload_external_alarm_na                  "null" // TODO
#define mqtt_topic_reader_1_status                      "/sensor/reader/1/status"
#define mqtt_payload_reader_1_connected                 "CONNECTED"
#define mqtt_payload_reader_1_disconnected              "DISCONNECTED"
#define mqtt_topic_reader_1_tamper_current              "/sensor/reader/1/tamper/current"
#define mqtt_topic_reader_1_tamper_saved                "/sensor/reader/1/tamper/saved"
#define mqtt_payload_reader_1_tamper_alarm              "ALARM"
#define mqtt_payload_reader_1_tamper_normal             "NORMAL"
#define mqtt_payload_reader_1_tamper_na                 "null"
#define mqtt_topic_reader_2_status                      "/sensor/reader/2/status"
#define mqtt_payload_reader_2_connected                 "CONNECTED"
#define mqtt_payload_reader_2_disconnected              "DISCONNECTED"
#define mqtt_topic_reader_2_tamper_current              "/sensor/reader/2/tamper/current"
#define mqtt_topic_reader_2_tamper_saved                "/sensor/reader/2/tamper/saved"
#define mqtt_payload_reader_2_tamper_alarm              "ALARM"
#define mqtt_payload_reader_2_tamper_normal             "NORMAL"
#define mqtt_payload_reader_2_tamper_na                 "null"
#define mqtt_topic_enclosure_tamper_current             "/sensor/enclosure/tamper/current" // ?
#define mqtt_topic_enclosure_tamper_saved               "/sensor/enclosure/tamper/saved" // ?
#define mqtt_payload_enclosure_tamper_alarm             "ALARM" // ?
#define mqtt_payload_enclosure_tamper_normal            "NORMAL" // ?
#define mqtt_payload_enclosure_tamper_na                "null" // ?
#define mqtt_topic_fire_sensor_current                  "/sensor/fire/current" // ?
#define mqtt_topic_fire_sensor_saved                    "/sensor/fire/saved" // ?
#define mqtt_payload_fire_sensor_alarm                  "ALARM" // ?
#define mqtt_payload_fire_sensor_normal                 "NORMAL" // ?
#define mqtt_payload_fire_sensor_na                     "null" // ?

#define mqtt_topic_storage_percentage                   "/storage/percentage"
#define mqtt_topic_storage_total                        "/storage/total"
#define mqtt_topic_storage_used                         "/storage/used"
#define mqtt_topic_storage_free                         "/storage/free"
#define mqtt_topic_rtc_warning                          "/sensor/rtc/warning"
#define mqtt_payload_rtc_warning                        "WARNING"
#define mqtt_payload_rtc_normal                         "NORMAL"
#define mqtt_topic_network_mode                         "/network/mode"
#define mqtt_topic_network_hostname                     "/network/hostname"
#define mqtt_topic_network_ip                           "/network/ip"
#define mqtt_topic_network_mask                         "/network/mask"
#define mqtt_topic_network_cidr                         "/network/cidr"
#define mqtt_topic_network_gw                           "/network/gateway"
#define mqtt_topic_network_dns                          "/network/dns"
#define mqtt_topic_network_speed                        "/network/speed"
#define mqtt_topic_network_duplex                       "/network/duplex"
#define mqtt_topic_firmware                             "/info/firmware"
#define mqtt_topic_hardware                             "/info/hardware"
#define mqtt_topic_boot_unix                            "/info/boot/unix"
#define mqtt_topic_boot_timestamp                       "/info/boot/timestamp"
#define mqtt_topic_reboot_status                        "/sensor/reboot"
#define mqtt_payload_reboot_status_needed               "NEEDED"
#define mqtt_payload_reboot_status_ok                   "OK"

#define mqtt_topic_event                                "/event"



#define mqtt_topic_database_card_request                "/db/card/request"
#define mqtt_topic_database_card_response               "/db/card/response"
#define mqtt_topic_lock_command                         "/command/lock"
#define mqtt_topic_sensor_restore_command               "/command/sensor/restore" // TODO
// #define mqtt_topic_reader_1_tamper_restore_command      "/command/reader/1/tamper/restore" // TODO
// #define mqtt_topic_reader_2_tamper_restore_command      "/command/reader/2/tamper/restore" // TODO
// #define mqtt_topic_enclosure_tamper_restore_command     "/command/enclosure/tamper/restore" // TODO
// #define mqtt_topic_fire_sensor_restore_command          "/command/fire/restore" // TODO

#define mqtt_topic_reboot_command                       "/command/reboot" // TODO

#define mqtt_topic_sync_config                          "/sync/config" // TODO

// #define mqtt_topic_xxx                              "/xxx"
