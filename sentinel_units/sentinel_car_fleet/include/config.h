#pragma once

#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER                          1024   // Set RX buffer to 1Kb // TODO

// #define SerialMon                                   Serial
// #define SerialAT                                    Serial1
// #define SerialCardReader                            Serial2
#define MPU6050_I2C_ADDRESS                         0x69
#define CARD_READER_BAUD_RATE                       9600
#define OBD_BAUD_RATE                               9600    // TODO
#define GSM_MODULE_UART_BAUD                        115200

#define GSM_PIN                                     ""          // No PIN

#define FORMAT_FS_IF_FAILED                         true
#define MQTT_CLEAN_SESSION                          false   // TODO - clean session or not ???

#define MQTT_BASE_TOPIC                             "car-fleet/"

#define MODULE_NAME                                 "Sentinel - Car Fleet"
#define MODULE_TYPE                                 "STL-CF"

#define MODULE_FIRMWARE_VERSION                     "1.0.5" // ! - change on firmware updates
// #define MODULE_HARDWARE_VERSION                     "1.0.5"

// GPIO 12 hack
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).
// espefuse.py --port COMx set_flash_voltage 3.3V       -> Enable internal flash voltage regulator (VDD_SDIO) to 3.3V.
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) set to 3.3V by efuse.

// espefuse.exe --port COM7 summary
// espefuse.exe --port COM7 set_flash_voltage 3.3V
// BURN
// espefuse.exe --port COM7 summary
