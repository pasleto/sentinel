#include "Buffer.h"

Buffer* buffer = new Buffer();

Buffer::Buffer() {}

void Buffer::init(int size) {
  _buffer = new char[size];
  _size = size;
}

char* Buffer::getBuffer() {
  return _buffer;
}

int Buffer::getSize() {
  return _size;
}
