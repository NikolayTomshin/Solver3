#include <Arduino.h>
#include "Utilities.h"

class StrVal;          //char array
class StrComposition;  //Composition of existing StrRefs

class StrRef {  //universal string reference
public:
  virtual explicit operator String() const = 0;  //StrReference can be conveted to String
  virtual explicit operator StrVal() const = 0;  //or to smart array of chars
  virtual uint16_t length() const = 0;           //have length
  virtual void print(HardwareSerial& port) = 0;  //Can be printed from

  virtual void toCharArray(char* charPptr) const = 0;  //printed to char array buffer

  virtual char& operator[](uint16_t index) = 0;  //Have access to characters by index
  virtual const char& operator[](uint16_t index) const = 0;

  virtual StrRef* substring(uint16_t from, uint16_t until) const = 0;                //get substring
  virtual uint16_t indexOf(const& StrRef, uint16_t from, uint16_t until) const = 0;  //find substrings
  virtual uint16_t lastIndexOf(const& StrRef, uint16_t from, uint16_t until) const = 0;
  virtual void cutInside(uint16_t from, uint16_t until) = 0;
  virtual void cutOutside(uint16_t from, uint16_t until) = 0;
  virtual void virtual StrVal cutSubstringVal(uint16_t from, uint16_t until) = 0;
  virtual StrRef* cutSubstringRef(uint16_t from, uint16_t until) = 0;
};
class StrNTP : public StrRef {  //Ponter to existing NullTerminated char array
protected:
  char* ptr;
public:
  StrNTP() {}
  StrNTP(StrVal& strVal);
  void setPtr(char* charPtr);


  virtual explicit operator String() const override;  //StrReference can be conveted to String
  virtual explicit operator StrVal() const override;  //or to array of chars
  virtual uint16_t length() const override;           //have length
  virtual void print(HardwareSerial& port) override;  //Can be printed from

  virtual void toCharArray(char* charPptr) const override;  //printed to char array buffer

  virtual char& operator[](uint16_t index) override;  //Have access to characters by index
  virtual const char& operator[](uint16_t index) const override;

  virtual StrRef* substring(uint16_t from, uint16_t until) const override;                //get substring
  virtual uint16_t indexOf(const& StrRef, uint16_t from, uint16_t until) const override;  //find substrings
  virtual uint16_t lastIndexOf(const& StrRef, uint16_t from, uint16_t until) const override;
  virtual void cutInside(uint16_t from, uint16_t until) override;
  virtual void cutOutside(uint16_t from, uint16_t until) override;
  virtual StrVal cutSubstringVal(uint16_t from, uint16_t until) override;
  virtual StrRef* cutSubstringRef(uint16_t from, uint16_t until) override;


  operator char*() const {
    return ptr;
  }
};
class StrVal : public StrNTP {  //Keeps dynamic char array by itself
private:
  using void StrNTP::setPtr(char* charPtr);
public:
  StrVal(const String& string);
  StrVal(const StrRef& str);
  StrVal(char* charPtr);
  virtual void setPtr(char* charPtr) = 0;

  ~StrVal() {
    if (ptr != NULL) delete[] ptr;
  }
};

struct StringPart;
class StrComposition : public Stack<StringPart>, public StrRef {
protected:
  struct StringPart {
    StrRef& strRef;
    uint16_t length;
    SumPart(StrRef& strRef) {
      this->strRef = strRef;
      length = strRef.length();
    }
  };
public:
  virtual explicit operator String() const override;  //StrReference can be conveted to String
  virtual explicit operator StrVal() const override;  //or to array of chars
  virtual uint16_t length() const override;           //have length
  virtual void print(HardwareSerial& port) override;  //Can be printed from

  virtual void toCharArray(char* charPptr) const override;  //printed to char array buffer

  virtual char& operator[](uint16_t index) override;  //Have access to characters by index
  virtual const char& operator[](uint16_t index) const override;

  virtual StrRef* substring(uint16_t from, uint16_t until) const override;                //get substring
  virtual uint16_t indexOf(const& StrRef, uint16_t from, uint16_t until) const override;  //find substrings
  virtual uint16_t lastIndexOf(const& StrRef, uint16_t from, uint16_t until) const override;
  virtual void cutInside(uint16_t from, uint16_t until) override;
  virtual void cutOutside(uint16_t from, uint16_t until) override;
  virtual StrRef* cutSubstringRef(uint16_t from, uint16_t until) override;
};