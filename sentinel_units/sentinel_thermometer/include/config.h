#pragma once

#define FORMAT_FS_IF_FAILED                         true
#define MQTT_CLEAN_SESSION                          false   // TODO - clean session or not ???
#define MQTT_BASE_TOPIC                             "thermometer/"
#define MODULE_NAME                                 "Sentinel - Thermometer"
#define MODULE_TYPE                                 "STL-TH"
#define MODULE_FIRMWARE_VERSION                     "1.0.1" // ! - change on firmware updates
// #define MODULE_HARDWARE_VERSION                     "1.0.1"

// GPIO 12 hack
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).
// espefuse.py --port COMx set_flash_voltage 3.3V       -> Enable internal flash voltage regulator (VDD_SDIO) to 3.3V.
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) set to 3.3V by efuse.

// espefuse.exe --port COM7 summary
// espefuse.exe --port COM7 set_flash_voltage 3.3V
// BURN
// espefuse.exe --port COM7 summary
