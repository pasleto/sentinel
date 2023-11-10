#pragma once

#define DEFAULT_DEVICE_ADMIN_USERNAME               "admin"
#define DEFAULT_DEVICE_ADMIN_PASSWORD               "password"
#define DEFAULT_CONFIG_BUTTON_DEBOUNCE              250     // ??? not working
#define DEFAULT_WAS_CONFIGURED                      false
#define DEFAULT_KEEP_WEBSERVER                      true
#define DEFAULT_CONFIG_MODE                         true
#define DEFAULT_SSL_USE                             false
#define DEFAULT_SSL_CA_CERTIFICATE                  ""
#define DEFAULT_SSL_VALIDATE_CLIENT                 false   // TODO
#define DEFAULT_SSL_CLIENT_CERTIFICATE              ""      // TODO
#define DEFAULT_SSL_CLIENT_PRIVATE_KEY              ""      // TODO
#define DEFAULT_MQTT_SERVER                         ""
#define DEFAULT_MQTT_PORT                           1883
#define DEFAULT_NTP_SERVER                          "pool.ntp.org"
#define DEFAULT_TIMEZONE                            "UTC0"
#define DEFAULT_TIMEZONE_NAME                       "UTC"
#define DEFAULT_DHCP_STATIC                         false
#define DEFAULT_DHCP_IP                             ""
#define DEFAULT_DHCP_MASK                           ""
#define DEFAULT_DHCP_GW                             ""
#define DEFAULT_DHCP_DNS                            ""
#define DEFAULT_MQTT_CREDENTIALS                    false
#define DEFAULT_MQTT_USER                           ""
#define DEFAULT_MQTT_PASS                           ""
#define DEFAULT_READER                              false   // ! - todo
#define DEFAULT_READER_KEYPAD                       false   // ! - todo
#define DEFAULT_READER_REVERSE                      false
#define DEFAULT_READER_SILENT                       false
#define DEFAULT_READER_TAMPER_SENSOR                false
#define DEFAULT_READER_TAMPER_INVERT                false   // default trigger is by pulling pin to ground - idle pulled to vcc by pull-up || pulled to ground is idle - trigger by openning the circuit -> pull to vcc by pull-up
#define DEFAULT_READER_TAMPER_RESTORE               true
#define DEFAULT_READER_TAMPER_ALARM                 false   // ???
#define DEFAULT_READER_BUZZER_TIMEOUT               150
#define DEFAULT_READER_TIMEOUT                      3000
#define DEFAULT_READER_PANIC_TIMEOUT                500
#define DEFAULT_READER_ERROR_TIMEOUT                750
#define DEFAULT_READER_TAMPER_DEBOUNCE              150
#define DEFAULT_BUTTON                              false   // ! - todo
#define DEFAULT_BUTTON_TIMEOUT                      5000
#define DEFAULT_BUTTON_DEBOUNCE                     150
#define DEFAULT_DOOR_SENSOR                         false
#define DEFAULT_DOOR_SENSOR_STATE                   false
#define DEFAULT_DOOR_SENSOR_DEBOUNCE                150
#define DEFAULT_DOOR_ALARM                          false   // TODO // if enabled
#define DEFAULT_DOOR_ALARM_STATE                    false   // TODO // holding state
#define DEFAULT_DOOR_ALARM_TIMEOUT                  5000    // TODO
#define DEFAULT_DOOR_ALARM_RESTORE                  true    // TODO ???
#define DEFAULT_EXTERNAL_ALARM                      false   // TODO // if enabled
#define DEFAULT_EXTERNAL_ALARM_STATE                false   // TODO // holding state
#define DEFAULT_ENCLOSURE_TAMPER_SENSOR             false   // TODO
#define DEFAULT_ENCLOSURE_TAMPER_INVERT             false   // TODO
#define DEFAULT_ENCLOSURE_TAMPER_RESTORE            true    // TODO
#define DEFAULT_ENCLOSURE_TAMPER_ALARM              false   // TODO
#define DEFAULT_ENCLOSURE_TAMPER_DEBOUNCE           150     // TODO
#define DEFAULT_FIRE_SENSOR                         false   // TODO
#define DEFAULT_FIRE_SENSOR_INVERT                  false   // TODO
#define DEFAULT_FIRE_SENSOR_RESTORE                 true    // TODO
#define DEFAULT_FIRE_SENSOR_ALARM                   false   // TODO
#define DEFAULT_FIRE_SENSOR_DEBOUNCE                150     // TODO
