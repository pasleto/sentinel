#define SerialMon                                   Serial
// #define SerialOSDP                                  Serial1  // use gpio05 and gpio17 -> SerialOSDP.begin(9600, SERIAL_8N1, 5, 17);
#define MCP_I2C_ADDRESS                             0x20  // A0 - GND, A1 - GND, A2 - GND
// ----- Generic Variables
#define MODULE_NAME                                 "Sentinel - Access Control"
#define MODULE_TYPE                                 "STL-AC"
#define MODULE_FIRMWARE_VERSION                     "1.3.7"
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
#define DEFAULT_WG_REVERSE                          false
#define DEFAULT_WG_SILENT                           false
#define DEFAULT_WG_TAMPER_SENSOR                    false   // TODO - if anti-tamper switch is used
#define DEFAULT_WG_TAMPER_INVERT                    false   // TODO - if tamper switch logic is inverted (default trigger is by pulling pin to ground - idle pulled to vcc by pull-up || pulled to ground is idle - trigger by openning the circuit -> pull to vcc by pull-up)
#define DEFAULT_WG_BUZZER_TIMEOUT                   150
#define DEFAULT_WG_TIMEOUT                          3000
#define DEFAULT_BUTTON_TIMEOUT                      5000
#define DEFAULT_DOOR_SENSOR                         false
#define MQTT_AVAILABILITY_BIRTH                     "ONLINE"
#define MQTT_AVAILABILITY_LWT                       "OFFLINE"
#define MQTT_CLEAN_SESSION                          false   // TODO - clean session or not ???
#define DEFAULT_ENCLOSURE_TAMPER_SENSOR             false   // TODO
#define DEFAULT_ENCLOSURE_TAMPER_INVERT             false   // TODO
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                            1
#define ESP_ETH_PHY_POWER                           16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                             23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                            18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                            ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN
// ----- ESP Pins
#define ESP_CONFIG_BUTTON                           02    // config button (INPUT) 3v3 trg  OD/IE/WPD     strapping pin (must be floating or LOW during boot)
#define ESP_WG_2_D0                                 04    // WG_2 Data 0 - GREEN (INPUT)    OD/IE/WPD     strapping pin
#define ESP_RXD_LED                                 05    // ? // onboard RXD Led                OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_WG_2_D1                                 12    // WG_2 Data 1 - WHITE (INPUT)    OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
#define ESP__X__14                                  14    // ? EPS (INPUT)                OD/IE/WPU
#define ESP_MCP_INT                                 15    // MCP Interrupt Pin (INPUT)      OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_TXD_LED                                 17    // ? // onboard TXD Led                OD/IE
#define ESP_SCL                                     32    // custom SCL pin                 OD/ID
#define ESP_SDA                                     33    // custom SDA pin                 OD/ID
#define ESP__X__35                                  35    // ? EPS (INPUT)                OD/ID         ONLY INPUT PIN
#define ESP_WG_1_D0                                 36    // WG_1 Data 0 - GREEN (INPUT)    OD/ID         ONLY INPUT PIN
#define ESP_WG_1_D1                                 39    // WG_1 Data 1 - WHITE (INPUT)    OD/ID         ONLY INPUT PIN
// ----- MCP Pins
#define MCP_WG_1_GREEN_LED                          0     // A0 - WG_1 Green LED for WG (OUTPUT)
#define MCP_WG_1_RED_LED                            1     // A1 - WG_1 Red LED for WG (OUTPUT)
#define MCP_WG_1_BUZZER                             2     // A2 - WG_1 Buzzer for WG (OUTPUT)
#define MCP_WG_1_TAMPER                             3     // A3 - WG_1 Tamper for WG (INPUT) // !
#define MCP__X__A4                                  4     // A4 - // ? WG_1 Hold (OUTPUT) || WG_1 Power (OUTPUT) || EPS (INPUT)
#define MCP__X__A5                                  5     // A5 - // ? WG_1 Hold (OUTPUT) || WG_1 Power (OUTPUT) || EPS (INPUT)
#define MCP_BUTTON_1                                6     // A6 - Button_1 (INPUT)
#define MCP_LOCK_TRIGGER                            7     // A7 - Lock_1 Trigger (OUTPUT)
#define MCP_WG_2_GREEN_LED                          8     // B0 - WG_2 Green LED for WG (OUTPUT)
#define MCP_WG_2_RED_LED                            9     // B1 - WG_2 Red LED for WG (OUTPUT)
#define MCP_WG_2_BUZZER                             10    // B2 - WG_2 Buzzer for WG (OUTPUT)
#define MCP_WG_2_TAMPER                             11    // B3 - WG_2 Tamper for WG (INPUT) // !
#define MCP__X__B4                                  12    // B4 - // ? WG_2 Hold (OUTPUT) || WG_2 Power (OUTPUT) || EPS (INPUT)
#define MCP__X__B5                                  13    // B5 - // ? WG_2 Hold (OUTPUT) || WG_2 Power (OUTPUT) || EPS (INPUT)
#define MCP_BUTTON_2                                14    // B6 - Button_2 (INPUT)
#define MCP_DOOR_SENSOR                             15    // B7 - Door_1 Sensor (INPUT)



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
