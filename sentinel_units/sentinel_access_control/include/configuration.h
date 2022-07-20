#define WEBSOCKETS_NETWORK_TYPE                     NETWORK_ESP32_ETH
// ----- Generic Variables
#define MODULE_NAME                                 "Sentinel Access Control"
#define MODULE_TYPE                                 "STL-AC"
#define MODULE_HARDWARE_VERSION                     "1.1.1"
#define MODULE_FIRMWARE_VERSION                     "1.1.1"
#define FORMAT_FS_IF_FAILED                         true
#define WSIO_RECONNECT_INTERVAL                     60000
#define WSIO_OUT_LOG_PATH                           "access-control/unit/log"
// #define WSIO_NAMESPACE                              "/stl-ac" // ?
// ----- Default Values for Variables
#define DEFAULT_IS_SETUP_DONE                       false
#define DEFAULT_BACKEND_SERVER                      "STL-GW"
#define DEFAULT_BACKEND_PORT                        443
#define DEFAULT_NTP_SERVER                          "STL-GW"
#define DEFAULT_TIMEZONE                            "UTC0" // CET-1CEST,M3.5.0,M10.5.0/3
#define DEFAULT_USE_SECURE_1                        false
#define DEFAULT_DATA_REVERSE_1                      false
#define DEFAULT_SILENT_MODE_1                       false
#define DEFAULT_DOOR_SENSOR_1                       true
#define DEFAULT_LOCK_1_READER_TIMEOUT               3000
#define DEFAULT_LOCK_1_BUTTON_TIMEOUT               5000
// #define DEFAULT_LOCK_1_REMOTE_TIMEOUT               5000 // TODO
// #define DEFAULT_LOCK_1_DIRECTION_INVERT             false // ? - better to invert this on backend side
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                            1                                   //  OD/IE/WPU
#define ESP_ETH_PHY_POWER                           16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                             23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                            18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                            ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN
// ----- Status LEDs Pins
#define ESP_RXD_LED                                 5     // onboard RXD Led                OD/IE/WPU
#define ESP_TXD_LED                                 17    // onboard TXD Led                OD/IE
// ----- I2C Pins
#define ESP_SCL                                     32    // custom SCL pin                 OD/ID
#define ESP_SDA                                     33    // custom SDA pin                 OD/ID
// ----- MCP Module Address
#define MCP_ADDR                                    0x20  // A0 - GND, A1 - GND, A2 - GND
// ----- ESP Pins
#define ESP_WG_1_1_D0                               36    // WG 1_1 Data 0 - GREEN (INPUT)  OD/ID - ONLY INPUT PIN
#define ESP_WG_1_1_D1                               39    // WG 1_1 Data 1 - WHITE (INPUT)  OD/ID - ONLY INPUT PIN
#define ESP_DOOR_SENSOR_1                           35    // Lock Sensor (INPUT)            OD/ID - ONLY INPUT PIN
#define ESP_LOCK_1_TRIGGER                          15    // Lock Trigger (OUTPUT)          OD/IE/WPU
#define ESP_BUTTON_1_2                              14    // Button 1_2 (INPUT)             OD/IE/WPU
#define ESP_WG_1_2_D0                               4     // WG 1_2 Data 0 - GREEN (INPUT)  OD/IE/WPD
#define ESP_WG_1_2_D1                               2     // WG 1_2 Data 1 - WHITE (INPUT)  OD/IE/WPD
#define ESP_BUTTON_1_1                              12    // Button 1_1 (INPUT)             OD/IE/WPD - enabled by espfuse set to 3.3V
// ----- MCP Pins
#define MCP_WG_1_1_GREEN_LED                        0     // A0 - WG 1_1 Green LED for WG (OUTPUT)
#define MCP_WG_1_1_RED_LED                          1     // A1 - WG 1_1 Red LED for WG (OUTPUT)
#define MCP_WG_1_1_BUZZER                           2     // A2 - WG 1_1 Buzzer for WG (OUTPUT)
#define MCP_WG_1_2_GREEN_LED                        3     // A3 - WG 1_2 Green LED for WG (OUTPUT)
#define MCP_WG_1_2_RED_LED                          4     // A4 - WG 1_2 Red LED for WG (OUTPUT)
#define MCP_WG_1_2_BUZZER                           5     // A5 - WG 1_2 Buzzer for WG (OUTPUT)
// #define MCP_6                                      6     // A6 - 
// #define MCP_7                                      7     // A7 - 
// #define MCP_BUTTON_1_1                              8     // B0 - Button 1_1 (INPUT)
// #define MCP_BUTTON_1_2                              9     // B1 - Button 1_2 (INPUT)
// #define MCP_10                                     10    // B2 -
// #define MCP_11                                     11    // B3 - 
// #define MCP_12                                     12    // B4 - 
// #define MCP_13                                     13    // B5 - 
// #define MCP_14                                     14    // B6 - 
// #define MCP_15                                     15    // B7 - 

// GPIO 34     OD/ID - ONLY INPUT PIN
// GPIO 27     OD/ID
// GPIO 26     OD/ID
// GPIO 25     OD/ID
// GPIO 22     OD/IE
// GPIO 21     OD/IE
// GPIO 19     OD/IE
// GPIO 13     OD/IE/WPD
// GPIO 03     OD/IE/WPU
