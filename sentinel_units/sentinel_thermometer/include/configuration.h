#define SerialMon                                   Serial
#define Sensor1                                     Wire
#define Sensor2                                     Wire1
// ----- Generic Variables
#define MODULE_NAME                                 "Sentinel - Thermometer"
#define MODULE_TYPE                                 "STL-TH"
#define MODULE_FIRMWARE_VERSION                     "1.0.1"
#define FORMAT_FS_IF_FAILED                         true
// ----- Default Values for Variables
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
#define MQTT_AVAILABILITY_BIRTH                     "ONLINE"
#define MQTT_AVAILABILITY_LWT                       "OFFLINE"
#define MQTT_CLEAN_SESSION                          false   // TODO - clean session or not ???
// ----- Ethernet Pins
// #define ESP_ETH_PHY_ADDR                            0 // ? - lilygo t-internet poe
#define ESP_ETH_PHY_ADDR                            1
#define ESP_ETH_PHY_POWER                           16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                             23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                            18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                            ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN
// #define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN // ? - lilygo t-internet poe
// ----- ESP Pins
#define ESP_CONFIG_BUTTON                           02    //                                OD/IE/WPD     strapping pin (must be floating or LOW during boot)
#define ESP__X__04                                  04    //                                OD/IE/WPD     - strapping pin -
#define ESP__X__05                                  05    //                                OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP__X__12                                  12    //                                OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
#define ESP_SCL_2                                   14    //                                OD/IE/WPU
#define ESP__X__15                                  15    //                                OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_SDA_2                                   17    //                                OD/IE
#define ESP_SCL_1                                   32    // I2C_1 SCL pin                  OD/ID
#define ESP_SDA_1                                   33    // I2C_1 SDA pin                  OD/ID
#define ESP_SENSOR_1                                35    //                                OD/ID         ONLY INPUT PIN
#define ESP_SENSOR_2                                36    //                                OD/ID         ONLY INPUT PIN
#define ESP__X__39                                  39    //                                OD/ID         ONLY INPUT PIN

// #define ESP__X__02                                  02    // SD Card Slot - MISO            OD/IE/WPD     strapping pin (must be floating or LOW during boot)
// #define ESP__X__04                                  04    //                                OD/IE/WPD     - strapping pin -
// #define ESP__X__12                                  12    //                                OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
// #define ESP__X__14                                  14    // SD Card Slot - SCLK            OD/IE/WPU
// #define ESP__X__15                                  15    // SD Card Slot - MOSI            OD/IE/WPU     strapping pin (must be HIGH during boot)
// #define ESP__X__16                                  16    //                                OD/ID
// #define ESP__X__32                                  32    //                                OD/ID
// #define ESP__X__33                                  33    //                                OD/ID
// #define ESP__X__34                                  34    //                                OD/ID         ONLY INPUT PIN
// #define ESP__X__35                                  35    //                                OD/ID         ONLY INPUT PIN
// #define ESP__X__36                                  36    //                                OD/ID         ONLY INPUT PIN
// #define ESP__X__39                                  39    //                                OD/ID         ONLY INPUT PIN



// GPIO 00          OD/IE/WPU     (must be LOW to enter boot mode)
// GPIO 01          OD/IE/WPU     Serial_0_TX
// GPIO 02          OD/IE/WPD     strapping pin (must be floating or LOW during boot)
// GPIO 03          OD/IE/WPU     Serial_0_RX
// GPIO 04          OD/IE/WPD     - strapping pin -
// GPIO 05          OD/IE/WPU     strapping pin (must be HIGH during boot)
// GPIO 12          OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
// GPIO 13          OD/IE/WPD
// GPIO 14          OD/IE/WPU
// GPIO 15          OD/IE/WPU     strapping pin (must be HIGH during boot)
// GPIO 16          OD/IE
// GPIO 17          OD/IE
// GPIO 18          OD/IE
// GPIO 19          OD/IE
// GPIO 21          OD/IE
// GPIO 22          OD/IE
// GPIO 23          OD/IE
// GPIO 25          OD/IE
// GPIO 26          OD/IE
// GPIO 27          OD/IE
// GPIO 32          OD/ID
// GPIO 33          OD/ID
// GPIO 34          OD/ID         ONLY INPUT PIN
// GPIO 35          OD/ID         ONLY INPUT PIN
// GPIO 36          OD/ID         ONLY INPUT PIN
// GPIO 39          OD/ID         ONLY INPUT PIN


// GPIO 12 hack
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).
// espefuse.py --port COMx set_flash_voltage 3.3V       -> Enable internal flash voltage regulator (VDD_SDIO) to 3.3V.
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) set to 3.3V by efuse.
