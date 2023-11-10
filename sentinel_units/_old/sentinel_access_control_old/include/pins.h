// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                            1
#define ESP_ETH_PHY_POWER                           16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                             23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                            18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                            ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN
// ----- ESP Pins
#define ESP_CONFIG_BUTTON                           02    // config button (INPUT) 3v3 trg  OD/IE/WPD     strapping pin (must be floating or LOW during boot)
#define ESP_READER_2_D0                             04    // WG_2 Data 0 - GREEN (INPUT)    OD/IE/WPD     strapping pin
#define ESP_RXD_LED                                 05    // ? // onboard RXD Led                OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_READER_2_D1                             12    // WG_2 Data 1 - WHITE (INPUT)    OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
#define ESP__X__14                                  14    // ? EPS (INPUT)                OD/IE/WPU
#define ESP_MCP_INT                                 15    // MCP Interrupt Pin (INPUT)      OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_TXD_LED                                 17    // ? // onboard TXD Led                OD/IE
#define ESP_SCL                                     32    // custom SCL pin                 OD/ID
#define ESP_SDA                                     33    // custom SDA pin                 OD/ID
#define ESP__X__35                                  35    // ? EPS (INPUT)                OD/ID         ONLY INPUT PIN
#define ESP_READER_1_D0                             36    // WG_1 Data 0 - GREEN (INPUT)    OD/ID         ONLY INPUT PIN
#define ESP_READER_1_D1                             39    // WG_1 Data 1 - WHITE (INPUT)    OD/ID         ONLY INPUT PIN
// ----- MCP Pins
#define MCP_READER_1_GREEN_LED                      0     // A0 - WG_1 Green LED for WG (OUTPUT)
#define MCP_READER_1_RED_LED                        1     // A1 - WG_1 Red LED for WG (OUTPUT)
#define MCP_READER_1_BUZZER                         2     // A2 - WG_1 Buzzer for WG (OUTPUT)
#define MCP_READER_1_TAMPER                         3     // A3 - WG_1 Tamper for WG (INPUT) // !
#define MCP__X__A4                                  4     // A4 - // ? WG_1 Hold (OUTPUT) || WG_1 Power (OUTPUT) || EPS (INPUT)
#define MCP__X__A5                                  5     // A5 - // ? WG_1 Hold (OUTPUT) || WG_1 Power (OUTPUT) || EPS (INPUT)
#define MCP_BUTTON_1                                6     // A6 - Button_1 (INPUT)
#define MCP_LOCK_TRIGGER                            7     // A7 - Lock_1 Trigger (OUTPUT)
#define MCP_READER_2_GREEN_LED                      8     // B0 - WG_2 Green LED for WG (OUTPUT)
#define MCP_READER_2_RED_LED                        9     // B1 - WG_2 Red LED for WG (OUTPUT)
#define MCP_READER_2_BUZZER                         10    // B2 - WG_2 Buzzer for WG (OUTPUT)
#define MCP_READER_2_TAMPER                         11    // B3 - WG_2 Tamper for WG (INPUT) // !
#define MCP__X__B4                                  12    // B4 - // ? WG_2 Hold (OUTPUT) || WG_2 Power (OUTPUT) || EPS (INPUT)
#define MCP__X__B5                                  13    // B5 - // ? WG_2 Hold (OUTPUT) || WG_2 Power (OUTPUT) || EPS (INPUT)
#define MCP_BUTTON_2                                14    // B6 - Button_2 (INPUT)
#define MCP_DOOR_SENSOR                             15    // B7 - Door_1 Sensor (INPUT)