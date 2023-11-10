#pragma once

// ----- ESP Pins
#define ESP_PWR_LED                                 02          // OD/IE/WPD     strapping pin (must be floating or LOW during boot)   // TODO
#define GSM_MODULE_PWR                              04          // OD/IE/WPD     - strapping pin -   PWR_PIN ： This Pin is the PWR-KEY of the SIM7600
#define ESP__X__05                                  05          // OD/IE/WPU     strapping pin (must be HIGH during boot) // ?
#define ESP_LED                                     12          // OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V
#define ESP_BEEPER                                  13          // OD/IE/WPD
#define ESP_CONFIG_BUTTON                           14          // OD/IE/WPU
#define ESP_RST                                     15          // OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_TX_OBD                                  18          // OD/IE
#define ESP_RX_OBD                                  19          // OD/IE
#define ESP_SDA                                     21          // OD/IE         SDA
#define ESP_SCL                                     22          // OD/IE         SCL
#define ESP_CAR_PWR_INT                             23          // OD/IE         // TODO - sense when car is started and wake module
#define GSM_MODULE_POWER                            25          // OD/ID         POWER_PIN : This pin controls the power supply of the SIM7600
#define GSM_MODULE_RX                               26          // OD/ID
#define GSM_MODULE_TX                               27          // OD/ID
#define ESP_PWR_CARD_READER                         32          // OD/ID
#define ESP_TX_CARD_READER                          33          // OD/ID
#define ESP_RX_CARD_READER                          34          // OD/ID         ONLY INPUT PIN
// #define GSM_AXP_IRQ                                 35          //
#define GSM_MODULE_INT                              36          // OD/ID         ONLY INPUT PIN   INT_PIN: It is connected to the SIM7600 status Pin, through which you can know whether the module starts normally
// #define GSM_PUSH_BUTTON                             37          // PWR BTN on side of the device
#define ESP_MPU_INT                                 39          // OD/ID         ONLY INPUT PIN

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


// GPIO 12 hack
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) determined by GPIO12 on reset (High for 1.8V, Low/NC for 3.3V).
// espefuse.py --port COMx set_flash_voltage 3.3V       -> Enable internal flash voltage regulator (VDD_SDIO) to 3.3V.
// espefuse.py --port COMx summary                      -> Flash voltage (VDD_SDIO) set to 3.3V by efuse.
