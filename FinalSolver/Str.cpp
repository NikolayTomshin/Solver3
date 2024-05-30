#include "Str.h"

uint16_t StrRep::strLen() const {
  uint16_t len = 0;
  {
    auto itp = iteratorForwardV();
    for (auto& it = *itp; it.notEnd(); ++it) {
      if (*it == '\n') break;
      ++len;
    }
  }
  return len;
}
SPtr<StrRep::RIterator> StrRep::iteratorForwardV() {
  return SPtr<RIterator>(new DumbRiterator(*this));
}
StrVal StrRep::toVal() {
  uint16_t len = strLen();
  StrVal str(len);
  {
    auto itp = iteratorForwardV();
    auto& it = *itp;
    for (uint16_t i = 0; i < len; ++i) {
      str[i] = *it;
      ++it;
    }
  }
  return str;
}
StrVal StrRep::toValFull() {
  uint16_t size = getSize();
  StrVal str(size);
  {
    auto itp = iteratorForwardV();
    auto& it = *itp;
    for (uint16_t i = 0; i < size; ++i) {
      str[i] = *it;
      ++it;
    }
  }
  return str;
}
void StrRep::print(HardwareSerial& port) {
  auto itp = iteratorForwardV();
  for (auto& it = *itp; it.notEnd(); ++it)
    port.write(*it);
}
void StrRep::printn(HardwareSerial& port) {
  print(port);
  port.write('\n');
}
void StrRep::printFull(HardwareSerial& port) {
  auto itp = iteratorForwardV();
  for (auto& it = *itp; it.notEnd(); ++it) {
    port.write(*it);
  }
}
void StrRep::printnFull(HardwareSerial& port) {
  printFull(port);
  port.write('\n');
}
virtual char StrEmpty::readAt(uint16_t index) override {
  return '\0';
}
virtual uint16_t StrEmpty::getSize() override {
  return 0;
}