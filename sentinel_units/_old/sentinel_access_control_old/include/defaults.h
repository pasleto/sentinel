#define DEFAULT_DEVICE_ADMIN_USERNAME               "admin"
#define DEFAULT_DEVICE_ADMIN_PASSWORD               "password"
#define DEFAULT_IS_SETUP_DONE                       false
#define DEFAULT_SSL_USE                             false
#define DEFAULT_SSL_CA_CERTIFICATE                  ""
#define DEFAULT_SSL_VALIDATE_CLIENT                 false   // TODO
#define DEFAULT_SSL_CLIENT_CERTIFICATE              ""      // TODO
#define DEFAULT_SSL_CLIENT_PRIVATE_KEY              ""      // TODO
#define DEFAULT_MQTT_SERVER                         "mqtt-server.local"
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
#define DEFAULT_READER_REVERSE                      false
#define DEFAULT_READER_SILENT                       false
#define DEFAULT_READER_TAMPER_SENSOR                false   // TODO - if anti-tamper switch is used
#define DEFAULT_READER_TAMPER_INVERT                false   // TODO - if tamper switch logic is inverted (default trigger is by pulling pin to ground - idle pulled to vcc by pull-up || pulled to ground is idle - trigger by openning the circuit -> pull to vcc by pull-up)
#define DEFAULT_READER_BUZZER_TIMEOUT               150
#define DEFAULT_READER_TIMEOUT                      3000
#define DEFAULT_BUTTON_TIMEOUT                      5000
#define DEFAULT_DOOR_SENSOR                         false
#define DEFAULT_ENCLOSURE_TAMPER_SENSOR             false   // TODO
#define DEFAULT_ENCLOSURE_TAMPER_INVERT             false   // TODO