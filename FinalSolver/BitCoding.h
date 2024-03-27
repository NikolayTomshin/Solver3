#pragma once
#include <stdint.h>
#include "Utilities.h"
#include "IO.h"
//data compression and bitwise manipulation

#define PermutationDebug
/*
Library for encoding multiple unsigned values in lesser number of bytes
*/
class BitCoding {
public:
  virtual ~BitCoding() = 0;

  static const String& binaryByteString(uint8_t value);
  static const String& binaryArrayString(const uint8_t value[], uint8_t size);
  static void writeBinaryString(const String& string, uint8_t valueArray[], uint8_t size);

  static uint8_t readBits(const uint8_t& valueByte, uint8_t bitIndex, uint8_t length);
  static bool readBit(const uint8_t& valueByte, uint8_t bitIndex);
  static void writeBits(uint8_t& valueByte, uint8_t value, uint8_t bitIndex, uint8_t length);
  static void writeBit(uint8_t& valueByte, bool value, uint8_t bitIndex);
  template<class T>
  static uint8_t sizeInBits(T number);
  static uint8_t sizeInBytes(uint16_t bits);
  static uint8_t sizeInBytesArray(uint8_t elementBitsLength, uint8_t arraySize);

  static uint8_t readBitsFromArray(const uint8_t array[], uint16_t bitIndex, uint8_t length);
  static uint8_t writeBitsToArray(uint8_t array[], uint8_t value, uint16_t bitIndex, uint8_t length);

  static uint8_t readSizedElement(const uint8_t array[], uint8_t elementBitsLength, uint8_t index);
  static void writeSizedElement(uint8_t array[], uint8_t value, uint8_t elementBitsLength, uint8_t index);

  static uint8_t sizeInBytesPermutation(uint8_t numberOfElements);
  static int16_t indexOfSized(const uint8_t source[], uint8_t arraySize, uint8_t searchValue, uint8_t elementBitSize);

  static void decodePermutation(uint8_t size, const uint8_t source[], uint8_t result[], uint8_t resultElementBits = 8);
  static void encodePermutation(uint8_t size, const uint8_t source[], uint8_t result[], uint8_t sourceElementBits = 8);
  static void readPermutation(uint8_t size, const uint8_t source[]);

  static void printByte(const uint8_t& value);
  static void printArray(const uint8_t array[], uint8_t end, uint8_t beg = 0);
  static void printSizedArray(const uint8_t array[], uint8_t size, uint8_t elementBitSize = 8);

  static bool compareBitsFromArrays(const uint8_t arrayA[], uint16_t bitIndexA, const uint8_t arrayB[], uint16_t bitIndexB, uint16_t length);

  static void compressArray(const uint8_t uncompressed[], uint8_t compressed[], uint8_t elementLength, uint8_t size);
  static void decompressArray(const uint8_t compressed[], uint8_t uncompressed[], uint8_t elementLength, uint8_t size);
  static void mergeBitSort(uint8_t* array, uint8_t bitLength, uint8_t size, bool ascending = true);
protected:

  class ArrayCursor {
    uint16_t bitIndex;
  public:
    uint8_t* array;
    ArrayCursor(uint8_t* arr, uint16_t bitindex = 0);
    void setBitIndex(uint16_t bitindex);
    uint8_t readBits(uint8_t length, bool right = true);
  };
  class BitSize {
  public:
    uint8_t bitLength;
    uint8_t maxNumber;
    BitSize(uint8_t bits);
    bool operator<(const uint8_t& other) const;
    void operator++();
    void operator--();
    uint8_t getBitLength(int8_t delta);
  };
  static uint8_t getMaskFirst(uint8_t length);
  static uint8_t getMaskLast(uint8_t length);
  static uint8_t getMask(uint8_t startIndex, uint8_t length);
};

template<class T>
static uint8_t BitCoding::sizeInBits(T number) {
  uint8_t size = 0;
  while (number > 0) {
    number = number >> 1;
    size++;
  }
  return size;
}
