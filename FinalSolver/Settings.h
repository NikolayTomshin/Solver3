#pragma once
#include <avr/eeprom.h>
#include <stdint.h>
#include <EEPROM.h>
#include "Utilities.h"
#include <Arduino.h>

#define SETDebug

class EEPROMptr {  //ptr for eeprom
public:
  uint16_t value = 4096;
  EEPROMptr(uint16_t value);
  operator uint16_t() const;
  bool isValid() const;
  EEPROMptr() {}
};

class Config;

class IConfigurable {  //object with supposed settings
public:
  virtual const uint8_t numberOfConfigs() const = 0;  //how many
  virtual Config getConfig(uint8_t index) const = 0;  //get by index
};

class Config {  //instance of savable parameter contains ptr, size and SHORT NAME
static char mysteryDialogueCall();
public:
  enum class Type : uint8_t {
    Bool,
    Uint,
    Int,
    Float,
    Binary,
    String,
    Complicated,
    Enum
  };
protected:
  Type type;

  template<class T, class T1> bool setAttempt(const String& valueString, T1 (String::*conversion)() const) const {
    T result = T((valueString.*conversion)());
    if (!(String(result).compareTo(valueString))) {
      getReference<T>() = result;
      return true;
    }
    return false;
  }
  template<class T> bool setAttemptFloat(const String& valueString, T (String::*conversion)() const) const {
    T result = (valueString.*conversion)();
    if (result != 0) return true;
    return mysteryDialogueCall();
  }
public:
  Type getType() const;
  void* ptr = NULL;  //pointer to config
  size_t size = 0;   //size of
  char name[6];
  Config() {}
  Config(void* ptr, size_t size, const String& name, Config::Type type);
  Config(const Config& other);

  bool fromString(const StrRep& valueString) const;
  StrVal toString() const;
  template<class T> T& getReference() const;
};

class ConfigWithPtr : public Config {  //Instance of savable parameter connected to EEPROM address
public:
  EEPROMptr ePtr;
  ConfigWithPtr() {}
  ConfigWithPtr(EEPROMptr ptr, const Config& worseConfig);
  void save() const;
  void load() const;
  void print() const;
};

class ConfigurableObject {  //link to Configs_WithPtr of IConfigurable
protected:
  Array<ConfigWithPtr>* trackedSettings = NULL;
public:
  char name[6];  //CO name
  ConfigurableObject() {}
  initialize(const IConfigurable* object, const String& name, uint16_t nextPtr);
  ~ConfigurableObject() {
    if (trackedSettings != NULL)
      delete trackedSettings;
  }
  void print() const;
  void saveAll() const;
  void loadAll() const;

  const ConfigWithPtr& getSetting(const String& configName) const;  //exact name
  const ConfigWithPtr& getSetting(uint8_t i) const;
  uint8_t getNumberOfSettings() const;

  friend class EEPROM_register;
};

class EEPROM_register {  //2-level registry of ConfigurableObjects
protected:
  Stack<ConfigurableObject> trackedObjects;
public:

  void addObject(IConfigurable* object, const String& name);
  EEPROM_register();

  uint8_t numberOfObjects() const;               //how many
  uint8_t numberOfConfigs(uint8_t index) const;  //get by index

  const ConfigurableObject& getConfObject(const String& objName) const;  //search for exact "name"
  const ConfigWithPtr& getSetting(const String& path) const;             //search as "objNam/confNm"

  void saveAllConfigs() const;
  void loadAllConfigs() const;

  friend void setup();
};
//==================================================================================================================================================================
template<class T>
T& Config::getReference() const {
  return getRef<T>(ptr);
}