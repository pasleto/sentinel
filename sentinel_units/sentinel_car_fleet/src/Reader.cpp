#include "Reader.h"
#include "Logger.h"
#include "Utils.h"
#include "config.h"
#include "pins.h"

Reader* reader = new Reader();

Reader::Reader() {
  // digitalWrite(ESP_PWR_CARD_READER, LOW); // LOW/HIGH depends on mosfet type - here should set mosfet to OFF
  // pinMode(ESP_PWR_CARD_READER, OUTPUT); // card reader mosfet // TODO

  digitalWrite(ESP_PWR_CARD_READER, HIGH);
  pinMode(ESP_PWR_CARD_READER, OUTPUT);
}

void Reader::init() {
  Serial2.begin(CARD_READER_BAUD_RATE, SERIAL_8N1, ESP_RX_CARD_READER, ESP_TX_CARD_READER);
  // Serial2.onReceive(on_card_reader_receive);
  // SerialMon.println(F("[READER] Card Reader Success"));
  if (!Serial2) {
    Log->println(F("[READER] Error"));
  } else {
    // Serial2.onReceive(_onCardReaderReceive);
    Serial2.onReceive([&]() {
      _onCardReaderReceive();
    });
    // Serial2.onReceive([&]() {
    //   if (Serial2.available()) {
    //     String driver_card = Serial2.readString();
    //     Log->println("[DEBUG] READER - READER Card: " + driver_card);
    //     utils->playOnCardBeep();
    //   }
    // });
    // TODO - power on reader -> ESP_PWR_CARD_READER
    digitalWrite(ESP_PWR_CARD_READER, LOW);
  }
}

void Reader::_onCardReaderReceive() { // TODO
  if (Serial2.available()) {
    String driver_card = Serial2.readString();
    Log->println("[DEBUG] READER - READER Card: " + driver_card);
    utils->playOnCardBeep();
  }
}
