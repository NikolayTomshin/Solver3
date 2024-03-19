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
public:
  enum class Type : uint8_t {
    Bool,
    Uint,
    Int,
    Float,
    Binary,
    String,
    Complicated
  };
protected:
  Type type;
public:
  Type getType() const;
  void* ptr = NULL;  //pointer to config
  size_t size = 0;   //size of
  char name[6];
  Config() {}
  Config(void* ptr, size_t size, const String& name, Config::Type type);
  Config(const Config& other);

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

  friend class EEPROM_register;
};

class EEPROM_register : public IConfigurable {  //2-level regestry of ConfigurableObjects
protected:
  ValueStack<ConfigurableObject> trackedObjects;
public:
  bool loadAll = false;

  void addObject(IConfigurable* object, const String& name);
  EEPROM_register();

  virtual const uint8_t numberOfConfigs() const override;  //how many
  virtual Config getConfig(uint8_t index) const override;  //get by index

  uint8_t numberOfObjects() const;               //how many
  uint8_t numberOfConfigs(uint8_t index) const;  //get by index

  const ConfigWithPtr& getSetting(const String& path) const;
  void loadAllConfigs() const;
};

/*class Test : public IConfigurable {
  int a, b, c;
  virtual const uint8_t numberOfConfigs() const override {
    return 2;
  }
  virtual Config getConfig(uint8_t index) const override {
    switch (index) {
      case 0: return Config((char*)&a, sizeof(a), "a");
      case 1: return Config((char*)&b, sizeof(b), "b");
    }
    return Config();
  }
};
*/
//==================================================================================================================================================================
template<class T>
T& Config::getReference() const {
  return getRef<T>(ptr);
}