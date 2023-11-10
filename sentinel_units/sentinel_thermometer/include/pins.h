#pragma once

// ----- Ethernet Pins
#define ESP_ETH_PHY_ADDR                            1
#define ESP_ETH_PHY_POWER                           16                                  //  OD/IE
#define ESP_ETH_PHY_MDC                             23                                  //  OD/IE
#define ESP_ETH_PHY_MDIO                            18                                  //  OD/IE
#define ESP_ETH_PHY_TYPE                            ETH_PHY_LAN8720
#define ESP_ETH_CLK_MODE                            ETH_CLOCK_GPIO0_IN
// ----- ESP Pins
#define ESP__X__02                                  02    // ?                              OD/IE/WPD     strapping pin (must be floating or LOW during boot)
#define ESP__X__04                                  04    // ?                              OD/IE/WPD     - strapping pin -
#define ESP_LED_MQTT                                05    // - MQTT Connected LED           OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_I2C_2_SCL                               12    // I2C 2 SCL                      OD/IE/WPD     strapping pin (must be LOW during boot) | ! this can be hacked -> enable by espfuse set to 3.3V -> is hacked
#define ESP_I2C_2_SDA                               14    // I2C 2 SDA                      OD/IE/WPU
#define ESP__X__15                                  15    // ?                              OD/IE/WPU     strapping pin (must be HIGH during boot)
#define ESP_LED_NETWORK                             17    // - Network Connected LED        OD/IE
#define ESP_I2C_1_SCL                               32    // I2C 1 SCL                      OD/ID
#define ESP_I2C_1_SDA                               33    // I2C 1 SDA                      OD/ID
#define ESP_CONFIG_BUTTON                           35    // config button (INPUT)          OD/ID         ONLY INPUT PIN
#define ESP__X__36                                  36    // ?                              OD/ID         ONLY INPUT PIN
#define ESP__X__39                                  39    // ?                              OD/ID         ONLY INPUT PIN



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
