#include <avr/pgmspace.h>
#include <stdint.h>
#pragma once
#include <Arduino.h>
#include "Utilities.h"

class StrVal;          //char array
class StrComposition;  //Composition of existing StrRefs


class StrRep : public ValRep<char> {};
class StrVar : StrRep {
public:
  virtual char& operator[](uint16_t index) = 0;

  class WriteIterator {
  protected:
    StrVar* strVar;
    uint16_t index = 0;
    uint16_t limit = 0;
    bool reverse = false;
  public:
    WriteIterator(StrVar& strVar)
      : strVar(&strVar), limit(strVar.getSize()) {}
    WriteIterator begin(bool backwards = false, uint16_t index = 0, bool reverse = false) {
      uLimitedStrict(index, limit);
      this->index = backwards ? mirrorValueOnRange<uint16_t>(0, index, limit) : index;
      this->reverse = reverse;
      return *this;
    }
    char& operator*() {
      return (*strVar)[index];
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
  };
  WriteIterator iteratorWrite() {
    return WriteIterator(*this);
  }
};
class FlashStr : public StrRep {
private:
  char* const pgmP = NULL;
public:
  FlashStr(__FlashStringHelper* pgmP)
    : pgmP((char*)pgmP) {}
  FlashStr(char* const pgmP)
    : pgmP(pgmP) {}
  virtual char operator[](uint16_t index) const override {
    return pgm_read_byte(pgmP + index);
  }
  virtual uint16_t getSize() const override {
    return strlen_P(pgmP);
  }
};
class StrRepSpan : public StrRep {
private:
  StrRep* strRep;
  uint16_t from;
  uint16_t until;
public:
  uint16_t getFrom() const {
    return from;
  }
  uint16_t getUntil() const {
    return until;
  }
  void setFrom(uint16_t from) {
    reset(from, until);
  }
  void setUntil(uint16_t until) {
    reset(from, until);
  }
  void updateSpan() {
    *this = StrRepSpan(*strRep, from, until);
  }
  void reset(uint16_t from, uint16_t until) {
    *this = StrRepSpan(*strRep, from, until);
  }
  StrRepSpan(const volatile StrRep& strRep, uint16_t from, uint16_t until = 0xFFFF)
    : strRep(&strRep) {
    uLimited(until, strRep.getSize());  //max until = size
    this->until = until;
    this->from = uLimitStrict(from, until);  //from is less then until
  }
  virtual char operator[](uint16_t index) const override {
    return (*strRep)[index + from];
  }
  virtual uint16_t getSize() const override {
    return until - from;
  }
};

class StrSum : public IndexedCompositionTemplate<char, StrRep> {
public:
  virtual char operator[](uint16_t index) const override {
    uLimitedStrict(index, size);
    for (auto it = stack.iteratorForward(); it.notEnd(); ++it) {
      auto& el = *it;
      if (el.isIndexInside(index))
        return (*el.strRep)[index - el.firstGlobalIndex];  //char by local index
    }
    return '\0';
  }
};