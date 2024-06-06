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
  using ValRep<char>::Reference;
  class LenSizeProxy;
  struct SizeInfo {
    uint16_t size = 0;
    uint16_t len;
    uint16_t free() const {
      return size - len;
    }
  };
  SizeInfo sizeInfo();
public:
  uint16_t strLen() const;  //index of first \0 char or size
  virtual SPtr<RIterator> iteratorForwardV() override;

  void toCharArray(char* pointer, uint16_t size);
  String toString();
  StrVal toVal();
  StrVal toValFull();

  void print(HardwareSerial& port);
  void printn(HardwareSerial& port);
  void printFull(HardwareSerial& port);
  void printnFull(HardwareSerial& port);

  int16_t indexOf(const StrRep& sample, uint16_t from = 0);
  int16_t indexOfLast(const StrRep& sample, uint16_t backFrom = 0xFFFF);

  bool operator==(const StrRep& other) const;  //content comparison

  StrSpan getSpan(uint16_t from, uint16_t until) const;

  SPtr<StrRep> getProxyLen();
};
class StrRep::LenSizeProxy : public Reference<StrRep> {  //nested proxy
public:
  using Reference<StrRep>::Reference;
  virtual uint16_t getSize() override;
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
  char* const pgmP = NULL;
public:
  FStr(__FlashStringHelper* pgmP)
    : pgmP((char*)pgmP) {}
  FStr(char* const pgmP)
    : pgmP(pgmP) {}
  virtual char readAt(uint16_t index) override {
    return pgm_read_byte(pgmP + index);
  }
  virtual uint16_t getSize() override {
    return strlen_P(pgmP);
  }
};
class FStrS : public FStr {
  uint16_t size;
public:
  FStrS(__FlashStringHelper* pgmP, uint16_t size)
    : FStr(pgmP), size(size) {}
  virtual uint16_t getSize() override {
    return size;
  }
};
class BoolStr : public FStr {
public:
  BoolStr(bool value) {
    value ? pgmP = F("true") : pgmP = F("false")
  }
};
#define Q(string_literal) (FStr(F(string_literal)))                //quick progmem string macro
#define QS(string_literal, size) (FStrS(F(string_literal), size))  //quick progmem sized string macro

class StrVar : public VariableRepresentation<StrRep> {  //Variable String  base class
protected:
  using RIterator = StrRep::RIterator;
  using DumbRiterator = StrRep::DumbRiterator;
  using VariableRepresentation<StrRep>::Reference;
  class LenSizeProxy;
public:
  char& operator[](uint16_t index) {
    return itemAt(index);
  }
  StrVar& truncate(uint16_t lengthLeft = 0);
  StrVar& fillWith(const StrRep& rep);
  StrVar& reverse();  //reverse this string
  StrVar& paste(const StrRep& rep, uint16_t from, uint16_t until = 0xFFFF);
  StrVar& append(const StrRep& rep);
  StrVar& replace(const StrRep& instances, const StrRep& with, uint16_t from = 0, uint16_t numberLimit = 0xFFFF);
  StrVar& replaceBack(const StrRep& instances, const StrRep& with, uint16_t backFrom = 0xFFFF, uint16_t numberLimit = 0xFFFF);
  StrVar& removeSpan(uint16_t from, uint16_t until);
  StrVar& insertAt(const StrRep& sample, uint16_t index);
  StrVar& insertBack(const StrRep& sample, uint16_t index);
  StrVal cut(uint16_t from, uint16_t until);

  StrSpanVar getSpanVar(uint16_t from, uint16_t until) const;

  SPtr<StrVar> getProxyVarLen();
};
class StrVar::LenSizeProxy : public StrVar::Reference {
public:
  using StrVar::Reference::Reference;
  virtual uint16_t getSize() override;
};
class StrChar : public StrVar {
protected:
  char letter;
public:
  StrChar(char letter)
    : letter(letter) {}
  StrChar(bool value)
    : StrChar(value ? '1' : '0') {}
  virtual char readAt(uint16_t index) override {
    return letter;
  }
  virtual uint16_t getSize() override {
    return 1;
  }
  virtual char& itemAt(uint16_t index) override {
    return letter;
  }
};

#define C(char_literal) StrChar(char_literal)

class StrVal : public StrVar {  //Value String class
protected:
  Array<char> array;
public:
  StrVal(uint16_t size)
    : array(size) {
    if (size) array[0] = '\0';
  }
  StrVal(const String& other)
    : StrVal(other.length()) {
    uint16_t limit = array.getSize();
    for (uint16_t i = 0; i < limit; ++i)
      array[i] = other[i];
  }
  StrVal(const StrVal& other)
    : array(other.array) {}
  StrVal(StrVal&& other)
    : array(move(other.array)) {}
  StrVal(const __FlashStringHelper* fsh) {
    FStr fs(fsh);
    uint16_t len = fs.getSize();
    array = Array<char>(len);
    for (uint16_t i = 0; i < len; ++i)
      array[i] = fs.readAt(i);
  }
  template<class T> StrVal(const T& other)
    : StrVal(String(other)) {}
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
    : pointer(pointer), size(size) {
    pointer[size] = '\0';
  }
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
    return size - 1;
  }
  virtual char& itemAt(uint16_t index) override {
    return pointer[index];
  }
};

//strCompose(StrSum&... args) - returns StrSum of arguments
template<typename T, typename = StrSum&>
StrSum strCompose(T first) {
  StrSum sum;
  sum.push(SPtr<StrRep>(&first));
  return sum;
}
template<typename T, typename... T2, typename = StrSum&>
StrSum strCompose(T first, T2... args) {
  return strCompose(args...).push(SPtr<StrRep>(&first));
}