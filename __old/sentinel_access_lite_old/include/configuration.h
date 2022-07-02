// ----- Generic Variables
#define MODULE_NAME                     "Sentinel Access Lite"
#define MODULE_TYPE                     "STL-AC-L"
#define MODULE_HARDWARE_VERSION         "1.0.7"
#define MODULE_FIRMWARE_VERSION         "1.0.7"
#define FORMAT_FS_IF_FAILED             true
// ----- Default Values for Variables
#define DEFAULT_BACKEND_SERVER          "STL-GW"
#define DEFAULT_BACKEND_PORT            443
#define DEFAULT_WSIO_NAMESPACE          "/stl-ac-l"
#define DEFAULT_NTP_SERVER              "STL-GW"
#define DEFAULT_TIMEZONE                "UTC0" // CET-1CEST,M3.5.0,M10.5.0/3
#define DEFAULT_USE_SECURE              false
#define DEFAULT_DATA_REVERSE            false
#define DEFAULT_SILENT_MODE             false
#define DEFAULT_LOCK_TIMEOUT            3000
#define DEFAULT_DOOR_SENSOR             true
#define DEFAULT_BUTTON_LOCK_TIMEOUT     5000
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                1                                   //  OD/IE/WPU
#define ESP_ETH_PHY_POWER               16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                 23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                ETH_CLOCK_GPIO0_IN
// ----- Status LEDs Pins
#define ESP_RXD_LED                     5     // onboard RXD Led                OD/IE/WPU
#define ESP_TXD_LED                     17    // onboard TXD Led                OD/IE
// ----- I2C Pins
#define ESP_SCL                         32    // custom SCL pin                 OD/ID
#define ESP_SDA                         33    // custom SDA pin                 OD/ID
// ----- ESP Pins
#define ESP_WG_D0                       36    // WG Data 0 - GREEN (INPUT)      OD/ID - ONLY INPUT PIN
#define ESP_WG_D1                       39    // WG Data 1 - WHITE (INPUT)      OD/ID - ONLY INPUT PIN
#define ESP_DOOR_SENSOR                 35    // Lock Sensor (INPUT)            OD/ID - ONLY INPUT PIN
#define ESP_LOCK_TRIGGER                15    // Lock Trigger (OUTPUT)          OD/IE/WPU
#define ESP_WG_BUZZER                   14    // Buzzer for WG (OUTPUT)         OD/IE/WPU
#define ESP_WG_GREEN_LED                4     // Green LED for WG (OUTPUT)      OD/IE/WPD
#define ESP_WG_RED_LED                  2     // Red LED for WG (OUTPUT)        OD/IE/WPD
#define ESP_BUTTON                      12    // Button (INPUT)                 OD/IE/WPD - enabled by espfuse set to 3.3V

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
