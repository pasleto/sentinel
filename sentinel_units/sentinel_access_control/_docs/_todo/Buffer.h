#pragma once

class Buffer {
  public:
    Buffer();
    void init(int size);
    char* getBuffer();
    int getSize();
  private:
    char* _buffer;
    int _size;
};

extern Buffer* buffer;
