#pragma once
#include <Arduino.h>
#include "Utilities.h"

class StrVal;          //char array
class StrComposition;  //Composition of existing StrRefs

class StrRep {  //universal string representation
public:
  virtual ~StrRep() = 0;
  //StrReference can be conveted to String
  virtual explicit operator String() const = 0;
  //or better to StrVal
  virtual explicit operator StrVal() const = 0;

  //Str always have length
  virtual uint16_t length() const = 0;
  //Print from hardware port. May specify max amount of characters to print from the beginning
  virtual void print(HardwareSerial& port, uint8_t length = 0xffff) = 0;

  //also can be pasted onto char array
  virtual void toCharArray(char* charPtr, uint16_t length = 0) const = 0;

  //characters can be accesed as in array
  virtual char& operator[](uint16_t index) = 0;       //: IndexedCollection<char>
  virtual char operator[](uint16_t index) const = 0;  //: IndexedCollection<char>
  //get substring
  virtual StrRep* substring(uint16_t from, uint16_t until) const = 0;
  //find substrings on char span left to right
  virtual uint16_t indexOf(const StrRep& str, uint16_t from, uint16_t until) const = 0;
  //and right to left
  virtual uint16_t lastIndexOf(const StrRep& str, uint16_t from, uint16_t until) const = 0;
  //edit representation
  //cut out span
  virtual void cutInside(uint16_t from, uint16_t until) = 0;
  //cut parts outside span
  virtual void cutOutside(uint16_t from, uint16_t until) = 0;
  //insert string
  virtual void insertAt(const StrRep& str, uint16_t from) = 0;
  //overtype string from beginning
  virtual void overtypeSpan(const StrRep& str, uint16_t from, uint16_t until) = 0;
  //overtype string from end
  virtual void overtypeSpanFromEnd(const StrRep& str, uint16_t from, uint16_t until) = 0;
};
class StrRef : public StrRep {  //universal string reference, non const functions will affect binded data
};
class StrNTP : public StrRef {  //Ponter to existing NullTerminated char array
protected:
  char* ptr;
public:
  StrNTP() {}
  StrNTP(StrVal& strVal);
  StrNTP(char* charPtr);

  void setPtr(char* charPtr);

  //StrReference can be conveted to String
  virtual explicit operator String() const override;
  //or better to StrVal
  virtual explicit operator StrVal() const override;

  //Str always have length
  virtual uint16_t length() const override;
  //and can be printed from harware port
  virtual void print(HardwareSerial& port, uint8_t length = 0xffff) override;

  //also can be pasted onto char array
  virtual void toCharArray(char* charPtr, uint16_t length = 0) const override;

  virtual char& operator[](uint16_t index) override;  //characters can be accesed as in array
  virtual char operator[](uint16_t index) const override;
  //get information from representation
  //get substring
  virtual StrRep* substring(uint16_t from, uint16_t until) const override;
  //find substrings on char span left to right
  virtual uint16_t indexOf(const StrRep& str, uint16_t from, uint16_t until) const override;
  //and right to left
  virtual uint16_t lastIndexOf(const StrRep& str, uint16_t from, uint16_t until) const override;
  //edit representation
  //cut out span
  virtual void cutInside(uint16_t from, uint16_t until) override;
  //cut parts outside span
  virtual void cutOutside(uint16_t from, uint16_t until) override;
  //insert string
  virtual void insertAt(const StrRep& str, uint16_t from) override;
  //overtype string from beginning
  virtual void overtypeSpan(const StrRep& str, uint16_t from, uint16_t until) override;
  //overtype string from end
  virtual void overtypeSpanFromEnd(const StrRep& str, uint16_t from, uint16_t until) override;

  operator char*() const {
    return ptr;
  }
};
class StrCPL : public StrNTP {  //Pointer to char array with length limit
protected:
  uint16_t limit;
public:
};
class StrVal : public StrNTP {  //Keeps dynamic char array by itself
private:
  using StrNTP::setPtr;  //setting ptr not allowed
public:

  StrVal(const String& string);            //make from String :(
  StrVal(const __FlashStringHelper* fsh);  //make from __FlashStringHelper 👍
  StrVal(const StrRep& str);
  StrVal(char* charPtr);

  ~StrVal() {
    if (ptr != NULL) delete[] ptr;
  }
};

struct StringPart;
class StrComposition : private Stack<StringPart>, public StrRep {
protected:
  struct StringPart {
    const StrRep& strRep;
    uint16_t length;
    SumPart(const StrRep& strRep) {
      this->strRep = strRep;
      length = strRep.length();
    }
  };
public:
  //StrReference can be conveted to String
  virtual explicit operator String() const override;
  //or better to StrVal
  virtual explicit operator StrVal() const override;

  //Str always have length
  virtual uint16_t length() const override;
  //Print from hardware port. May specify max amount of characters to print from the beginning
  virtual void print(HardwareSerial& port, uint8_t length = 0xffff) override;

  //also can be pasted onto char array
  virtual void toCharArray(char* charPtr, uint16_t length = 0) const override;

  //characters can be accesed as in array
  // virtual char& operator[](uint16_t index) override;
  // //characters can be accesed as in const array
  // virtual char operator[](uint16_t index) const override;
  //Get information from representation
  //get substring
  virtual StrRep* substring(uint16_t from, uint16_t until) const override;
  //find substrings on char span left to right
  virtual uint16_t indexOf(const StrRep& str, uint16_t from, uint16_t until) const override;
  //and right to left
  virtual uint16_t lastIndexOf(const StrRep& str, uint16_t from, uint16_t until) const override;
  //edit representation
  //cut out span
  virtual void cutInside(uint16_t from, uint16_t until) override;
  //cut parts outside span
  virtual void cutOutside(uint16_t from, uint16_t until) override;
  //insert string
  virtual void insertAt(const StrRep& str, uint16_t from) override;
  //overtype string from beginning
  virtual void overtypeSpan(const StrRep& str, uint16_t from, uint16_t until) override;
  //overtype string from end
  virtual void overtypeSpanFromEnd(const StrRep& str, uint16_t from, uint16_t until) override;
};