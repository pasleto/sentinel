#pragma once

#define preference_firmware_version               "firmware_ver"
#define preference_hardware_version               "hardware_ver"
#define preference_rsa_pub                        "rsa_pub"             // ?
#define preference_rsa_priv                       "rsa_priv"            // ?
#define preference_hostname                       "hostname"
#define preference_ip_address                     "ip_address"          // !
#define preference_device_username                "device_username"
#define preference_device_password                "device_password"
#define preference_ip_static                      "ip_static"
#define preference_ip_static_ip                   "ip_static_ip"
#define preference_ip_static_mask                 "ip_static_mask"
#define preference_ip_static_gw                   "ip_static_gw"
#define preference_ip_static_dns                  "ip_static_dns"
#define preference_button_1                       "btn_1"               // ! - todo - whether the button 1 is attached / enabled
#define preference_button_1_timeout               "btn_1_timeout"
#define preference_button_2                       "btn_2"               // ! - todo - whether the button 2 is attached / enabled
#define preference_button_2_timeout               "btn_2_timeout"
#define preference_door_sensor                    "door_sensor"
#define preference_door_sensor_state              "door_sensor_val"
#define preference_door_alarm                     "door_alarm"          // TODO
#define preference_door_alarm_state               "door_alarm_val"      // TODO
#define preference_door_alarm_restore             "door_alarm_res"      // TODO
#define preference_door_alarm_timeout             "door_alarm_time"     // TODO
#define preference_external_alarm                 "ext_alarm"           // TODO
#define preference_external_alarm_state           "ext_alarm_val"       // TODO
#define preference_reader_1                       "wg_1"                // ! - todo - whether the reader 1 is attached / enabled
#define preference_reader_1_keypad                "wg_1_keypad"         // ! - todo - whether the reader 1 has keypad
#define preference_reader_1_buzzer_timeout        "wg_1_bz_timeout"
#define preference_reader_1_timeout               "wg_1_timeout"
#define preference_reader_1_reverse               "wg_1_reverse"
#define preference_reader_1_silent                "wg_1_silent"
#define preference_reader_1_tamper                "wg_1_tamper"
#define preference_reader_1_tamper_invert         "wg_1_tamper_inv"
#define preference_reader_1_tamper_restore        "wg_1_tamper_res"
#define preference_reader_1_tamper_alarm          "wg_1_tamper_alr"
#define preference_reader_2                       "wg_2"                // ! - todo - whether the reader 2 is attached / enabled
#define preference_reader_2_keypad                "wg_2_keypad"         // ! - todo - whether the reader 2 has keypad
#define preference_reader_2_buzzer_timeout        "wg_2_bz_timeout"
#define preference_reader_2_timeout               "wg_2_timeout"
#define preference_reader_2_reverse               "wg_2_reverse"
#define preference_reader_2_silent                "wg_2_silent"
#define preference_reader_2_tamper                "wg_2_tamper"
#define preference_reader_2_tamper_invert         "wg_2_tamper_inv"
#define preference_reader_2_tamper_restore        "wg_2_tamper_res"
#define preference_reader_2_tamper_alarm          "wg_2_tamper_alr"
#define preference_enclosure_tamper               "encl_tamper"         // ?
#define preference_enclosure_tamper_invert        "encl_tamper_inv"     // ?
#define preference_enclosure_tamper_restore       "encl_tamper_res"     // ?
#define preference_enclosure_tamper_alarm         "encl_tamper_alr"     // ?
#define preference_fire_sensor                    "fire_sensor"         // ?
#define preference_fire_sensor_invert             "fire_sensor_inv"     // ?
#define preference_fire_sensor_restore            "fire_sensor_res"     // ?
#define preference_fire_sensor_alarm              "fire_sensor_alr"     // ?
#define preference_ntp_server                     "ntp_server"
#define preference_timezone                       "time_zone"
#define preference_timezone_name                  "tz_name"
#define preference_mqtt_server                    "mqtt_server"
#define preference_mqtt_port                      "mqtt_port"
#define preference_mqtt_credentials_set           "mqtt_cred"
#define preference_mqtt_username                  "mqtt_user"
#define preference_mqtt_password                  "mqtt_pass"
#define preference_ssl_use                        "ssl_use"
#define preference_ssl_root_ca                    "ssl_root_ca"
#define preference_ssl_validate_client            "ssl_validate_cl"
#define preference_ssl_client_certificate         "ssl_client_cert"
#define preference_ssl_client_private_key         "ssl_client_priv"
#define preference_keep_webserver                 "keep_webserver"
#define preference_was_configured                 "was_configured"
#define preference_config_mode                    "config_mode"
#define preference_last_boot_time_unix            "last_boot_unix"
#define preference_last_boot_time_timestamp       "last_boot_stamp"
#define preference_reboot_needed                  "reboot_needed"
#define preference_reboot_needed_timestamp        "reboot_needed_t"
// #define preference_x "x"
// #define preference_started_before "started_before"