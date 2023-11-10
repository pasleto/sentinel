#pragma once

// ? wiegand place on input/output pins (no strapping) - ? future possibility to use as osdp pins

#if defined(WT32_ETH01)                             // ! WT32-ETH01

    #define EXTERNAL_ALARM_TRIGGER                  0       // TODO
    #define INTERNAL_ALARM_TRIGGER                  0       // TODO
    #define DOOR_ALARM_BYPASS_SWITCH                0       // TODO
// ----- ESP Pins
#define ESP_STATUS_LED_DATA                         02    // ? (OUTPUT) - NeoPixel            OD/IE/WPD     strapping pin (must be floating or LOW during boot)
#define ESP_READER_2_D0                             04    // (INPUT) - WG_2 Data 0 - GREEN  OD/IE/WPD     - strapping pin -
#define ESP__X__05                                  05    // ?                              - OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_READER_2_D1                             12    // (INPUT) - WG_2 Data 1 - WHITE  OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
#define ESP__X__14                                  14    // ?                              OD/IE/WPU
#define ESP_MCP_INT                                 15    // (INPUT) - MCP Interrupt Pin    OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP__X__17                                  17    // ?                              - OD/IE
#define ESP_SCL                                     32    // custom SCL pin                 OD/ID
#define ESP_SDA                                     33    // custom SDA pin                 OD/ID
#define ESP_CONFIG_BUTTON                           35    // (INPUT) - config button        OD/ID         ONLY INPUT PIN
#define ESP_READER_1_D0                             36    // (INPUT) - WG_1 Data 0 - GREEN  OD/ID         ONLY INPUT PIN
#define ESP_READER_1_D1                             39    // (INPUT) - WG_1 Data 1 - WHITE  OD/ID         ONLY INPUT PIN
// ----- MCP Pins
#define MCP_READER_1_GREEN_LED                      0     // A0 - (OUTPUT) - WG_1 Green LED for WG
#define MCP_READER_1_RED_LED                        1     // A1 - (OUTPUT) - WG_1 Red LED for WG
#define MCP_READER_1_BUZZER                         2     // A2 - (OUTPUT) - WG_1 Buzzer for WG
#define MCP_READER_1_TAMPER                         3     // A3 - (INPUT) - WG_1 Tamper for WG
#define MCP_READER_1_SUCCESS_OUT                    4     // ? A4 - (OUTPUT) - 
#define MCP_ENCLOSURE_TAMPER_SENSOR                 5     // A5 - (INPUT) - Enclosure Tamper
#define MCP_BUTTON_1                                6     // A6 - (INPUT) - Button_1
#define MCP_LOCK_TRIGGER                            7     // A7 - (OUTPUT) Lock_1 Trigger
#define MCP_READER_2_GREEN_LED                      8     // B0 - (OUTPUT) - WG_2 Green LED for WG
#define MCP_READER_2_RED_LED                        9     // B1 - (OUTPUT) - WG_2 Red LED for WG
#define MCP_READER_2_BUZZER                         10    // B2 - (OUTPUT) - WG_2 Buzzer for WG
#define MCP_READER_2_TAMPER                         11    // B3 - (INPUT) - WG_2 Tamper for WG
#define MCP_READER_2_SUCCESS_OUT                    12    // ? B4 - (OUTPUT) - 
#define MCP_FIRE_SENSOR                             13    // B5 - (INPUT) - Fire Alarm Sensor
#define MCP_BUTTON_2                                14    // B6 - (INPUT) - Button_2
#define MCP_DOOR_SENSOR                             15    // B7 - (INPUT) - Door_1 Sensor

#elif defined(WESP32)                               // ! WESP32

// ----- ESP Pins
#define ESP_                                        02    //
// ----- MCP Pins
#define MCP__X__00                                  0     // A0 - 
#define MCP__X__01                                  1     // A1 - 
#define MCP__X__02                                  2     // A2 - 
#define MCP__X__03                                  3     // A3 - 
#define MCP__X__04                                  4     // A4 - 
#define MCP__X__05                                  5     // A5 - 
#define MCP__X__06                                  6     // A6 - 
#define MCP__X__07                                  7     // A7 - 
#define MCP__X__08                                  8     // B0 - 
#define MCP__X__09                                  9     // B1 - 
#define MCP__X__10                                  1     // B2 - 
#define MCP__X__11                                  1     // B3 - 
#define MCP__X__12                                  1     // B4 - 
#define MCP__X__13                                  1     // B5 - 
#define MCP__X__14                                  1     // B6 - 
#define MCP__X__15                                  1     // B7 - 

#elif defined(T_ETH_LITE_ESP32)                     // ! T_ETH_LITE_ESP32

// ----- ESP Pins
#define ESP_                                        02    //
// ----- MCP Pins
#define MCP__X__00                                  0     // A0 - 
#define MCP__X__01                                  1     // A1 - 
#define MCP__X__02                                  2     // A2 - 
#define MCP__X__03                                  3     // A3 - 
#define MCP__X__04                                  4     // A4 - 
#define MCP__X__05                                  5     // A5 - 
#define MCP__X__06                                  6     // A6 - 
#define MCP__X__07                                  7     // A7 - 
#define MCP__X__08                                  8     // B0 - 
#define MCP__X__09                                  9     // B1 - 
#define MCP__X__10                                  10    // B2 - 
#define MCP__X__11                                  11    // B3 - 
#define MCP__X__12                                  12    // B4 - 
#define MCP__X__13                                  13    // B5 - 
#define MCP__X__14                                  14    // B6 - 
#define MCP__X__15                                  15    // B7 - 

#elif defined(T_ETH_LITE_ESP32S3)                   // ! T_ETH_LITE_ESP32S3

// ----- ESP Pins
#define ESP_                                        02    //
// ----- MCP Pins
#define MCP__X__00                                  0     // A0 - 
#define MCP__X__01                                  1     // A1 - 
#define MCP__X__02                                  2     // A2 - 
#define MCP__X__03                                  3     // A3 - 
#define MCP__X__04                                  4     // A4 - 
#define MCP__X__05                                  5     // A5 - 
#define MCP__X__06                                  6     // A6 - 
#define MCP__X__07                                  7     // A7 - 
#define MCP__X__08                                  8     // B0 - 
#define MCP__X__09                                  9     // B1 - 
#define MCP__X__10                                  10    // B2 - 
#define MCP__X__11                                  11    // B3 - 
#define MCP__X__12                                  12    // B4 - 
#define MCP__X__13                                  13    // B5 - 
#define MCP__X__14                                  14    // B6 - 
#define MCP__X__15                                  15    // B7 - 

#else
#error "No valid board type specified!"
#endif





// ! WESP32
// ----- ESP Pins
// ----- MCP Pins



// HWSerial -> RX pin můžeme nastavit na libovolný pin a TX pin na GPIO0 až GPIO31

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
