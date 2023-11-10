#include "Oled.h"
#include "Utils.h"
#include "Logger.h"
#include "Filesystem.h"
#include "config.h"

Oled* oled = new Oled();

const unsigned char _logo [] PROGMEM = { // 28x28
	0x00, 0x02, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x3f, 0xf0, 0xff, 0xe0,
	0x3f, 0xc0, 0x1f, 0xe0, 0x38, 0x00, 0x00, 0xe0, 0x30, 0x00, 0x00, 0xe0, 0x30, 0x00, 0x00, 0xe0,
	0x30, 0x00, 0x00, 0xe0, 0x30, 0x00, 0x00, 0xe0, 0x38, 0x00, 0x00, 0xe0, 0x3e, 0x07, 0x80, 0x60,
	0x1f, 0x87, 0xe0, 0x00, 0x07, 0xf1, 0xfc, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0x3e, 0x0f, 0xc0,
	0x18, 0x06, 0x03, 0x80, 0x0c, 0x00, 0x03, 0x80, 0x0e, 0x00, 0x03, 0x00, 0x06, 0x00, 0x07, 0x00,
	0x07, 0x00, 0x0e, 0x00, 0x03, 0x80, 0x1c, 0x00, 0x01, 0xc0, 0x1c, 0x00, 0x00, 0xe0, 0x78, 0x00,
	0x00, 0x78, 0xf0, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x06, 0x00, 0x00
};

// const unsigned char _wifiIcon [] PROGMEM = { // 10x8
//   0x3f, 0x00, 0x7f, 0x80, 0xc0, 0xc0, 0x1e, 0x00, 0x3f, 0x00, 0x21, 0x00, 0x0c, 0x00, 0x0c, 0x00
// };

// const unsigned char _wifiIcon [] PROGMEM = { // 14x12 - old
//   0x0f, 0xc0, 0x3f, 0xf0, 0x7f, 0xf8, 0xe0, 0x1c, 0xcf, 0xcc, 0x1f, 0xe0, 0x30, 0x30, 0x07, 0x80,
// 	0x0f, 0xc0, 0x08, 0x40, 0x03, 0x00, 0x03, 0x00
// };

Oled::Oled() {
  _display = new Adafruit_SSD1306(128, 32, &Wire);
  _initialized = false;
  _moduleType = MODULE_TYPE;
  _macAddress = utils->getMac(); // _macAddress = utils->getMacNag();
  _ipAddress = "not-connected";
}

void Oled::init() {
  if (!_display->begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Log->println(F("[OLED] Error"));
  } else {
    _initialized = true;
    _clear();
    _drawBooting();
  }
}

void Oled::setup() {
  if (_initialized) {
    _clear();
    _draw();
  }
}

void Oled::setIpAddress(String value) {
  _ipAddress = value;
  _draw();
}

// void Oled::setNetworkConnected(bool value) { // TODO - implement to draw
//   _networkConnected = value;
//   _draw();
// }

// void Oled::setMqttConnected(bool value) { // TODO - implement to draw
//   _mqttConnected = value;
//   _draw();
// }

void Oled::_clear() {
  if (_initialized) {
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->display();
  }
}

void Oled::_drawBooting() {
  if (_initialized) {
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->drawBitmap(8, 2, _logo, 28, 28, WHITE);
    _display->setCursor(48, 6);
    _display->println("SENTINEL");
    _display->setCursor(48, 20);
    _display->println("Booting ...");
    _display->display();
  }
}

void Oled::drawUpdating() {
  if (_initialized) {
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->drawBitmap(8, 2, _logo, 28, 28, WHITE);
    _display->setCursor(48, 6);
    _display->println("SENTINEL");
    _display->setCursor(48, 20);
    _display->println("Updating ...");
    _display->display();
  }
}

void Oled::_draw() {
  if (_initialized) {
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->setCursor(2, 2);
    _display->print(_moduleType);
    _display->print("  "); // _display->print(" ");
    _display->println(filesystem->getFirmwareVersion());
    // if (filesystem->getConfigMode()) {
    //   _display->drawBitmap(117, 1, _wifiIcon, 10, 8, WHITE);
    //   // _display->drawBitmap(116, 2, _wifiIcon, 10, 8, WHITE);
    //   // _display->drawBitmap(112, 2, _wifiIcon, 14, 12, WHITE);
    // }
    _display->setCursor(2, 12);
    _display->println(_macAddress);
    _display->setCursor(2, 22);
    _display->println(_ipAddress);
    _display->display();
  }
}
