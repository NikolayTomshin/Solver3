#pragma once
#include <avr/pgmspace.h>
#include <stdint.h>
#include <Arduino.h>
#include "Utilities.h"
//
// StrRep - parent of all strings
// \-StrVar - variable strings
// | \-StrVal - string with char array inside
// | \-StrBuffer - reference to buffer with limit
// | \-StrSpanVar - view of StrVar span
// \-StrSum - view of string concatenation
// \-StrSpan - view of StrRep span
// \-FStr - view of string in flash memory
//
class StrRep;
using StrSpan = SpanRepresentation<StrRep>;
using StrSpanVar = VariableSpanRepresentation<StrRep>;
using StrSum = RepresentationComposition<StrRep>;
class StrVal;
class StrRep : public ValRep<char> {
protected:
  using RIterator = Representation<char>::RIterator;
  using DumbRiterator = Representation<char>::DumbRiterator;
public:
  uint16_t strLen() const;
  virtual SPtr<RIterator> iteratorForwardV() override;

  StrVal toVal();
  StrVal toValFull();

  void print(HardwareSerial& port);
  void printn(HardwareSerial& port);
  void printFull(HardwareSerial& port);
  void printnFull(HardwareSerial& port);
};
class StrEmpty : public StrRep {
public:
  StrEmpty() {}
  virtual char readAt(uint16_t index) override;
  virtual uint16_t getSize() override;
};
class FStr : public StrRep {
protected:
  using typename StrRep::RIterator;
  using typename StrRep::DumbRiterator;
  const char* pgmP = NULL;
public:
  FStr(__FlashStringHelper* pgmP)
    : pgmP((char*)pgmP) {}
  FStr(char* const pgmP)
    : pgmP(pgmP) {}
  FStr(const FStr& other)
    : pgmP(other.pgmP) {}
  FStr& operator=(const FStr& other) {
    pgmP = other.pgmP;
    return *this;
  }
  virtual char readAt(uint16_t index) override {
    return pgm_read_byte(pgmP + index);
  }
  virtual uint16_t getSize() override {
    return strlen_P(pgmP);
  }
};

class StrVar : public VariableRepresentation<StrRep> {  //Variable String  base class
protected:
  using RIterator = StrRep::RIterator;
  using DumbRiterator = StrRep::DumbRiterator;
public:
  char& operator[](uint16_t index) {
    return itemAt(index);
  }
  void paste(const StrRep& rep, uint16_t from, uint16_t until = 0xFFFF) {
    uLimited(from, getSize());
    uLimited(until, getSize());
    for (; from < until; ++from)
      itemAt(from) = rep.readAt(from);
  }
};
class StrVal : public StrVar {  //Value String class
protected:
  Array<char> array;
public:
  StrVal(uint16_t size)
    : array(size) {}
  StrVal(const StrVal& other)
    : array(other.array) {}
  StrVal(StrVal&& other)
    : array(move(other.array)) {}
  StrVal(__FlashStringHelper* fsh) {
    FStr fs(fsh);
    uint16_t len = fs.getSize();
    array = Array<char>(len);
    for (uint16_t i = 0; i < len; ++i)
      array[i] = fs.readAt(i);
  }
  ~StrVal() {}
  StrVal& operator=(const StrVal& other) {
    array = other.array;
    return *this;
  }
  StrVal& operator=(const StrRep& other) {
    *this = other.toVal();
    return *this;
  }
  StrVal& operator=(StrVal&& other) {
    array = move(other.array);
    return *this;
  }
  virtual char readAt(uint16_t index) override {
    return array[index];
  }
  virtual uint16_t getSize() override {
    return array.getSize();
  }
  virtual char& itemAt(uint16_t index) override {
    return array[index];
  }
};
class StrBuffer : public StrVar {  //reference to char[] buffer
protected:
  char* pointer = NULL;
  uint16_t size = 0;
public:
  StrBuffer(char* pointer, uint16_t size)
    : pointer(pointer), size(size) {}
  StrBuffer(const StrBuffer& other) {
    *this = other;
  }
  StrBuffer(StrBuffer&& other) {
    *this = move(other);
  }
  ~StrBuffer() {}
  StrBuffer& operator=(const StrBuffer& other) {
    uint16_t limit = uLimit(size, other.size);
    for (uint16_t i = 0; i < limit; ++i)
      pointer[i] = other[i];
    return *this;
  }
  StrBuffer& operator=(StrBuffer&& other) {
    swap(pointer, other.pointer);
    swap(size, other.size);
    return *this;
  }
  virtual char readAt(uint16_t index) override {
    return pointer[index];
  }
  virtual uint16_t getSize() override {
    return size;
  }
  virtual char& itemAt(uint16_t index) override {
    return pointer[index];
  }
};
class //continue

StrVal boolStr(bool value) {
  return value ? F("true") : F("false");
}
bool fromString(const String& boolString) {
  return !boolStr(true).compareTo(boolString);
}

template<> inline void pout(const StrRep& data) {
  data.print(Serial);
}
template<> inline void poutN(const StrRep& data) {
  data.print(Serial);
  pnl();
}
//strCompose(StrSum&... args) - returns StrSum of arguments
template<typename T, typename = StrSum&>
StrSum strCompose(T first) {
  StrSum sum;
  sum.push(SPtr<StrRep>(&first));
  return sum;
}
template<typename T, typename... T2, typename = StrSum&>
StrSum strCompose(T first, T2... args) {
  return strCompose(first, args...).push(SPtr<StrRep>(&first));
}