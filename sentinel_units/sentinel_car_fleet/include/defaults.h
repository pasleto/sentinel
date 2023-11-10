#pragma once

#define DEFAULT_DEVICE_ADMIN_USERNAME               "admin"
#define DEFAULT_DEVICE_ADMIN_PASSWORD               "password"
#define DEFAULT_WAS_CONFIGURED                      false
#define DEFAULT_CONFIG_MODE                         true

#define DEFAULT_SSL_USE                             false
#define DEFAULT_SSL_CA_CERTIFICATE                  ""
#define DEFAULT_SSL_VALIDATE_CLIENT                 false   // TODO
#define DEFAULT_SSL_CLIENT_CERTIFICATE              ""      // TODO
#define DEFAULT_SSL_CLIENT_PRIVATE_KEY              ""      // TODO
#define DEFAULT_MQTT_SERVER                         ""      // ? - mqtt-server.local
#define DEFAULT_MQTT_PORT                           1883
#define DEFAULT_NTP_SERVER                          "pool.ntp.org" // ?
#define DEFAULT_TIMEZONE                            "UTC0"
#define DEFAULT_TIMEZONE_NAME                       "UTC"
#define DEFAULT_MQTT_CREDENTIALS                    false
#define DEFAULT_MQTT_USER                           ""
#define DEFAULT_MQTT_PASS                           ""

#define DEFAULT_HW_REBOOT                           false

#define DEFAULT_GNSS_MODE                           5           // GLONASS + GALILEO
#define DEFAULT_NETWORK_MODE                        2           // 2 - Automatic | 13 - GSM Only | 38 - LTE Only | 48 - Any mode but LTE | 51 GSM and LTE Only
#define DEFAULT_GPRS_APN                            ""
#define DEFAULT_GPRS_CREDENTIALS                    false
#define DEFAULT_GPRS_USER                           ""
#define DEFAULT_GPRS_PASS                           ""

#define DEFAULT_CONFIG_BUTTON_DEBOUNCE              250 // ???
