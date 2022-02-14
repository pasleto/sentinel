// ----- Generic Variables
#define MODULE_NAME "Sentinel Access Unit"
#define MODULE_HW_VERSION "1.0.0"
#define MODULE_SW_VERSION "1.0.0"
#define FORMAT_SPIFFS_IF_FAILED true
#define FORMAT_LITTLEFS_IF_FAILED true
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR 1
#define ESP_ETH_PHY_POWER 16
#define ESP_ETH_PHY_MDC 23
#define ESP_ETH_PHY_MDIO 18
#define ESP_ETH_PHY_TYPE ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
// ----- I2C Pins
#define ESP_SCL 32
#define ESP_SDA 33
// ----- SPI Pins
#define ESP_MOSI 15
#define ESP_MISO 2
#define ESP_SCLK 14
#define ESP_CS 12
// ----- MCP Modules Addresses
#define MCP01_ADDR 0x20 // all pulled to GND
#define MCP02_ADDR 0x27 // all pulled to VCC
// ----- Status LEDs Pins
#define STATUS_BOOT_LED 5 // onboard RXD Led
#define STATUS_SUCCESS_LED 17 // onboard TXD Led
// ----- Wiegand Data Pins
#define ESP_WG01_D0 36
#define ESP_WG01_D1 39 // when no reader connected set input_pulldown
#define ESP_WG02_D0 35
#define ESP_WG02_D1 4 // when no reader connected set input_pulldown
// ----- MCP01 Pins
#define MCP01_WG01_POWER 0 // WG01 Power (output) -> A0
#define MCP01_WG01_GREEN_LED 1 // WG01 Green LED (output) -> A1
#define MCP01_WG01_RED_LED 2 // WG01 Red LED (output) -> A2
#define MCP01_WG01_HOLD 3 // WG01 Hold (output) -> A3
#define MCP01_WG01_BUZZER 4 // WG01 Buzzer (output) -> A4
#define MCP01_WG01_TAMPER 5 // WG01 Tamper (input) -> A5 // ? how to connect WG01 Tamper Analog (0V - 5V) - external analog to digital circuit
#define MCP01_LOCK01 6 // LOCK01 (output) -> A6
#define MCP01_BUTTON01 7 // BUTTON01 (input) -> A7 // ?
#define MCP01_WG02_POWER 8 // WG02 Power (output) -> B0
#define MCP01_WG02_GREEN_LED 9 // WG02 Green LED (output) -> B1
#define MCP01_WG02_RED_LED 10 // WG02 Red LED (output) -> B2
#define MCP01_WG02_HOLD 11 // WG02 Hold (output) -> B3
#define MCP01_WG02_BUZZER 12 // WG02 Buzzer (output) -> B4
#define MCP01_WG02_TAMPER 13 // WG02 Tamper (input) -> B5 // ? how to connect WG02 Tamper Analog (0V - 5V) - external analog to digital circuit
#define MCP01_LOCK02 14 // LOCK02 (output) -> B6
#define MCP01_BUTTON02 15 // BUTTON02 (input) -> B7 // ?
// ----- MCP02 Pins
#define MCP02_STATUS_LED01_GREEN 0 // Green Status LED (output) -> A0 // ?
#define MCP02_STATUS_LED01_RED 1 // Red Status LED (output) -> A1 // ?
#define MCP02_STATUS_LED02_GREEN 2 // Green Status LED (output) -> A2 // ?
#define MCP02_STATUS_LED02_RED 3 // Red Status LED (output) -> A3 // ?
#define MCP02_WG01_DATA_REVERSE 4 // DIP01_1 (input) -> A4
#define MCP02_WG02_DATA_REVERSE 5 // DIP01_2 (input) -> A5
#define MCP02_WG03_DATA_REVERSE 6 // DIP01_3 (input) -> A6
#define MCP02_WG04_DATA_REVERSE 7 // DIP02_1 (input) -> A7
#define MCP02_DIP02_2 8 // DIP02_2 (input) -> B0
#define MCP02_DIP02_3 9 // DIP02_3 (input) -> B1
#define MCP02_DIP03_1 10 // DIP03_1 (input) -> B2
#define MCP02_DIP03_2 11 // DIP03_2 (input) -> B3
#define MCP02_DIP03_3 12 // DIP03_3 (input) -> B4
#define MCP02_RF_DATA 13 // RF_DATA (input) -> B5 // ?
#define MCP02_RF_LEARN_BTN 14 // RF_LEARN_BTN (input) -> B6 // ?
#define MCP02_RF_DELETE_BTN 15 // RF_DELETE_BTN (input) -> B7 // ?
