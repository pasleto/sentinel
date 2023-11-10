#pragma once

#include <ELMduino.h>
#include <SoftwareSerial.h> // TODO - causing error in version 7.0.1 above -> dangerous relocation: l32r: literal placed after use [DNSServer || WebServer || SSLClient]

class Obd {
  public:
    Obd();
    void init();
    void handle();
  private:
    ELM327 elm_327; // ?
    SoftwareSerial SerialOBD; // ?
};

extern Obd* obd;
