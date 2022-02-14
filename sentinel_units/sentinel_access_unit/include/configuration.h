// ----- Generic Variables
#define MODULE_NAME                "Sentinel Access Unit"
#define MODULE_HW_VERSION          "1.0.0"
#define MODULE_SW_VERSION          "1.0.0"
#define FORMAT_SPIFFS_IF_FAILED    true
// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR           1
#define ESP_ETH_PHY_POWER          16
#define ESP_ETH_PHY_MDC            23
#define ESP_ETH_PHY_MDIO           18
#define ESP_ETH_PHY_TYPE           ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE           ETH_CLOCK_GPIO0_IN
// ----- I2C Pins
#define ESP_SCL                    32    // custom SCL pin
#define ESP_SDA                    33    // custom SDA pin
// ----- MCP Modules Addresses
#define MCP01_ADDR                 0x20  // A0 - GND, A1 - GND, A2 - GND
#define MCP02_ADDR                 0x21  // A0 - VCC, A1 - GND, A2 - GND
#define MCP03_ADDR                 0x22  // A0 - GND, A1 - VCC, A2 - GND
// ----- Status LEDs Pins
#define STATUS_BOOT_LED            5     // onboard RXD Led
#define STATUS_SUCCESS_LED         17    // onboard TXD Led
// ----- Wiegand Data Pins
#define ESP_WG01_D0                36    // WG01 Data 0
#define ESP_WG01_D1                39    // WG01 Data 1
#define ESP_WG02_D0                35    // WG02 Data 0
#define ESP_WG02_D1                4     // WG02 Data 1
#define ESP_WG03_D0                15    // WG03 Data 0
#define ESP_WG03_D1                2     // WG03 Data 1
#define ESP_WG04_D0                14    // WG04 Data 0
#define ESP_WG04_D1                12    // WG04 Data 1
// ----- MCP01 Pins
#define MCP01_WG01_POWER           0     // A0 - Power for WG01 (OUTPUT)
#define MCP01_WG01_GREEN_LED       1     // A1 - Green LED for WG01 (OUTPUT)
#define MCP01_WG01_RED_LED         2     // A2 - Red LED for WG01 (OUTPUT)
#define MCP01_WG01_HOLD            3     // A3 - Hold for WG01 (OUTPUT)
#define MCP01_WG01_BUZZER          4     // A4 - Buzzer for WG01 (OUTPUT)
#define MCP01_WG01_TAMPER          5     // A5 - Tamper for WG01 (OUTPUT) // ? connect via external analog to digital converter (0-5V to digital)
#define MCP01_WG02_POWER           6     // A6 - Power for WG02 (OUTPUT)
#define MCP01_WG02_GREEN_LED       7     // A7 - Green LED for WG02 (OUTPUT)
#define MCP01_WG02_RED_LED         8     // B0 - Red LED for WG02 (OUTPUT)
#define MCP01_WG02_HOLD            9     // B1 - Hold for WG02 (OUTPUT)
#define MCP01_WG02_BUZZER          10    // B2 - Buzzer for WG02 (OUTPUT)
#define MCP01_WG02_TAMPER          11    // B3 - Tamper for WG02 (OUTPUT) // ? connect via external analog to digital converter (0-5V to digital)
#define MCP01_WG03_POWER           12    // B4 - Power for WG03 (OUTPUT)
#define MCP01_WG03_GREEN_LED       13    // B5 - Green LED for WG03 (OUTPUT)
#define MCP01_WG03_RED_LED         14    // B6 - Red LED for WG03 (OUTPUT)
#define MCP01_WG03_HOLD            15    // B7 - Hold for WG03 (OUTPUT)
// ----- MCP02 Pins
#define MCP02_WG03_BUZZER          1     // A0 - Buzzer for WG03 (OUTPUT)
#define MCP02_WG03_TAMPER          2     // A1 - Tamper for WG03 (OUTPUT) // ? connect via external analog to digital converter (0-5V to digital)
#define MCP02_WG04_POWER           3     // A2 - Power for WG04 (OUTPUT)
#define MCP02_WG04_GREEN_LED       4     // A3 - Green LED for WG04 (OUTPUT)
#define MCP02_WG04_RED_LED         5     // A4 - Red LED for WG04 (OUTPUT)
#define MCP02_WG04_HOLD            6     // A5 - Hold for WG04 (OUTPUT)
#define MCP02_WG04_BUZZER          7     // A6 - Buzzer for WG04 (OUTPUT)
#define MCP02_WG04_TAMPER          8     // A7 - Tamper for WG04 (OUTPUT) // ? connect via external analog to digital converter (0-5V to digital)
#define MCP02_LOCK_01              9     // B0 - Lock 01 (OUTPUT)
#define MCP02_LOCK_02              10    // B1 - Lock 02 (OUTPUT)
#define MCP02_WG_GND_CONNECT       11    // B2 - Ground Connection For Wiegand Data (OUTPUT)
#define MCP02_b                    12    // B3 - // ?
#define MCP02_BUTTON_01            13    // B4 - Button 01 (INPUT)
#define MCP02_BUTTON_02            14    // B5 - Button 01 (INPUT)
#define MCP02_BUTTON_03            15    // B6 - Button 01 (INPUT)
#define MCP02_BUTTON_04            16    // B7 - Button 01 (INPUT)
// ----- MCP03 Pins
#define MCP03_WG01_DATA_REVERSE    1     // A0 - dip01_01 (INPUT) - if WG01 should reverse byte order - LOW original | HIGH reverse
#define MCP03_WG02_DATA_REVERSE    2     // A1 - dip01_02 (INPUT) - if WG02 should reverse byte order - LOW original | HIGH reverse
#define MCP03_WG03_DATA_REVERSE    3     // A2 - dip01_03 (INPUT) - if WG03 should reverse byte order - LOW original | HIGH reverse
#define MCP03_WG04_DATA_REVERSE    4     // A3 - dip02_01 (INPUT) - if WG04 should reverse byte order - LOW original | HIGH reverse
#define MCP03_WG01_CONNECTED       5     // A4 - dip02_02 (INPUT) - if WG01 is connected - LOW not connected | HIGH connected
#define MCP03_WG02_CONNECTED       6     // A5 - dip02_03 (INPUT) - if WG02 is connected - LOW not connected | HIGH connected
#define MCP03_WG03_CONNECTED       7     // A6 - dip03_01 (INPUT) - if WG03 is connected - LOW not connected | HIGH connected
#define MCP03_WG04_CONNECTED       8     // A7 - dip03_02 (INPUT) - if WG04 is connected - LOW not connected | HIGH connected
#define MCP03_a                    9     // B0 - dip03_03 (INPUT) // ?
#define MCP03_b                    10    // B1 - // ?
#define MCP03_c                    11    // B2 - // ?
#define MCP03_d                    12    // B3 - // ?
#define MCP03_RF_DATA              13    // B4 - RF Data Reveiver (INPUT)
#define MCP03_RF_LEARN_01          14    // B5 - Button for learning remote controller for LOCK01 (INPUT)
#define MCP03_RF_LEARN_02          15    // B6 - Button for learning remote controller for LOCK02 (INPUT)
#define MCP03_RF_DELETE            16    // B7 - Button for deleting remote controller (INPUT)