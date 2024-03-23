#include <avr/eeprom.h>
#include "EEPROM.h"
#include <stdint.h>
#include "Settings.h"

EEPROMptr::EEPROMptr(uint16_t value) {
  if (value > 4095) {
    value = 4096;
  }
  this->value = value;
}
EEPROMptr::operator uint16_t() const {
  return value;
}
bool EEPROMptr::isValid() const {
  return value < 4096;
}

Config::Config(void* ptr, size_t size, const String& name, Config::Type type) {
  this->ptr = ptr;
  this->size = size;
  this->type = type;
  name.toCharArray(this->name, 6);
}
Config::Config(const Config& other) {
  for (uint8_t i = 0; i < 6; i++)
    this->name[i] = other.name[i];
  this->ptr = other.ptr;
  this->size = other.size;
}
ConfigWithPtr::ConfigWithPtr(EEPROMptr ptr, const Config& worseConfig) {
  for (uint8_t i = 0; i < 6; i++)
    this->name[i] = worseConfig.name[i];
  this->ptr = worseConfig.ptr;
  this->size = worseConfig.size;
  ePtr = ptr;
}
void ConfigWithPtr::save() const {
  for (uint8_t i = 0; i < size; i++) {
    EEPROM.put(ePtr + i, ptr + i);
  }
}

void ConfigWithPtr::load() const {
  for (uint8_t i = 0; i < size; i++)
    *(char*)(ptr + i) = eeprom_read_byte(ePtr + i);
}
void ConfigWithPtr::print() const {
  Serial.print(F("CwPtr: name="));
  Serial.print(name);
  Serial.print(F("\t size="));
  Serial.print(size);
  Serial.print(F("\t ePtr="));
  Serial.println(ePtr);
}
ConfigurableObject::initialize(const IConfigurable* object, const String& name, uint16_t nextPtr) {
  uint8_t size = object->numberOfConfigs();
  trackedSettings = new Array<ConfigWithPtr>(size);
  for (uint8_t i = 0; i < size; i++) {  //for all configs
    Config temp = object->getConfig(i);
#ifdef SETDebug
    Serial.print(F("\tsetting "));
    Serial.print(temp.name);
    Serial.print("\t");
    Serial.print(temp.size);
    Serial.print(F("\t at "));
    Serial.println(nextPtr);
#endif  //SETDebug
    (*trackedSettings)[i] = ConfigWithPtr(EEPROMptr(nextPtr), temp);
    nextPtr += temp.size;
  }
  name.toCharArray(this->name, 6);
}
void ConfigurableObject::print() const {
  for (ArrayIterator<ConfigWithPtr> config(trackedSettings); !config.isEnd(); config++)
    (*config).print();
}
void ConfigurableObject::saveAll() const {
  for (ArrayIterator<ConfigWithPtr> config(trackedSettings); !config.isEnd(); config++)
    (*config).save();
}
void ConfigurableObject::loadAll() const {
  for (ArrayIterator<ConfigWithPtr> config(trackedSettings); !config.isEnd(); config++)
    (*config).load();
}
const ConfigWithPtr& ConfigurableObject::getSetting(const String& configName) const {
#ifdef SETDebug
  Serial.print(F("Set search configName="));
  Serial.print(configName);
  Serial.print(F("\t size="));
  Serial.println(configName.length());
#endif  //SETDebug
  if ((trackedSettings != NULL) && (configName.length() < 6))
    for (ArrayIterator<ConfigWithPtr> config(trackedSettings); !config.isEnd(); config++)
      if (!configName.compareTo(String((*config).name))) {
#ifdef SETDebug
        Serial.println(F("Found"));
        (*config).print();
#endif  //SETDebug
        return *config;
      }
#ifdef SETDebug
  Serial.println(F("Not found"));
#endif  //SETDebug
  return ConfigWithPtr();
}

void EEPROM_register::addObject(IConfigurable* object, const String& name) {
  uint16_t nextPtr = 0;
  if (trackedObjects.getSize()) {
    const ConfigWithPtr& conf = trackedObjects.peek().trackedSettings->last();  //last created setting
    nextPtr = conf.ePtr.value + conf.size;
#ifdef SETDebug
    Serial.print(F("last eptr="));
    Serial.print(conf.ePtr.value);
    Serial.print(F("\tlast size="));
    Serial.print(conf.size);
#endif  //SETDebug
  }
#ifdef SETDebug
  Serial.print(F("Adding "));
  Serial.print(name);
  Serial.print(F(" next ePtr="));
  Serial.println(nextPtr);
#endif  //SETDebug

  trackedObjects.push(ConfigurableObject());  //add to tracked objects

#ifdef SETDebug
  Serial.println("ok");
#endif  //SETDebug
  trackedObjects.peek().initialize(object, name, nextPtr);
}
EEPROM_register::EEPROM_register() {
  //addObject(this, "reg");
}
const ConfigurableObject& EEPROM_register::getConfObject(const String& objName) const {
#ifdef SETDebug
  Serial.print(F("Obj search name="));
  Serial.println(objName);
#endif  //SETDebug
  if (objName.length() <= 6)
    for (StackIterator<ConfigurableObject> confObject(trackedObjects); !confObject.isEnd(); confObject++) {
#ifdef SETDebug
      Serial.print(F("Comparing to "));
      Serial.println((*confObject).name);
#endif  //SETDebug
      if (!objName.compareTo(String((*confObject).name))) {
#ifdef SETDebug
        Serial.println(F("Found obj"));
#endif  //SETDebug
        return *confObject;
      }
    }
  return ConfigurableObject();
}

const ConfigWithPtr& EEPROM_register::getSetting(const String& path) const {
  int separatorIndex = path.indexOf("/");
  return getConfObject(path.substring(0, separatorIndex - 1)).getSetting(path.substring(separatorIndex + 1, path.length() - 1));
}

void EEPROM_register::loadAllConfigs() const {
  for (StackIterator<ConfigurableObject> confObject(trackedObjects); !confObject.isEnd(); confObject++)
    (*confObject).loadAll();
}
void EEPROM_register::saveAllConfigs() const {
  for (StackIterator<ConfigurableObject> confObject(trackedObjects); !confObject.isEnd(); confObject++)
    (*confObject).saveAll();
}