#include <avr/pgmspace.h>
#include <stdint.h>
#pragma once
#include <Arduino.h>
#include "Utilities.h"

class StrVal;          //char array
class StrComposition;  //Composition of existing StrRefs


class StrRep : ICollection {
public:
  virtual char operator[](uint16_t index) const = 0;

  class ReadIterator {
  protected:
    const StrRep& strRep;
    uint16_t index = 0;
    uint16_t limit = 0;
    bool reverse = false;
  public:
    ReadIterator(const StrRep& strRep)
      : strRep(strRep), limit(strRep.getSize()) {}
    ReadIterator begin(bool backwards = false, uint16_t index = 0, bool reverse) {
      uLimitedStrict(index, limit);
      this->index = backwards ? mirrorValueOnRange(0, index, limit) : index;
      this->reverse = reverse;
      return *this;
    }
    char operator*() {
      return strRep[index];
    }
    ReadIterator& operator++() {
      return operator+=(1);
    }
    ReadIterator& operator--() {
      return operator+=(-1);
    }
    ReadIterator& operator+=(int16_t delta) {
      if (reverse) delta = -delta;
      index += delta;
      return *this;
    }
    bool notEnd() const {
      return index < limit;
    }
  };
  ReadIterator iteratorRead() const {
    return ReadIterator(*this);
  }
};
class StrVar : StrRep {
public:
  virtual char& operator[](uint16_t index) = 0;

  class WriteIterator {
  protected:
    StrVar& strVar;
    uint16_t index = 0;
    uint16_t limit = 0;
    bool reverse = false;
  public:
    WriteIterator(StrVar& strVar)
      : strVar(strVar), limit(strVar.getSize()) {}
    WriteIterator begin(bool backwards = false, uint16_t index = 0, bool reverse) {
      uLimitedStrict(index, limit);
      this->index = backwards ? mirrorValueOnRange(0, index, limit) : index;
      this->reverse = reverse;
      return *this;
    }
    char& operator*() {
      return strVar[index];
    }
    WriteIterator& operator++() {
      return operator+=(1);
    }
    WriteIterator& operator--() {
      return operator+=(-1);
    }
    WriteIterator& operator+=(int16_t delta) {
      if (reverse) delta = -delta;
      index += delta;
      return *this;
    }
    bool notEnd() const {
      return index < limit;
    }
  } WriteIterator iteratorWrite() {
    return WriteIterator(*this);
  }
};
class FlashStr : public StrRep {
private:
  char* eptr = NULL;
public:
  FlashStr(__FlashStringHelper* eptr)
    : eptr((void*)eptr) {}
  virtual char operator[](uint16_t index) const override {
    return pgm_read_byte(eptr + index);
  }
  virtual uint16_t getSize() const override {
    return strlen_P(eptr);
  }
};

class StrRepSpan : public StrRep {
private:
  const volatile StrRep& strRep;
  uint16_t from;
  uint16_t until;
public:
  void updateSpan() {
    *this = StrRepSpan(strRep, from, until);
  }
  StrRepSpan(const volatile StrRep& strRep, uint16_t from, uint16_t until = 0xFFFF)
    : strRep(strRep) {
    uLimited(until, strRep.getSize());  //max until = size
    this->until = until;
    this->from = uLimitStrict(from, until);  //from is less then until
  }
  virtual char operator[](uint16_t index) const override {
    return strRep[index + from];
  }
  virtual uint16_t getSize() const override {
    return until - from;
  }
}