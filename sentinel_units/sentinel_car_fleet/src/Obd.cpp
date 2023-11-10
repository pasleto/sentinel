#include "Obd.h"
#include "Logger.h"
#include "Utils.h"
#include "config.h"
#include "pins.h"

Obd* obd = new Obd();

Obd::Obd() {}

void Obd::init() {
  SerialOBD.begin(OBD_BAUD_RATE, SWSERIAL_8N1, ESP_RX_OBD, ESP_TX_OBD); // SerialOBD.begin(OBD_BAUD_RATE, SWSERIAL_8N1, ESP_RX_OBD, ESP_TX_OBD, false);
  if (!SerialOBD) {
    Log->println(F("[OBD] Error"));
  }
}

void Obd::handle() {
  if (SerialOBD.available()) { // TODO
    String driver_card = SerialOBD.readString();
    Log->println("[DEBUG] OBD - READER Card: " + driver_card);
    utils->playOnCardBeep();
  }
}
