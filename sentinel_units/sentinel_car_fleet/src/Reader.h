#pragma once

#include <Arduino.h>

class Reader {
  public:
    Reader();
    void init();
  private:
    void _onCardReaderReceive();
};

extern Reader* reader;
