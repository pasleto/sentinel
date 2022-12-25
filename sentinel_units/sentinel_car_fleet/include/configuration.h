#define TINY_GSM_MODEM_SIM7600
#define SerialMon                                   Serial
#define SerialAT                                    Serial1
#define SerialCardReader                            Serial2



// ----- Generic Variables
#define MODULE_NAME                                 "Sentinel Car Fleet"
#define MODULE_TYPE                                 "STL-CF"
#define MODULE_HARDWARE_VERSION                     "1.0.1"
#define MODULE_FIRMWARE_VERSION                     "1.0.1"
#define FORMAT_FS_IF_FAILED                         true
// ----- Default Values for Variables
#define GSM_PIN                                     ""          // No PIN
#define DEFAULT_HW_REBOOT                           false
#define DEFAULT_IS_SETUP_DONE                       false
#define DEFAULT_BACKEND_SERVER                      ""
#define DEFAULT_HTTP_PORT                           80
#define DEFAULT_MQTT_PORT                           1883
#define DEFAULT_NTP_SERVER                          "pool.ntp.org"
#define DEFAULT_TIMEZONE                            "UTC0"
#define DEFAULT_TIMEZONE_NAME                       "UTC"
#define DEFAULT_GNSS_MODE                           5           // GLONASS + GALILEO
#define DEFAULT_NETWORK_MODE                        2           // Automatic
#define DEFAULT_GPRS_APN                            ""
#define DEFAULT_GPRS_CREDENTIALS                    false
#define DEFAULT_GPRS_USER                           ""
#define DEFAULT_GPRS_PASS                           ""
#define DEFAULT_HTTP_GPS_ENDPOINT                   "/api-call/test" // TODO

#define DEFAULT_MQTT_CREDENTIALS                    false
#define DEFAULT_MQTT_USER                           ""
#define DEFAULT_MQTT_PASS                           ""

#define MQTT_AVAILABILITY_BIRTH                     "ONLINE"
#define MQTT_AVAILABILITY_LWT                       "OFFLINE"
// ----- SIM7600 GSM Module
#define GSM_MODULE_UART_BAUD                        115200
#define GSM_MODULE_RX                               26          // OD/ID
#define GSM_MODULE_TX                               27          // OD/ID
#define GSM_MODULE_POWER                            25          // OD/ID                          POWER_PIN : This pin controls the power supply of the SIM7600
#define GSM_MODULE_PWR                              4           // OD/IE/WPD     strapping pin    PWR_PIN ： This Pin is the PWR-KEY of the SIM7600
#define GSM_MODULE_INT                              36          // OD/ID         ONLY INPUT PIN   INT_PIN: It is connected to the SIM7600 status Pin, through which you can know whether the module starts normally
// ----- ESP Pins

#define ESP_02                                              2           // OD/IE/WPD     strapping pin
#define ESP_05                                              5           // OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_LED                                     12          // OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V
#define ESP_13                                              13          // OD/IE/WPD
#define ESP_WIFI_BUTTON                             14          // OD/IE/WPU
#define ESP_RST                                     15          // OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_18                                              18          // OD/IE
#define ESP_19                                              19          // OD/IE
#define ESP_SDA                                     21          // OD/IE         SDA
#define ESP_SCL                                     22          // OD/IE         SCL
#define ESP_23                                              23          // OD/IE
#define ESP_PWR_CARD_READER                         32          // OD/ID
#define ESP_TX_CARD_READER                          33          // OD/ID
#define ESP_RX_CARD_READER                          34          // OD/ID         ONLY INPUT PIN
#define ESP_39                                              39          // OD/ID         ONLY INPUT PIN



// GPIO 02          OD/IE/WPD     strapping pin (must be floating or LOW during boot)
// GPIO 05          OD/IE/WPU     VSPI_SS     strapping pin (must be HIGH during boot) 
    // GPIO 12          OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V
// GPIO 13          OD/IE/WPD
    // GPIO 14          OD/IE/WPU
    // GPIO 15          OD/IE/WPU     strapping pin (must be HIGH during boot)
// GPIO 18          OD/IE         VSPI_SCK
// GPIO 19          OD/IE         VSPI_MISO
    // GPIO 21          OD/IE         WIRE_SDA
    // GPIO 22          OD/IE         WIRE_SCL
// GPIO 23          OD/IE         VSPI_MOSI
    // GPIO 32          OD/ID
    // GPIO 33          OD/ID
// GPIO 34          OD/ID         ONLY INPUT PIN
// GPIO 39          OD/ID         ONLY INPUT PIN

// No use
    // GPIO 00          OD/IE/WPU     (must be LOW to enter boot mode)
    // GPIO 01          OD/IE/WPU     Serial TX
    // GPIO 03          OD/IE/WPU     Serial RX
// GSM Module
    // GPIO 04          OD/IE/WPD     strapping pin
    // GPIO 25          OD/ID
    // GPIO 26          OD/ID
    // GPIO 27          OD/ID
    // GPIO 36          OD/ID         ONLY INPUT PIN
