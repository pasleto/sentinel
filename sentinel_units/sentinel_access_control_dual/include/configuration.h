#define WEBSOCKETS_NETWORK_TYPE               NETWORK_ESP32_ETH
// ----- Generic Variables
#define MODULE_NAME                           "Sentinel Access Control Dual"
#define MODULE_TYPE                           "STL-AC-D"
#define MODULE_HARDWARE_VERSION               "1.1.0"
#define MODULE_FIRMWARE_VERSION               "1.1.0"
#define FORMAT_FS_IF_FAILED                   true
// ----- Default Values for Variables
#define DEFAULT_IS_SETUP_DONE                 false
#define DEFAULT_BACKEND_SERVER                "STL-GW"
#define DEFAULT_BACKEND_PORT                  443
#define DEFAULT_WSIO_NAMESPACE                "/stl-ac-d"
#define DEFAULT_NTP_SERVER                    "STL-GW"
#define DEFAULT_TIMEZONE                      "UTC0" // CET-1CEST,M3.5.0,M10.5.0/3
#define DEFAULT_USE_SECURE_1                  false
#define DEFAULT_USE_SECURE_2                  false
#define DEFAULT_DATA_REVERSE_1                false
#define DEFAULT_DATA_REVERSE_2                false
#define DEFAULT_SILENT_MODE_1                 false
#define DEFAULT_SILENT_MODE_2                 false
#define DEFAULT_LOCK_1_READER_TIMEOUT         3000
#define DEFAULT_LOCK_2_READER_TIMEOUT         3000
#define DEFAULT_DOOR_SENSOR_1                 true
#define DEFAULT_DOOR_SENSOR_2                 true
#define DEFAULT_LOCK_1_BUTTON_TIMEOUT         5000
#define DEFAULT_LOCK_2_BUTTON_TIMEOUT         5000
#define DEFAULT_WSIO_OUT_LOG_READER           "access-control/dual/log/reader"
#define DEFAULT_WSIO_OUT_LOG_BUTTON           "access-control/dual/log/button"
#define DEFAULT_WSIO_OUT_LOG_DOOR             "access-control/dual/log/door"
#define DEFAULT_WSIO_OUT_LOG_RTC              "access-control/dual/log/rtc"
// #define DEFAULT_WSIO_EVENT_XX              ""
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                      1                                   //  OD/IE/WPU
#define ESP_ETH_PHY_POWER                     16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                       23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                      18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                      ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                      ETH_CLOCK_GPIO0_IN
// ----- Status LEDs Pins
#define ESP_RXD_LED                           5     // onboard RXD Led                OD/IE/WPU
#define ESP_TXD_LED                           17    // onboard TXD Led                OD/IE
// ----- I2C Pins
#define ESP_SCL                               32    // custom SCL pin                 OD/ID
#define ESP_SDA                               33    // custom SDA pin                 OD/ID
// ----- MCP Module Address
#define MCP_1_ADDR                            0x27  // A0 - VCC, A1 - VCC, A2 - VCC
#define MCP_2_ADDR                            0x20  // A0 - GND, A1 - GND, A2 - GND
// ----- ESP Pins
#define ESP_WG_1_1_D0                         36    // WG 1_1 Data 0 - GREEN (INPUT)  OD/ID - ONLY INPUT PIN
#define ESP_WG_1_1_D1                         39    // WG 1_1 Data 1 - WHITE (INPUT)  OD/ID - ONLY INPUT PIN
#define ESP_WG_2_1_D0                         35    // WG 2_1 Data 0 - GREEN (INPUT)  OD/ID - ONLY INPUT PIN
#define ESP_WG_2_2_D0                         15    // WG 2_2 Data 0 - GREEN (INPUT)  OD/IE/WPU
#define ESP_WG_2_2_D1                         14    // WG 2_2 Data 1 - WHITE (INPUT)  OD/IE/WPU
#define ESP_WG_1_2_D0                         4     // WG 1_2 Data 0 - GREEN (INPUT)  OD/IE/WPD
#define ESP_WG_1_2_D1                         2     // WG 1_2 Data 1 - WHITE (INPUT)  OD/IE/WPD
#define ESP_WG_2_1_D1                         12    // WG 2_1 Data 1 - WHITE (INPUT)  OD/IE/WPD - enabled by espfuse set to 3.3V
// ----- MCP_1 Pins
#define MCP_1_LOCK_1_TRIGGER                  0     // A0 - Lock 1 Trigger (OUTPUT)
#define MCP_1_LOCK_2_TRIGGER                  1     // A1 - Lock 2 Trigger (OUTPUT)
#define MCP_1_DOOR_SENSOR_1                   2     // A2 - Lock 1 Sensor (INPUT)
#define MCP_1_DOOR_SENSOR_2                   3     // A3 - Lock 2 Sensor (INPUT)
#define MCP_1_BUTTON_1_1                      4     // A4 - Button 1_1 (INPUT)
#define MCP_1_BUTTON_1_2                      5     // A5 - Button 1_2 (INPUT)
#define MCP_1_BUTTON_2_1                      6     // A6 - Button 2_1 (INPUT)
#define MCP_1_BUTTON_2_2                      7     // A7 - Button 2_2 (INPUT)
// #define MCP_1_8                               8     // B0 - 
// #define MCP_1_9                               9     // B1 - 
// #define MCP_1_10                              10    // B2 - 
// #define MCP_1_11                              11    // B3 - 
// #define MCP_1_12                              12    // B4 - 
// #define MCP_1_13                              13    // B5 - 
// #define MCP_1_14                              14    // B6 - 
// #define MCP_1_15                              15    // B7 - 
// ----- MCP_2 Pins
#define MCP_2_WG_1_1_GREEN_LED                0     // A0 - WG 1_1 Green LED for WG (OUTPUT)
#define MCP_2_WG_1_1_RED_LED                  1     // A1 - WG 1_1 Red LED for WG (OUTPUT)
#define MCP_2_WG_1_1_BUZZER                   2     // A2 - WG 1_1 Buzzer for WG (OUTPUT)
#define MCP_2_WG_1_2_GREEN_LED                3     // A3 - WG 1_2 Green LED for WG (OUTPUT)
#define MCP_2_WG_1_2_RED_LED                  4     // A4 - WG 1_2 Red LED for WG (OUTPUT)
#define MCP_2_WG_1_2_BUZZER                   5     // A5 - WG 1_2 Buzzer for WG (OUTPUT)
#define MCP_2_WG_2_1_GREEN_LED                6     // A6 - WG 2_1 Green LED for WG (OUTPUT)
#define MCP_2_WG_2_1_RED_LED                  7     // A7 - WG 2_1 Red LED for WG (OUTPUT)
#define MCP_2_WG_2_1_BUZZER                   8     // B0 - WG 2_1 Buzzer for WG (OUTPUT)
#define MCP_2_WG_2_2_GREEN_LED                9     // B1 - WG 2_2 Green LED for WG (OUTPUT)
#define MCP_2_WG_2_2_RED_LED                  10    // B2 - WG 2_2 Red LED for WG (OUTPUT)
#define MCP_2_WG_2_2_BUZZER                   11    // B3 - WG 2_2 Buzzer for WG (OUTPUT)
// #define MCP_2_12                              12    // B4 - 
// #define MCP_2_13                              13    // B5 - 
// #define MCP_2_14                              14    // B6 - 
// #define MCP_2_15                              15    // B7 - 

// GPIO 34     OD/ID - ONLY INPUT PIN
// GPIO 27     OD/ID
// GPIO 26     OD/ID
// GPIO 25     OD/ID
// GPIO 22     OD/IE
// GPIO 21     OD/IE
// GPIO 19     OD/IE
// GPIO 13     OD/IE/WPD
// GPIO 03     OD/IE/WPU




// 1x ESP WT32
// 1x RTC DS3231
// 3x Optocoupler 
// 1x SSR
// 1x Battery Board (12V to 12V, 3.7 battery)
// 2x Voltage Regulator (12V to 5V, 12V to 3V)
// 4x 1kOhm Resistor
// 1x DC Barrel Jack
  // 1x Battery Connector
// 1x 4pin Header
// 2x Dual Slide Switch
// 13x Screw Terminal



// ----- Slide Switches
// Dual Switch - Programmin mode - connect 5V to ESP & connect IO0 to GND
// Dual Switch - Power CutOff - connect 12V IN to battery board & connect battery to battery board

// ----- Pin Terminals
// 5V
// GND
// TX
// RX

// ----- Special Connectors
// 12V + IN
// 12V - IN
// BATTERY +
// BATTERY -

// ----- Screw Terminals
// WG 12V
// WG GND
// WG RED LED
// WG GREEN LED
// WG BUZZER
// WG D0
// WG D1
// LOCK 12V TRIGGER (from relay)
// LOCK SENSOR TRIGGER
// LOCK SENSOR COMMON
// BUTTON 12V
// BUTTON GND / BUTTON COMMON
// BUTTON TRIGGER
