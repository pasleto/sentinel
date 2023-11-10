#pragma once

#define MODULE_NAME                                 "Sentinel - Access Control"
#define MODULE_TYPE                                 "STL-AC"
#define DEVELOPER                                   "pasleto"
#define MQTT_BASE_TOPIC                             "access-control/"
#define FORMAT_FS_IF_FAILED                         true
#define MQTT_CLEAN_SESSION                          false                               // TODO - clean session or not ???
#define MCP_I2C_ADDRESS                             0x20                                // A0 - GND, A1 - GND, A2 - GND
#define OLED_I2C_ADDRESS                            0x3C

#define STATUS_LED_TYPE                             NEO_KHZ800
#define STATUS_LED_COLOR_ORDER                      NEO_GRB
#define STATUS_LED_COUNT                            1
#define STATUS_LED_BRIGHTNESS                       16  // ? 32

// #define JSON_MAX_SIZE                               12288                               // !!!


// GPIO 12 hack
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).
// espefuse.py --port COMx set_flash_voltage 3.3V       -> Enable internal flash voltage regulator (VDD_SDIO) to 3.3V.
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) set to 3.3V by efuse.

// espefuse.exe --port COM7 summary
// espefuse.exe --port COM7 set_flash_voltage 3.3V
// BURN
// espefuse.exe --port COM7 summary
