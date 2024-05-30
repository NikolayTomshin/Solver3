#include "BitCoding.h"


StrVal BitCoding::binaryByteString(uint8_t value) {
  StrVal chars(8);
  for (uint8_t i = 0; i < 8; ++i)
    chars[i] = readBit(value, i) ? '1' : '0';
  return chars;
}
StrVal BitCoding::binaryArrayString(const uint8_t value[], const uint8_t size) {
  const uint8_t charSize = size * 8;
  StrVal chars(charSize);  //+1 for '\0'
  for (uint8_t i = 0; i < charSize; ++i)
    chars[i] = readBit(value, i) ? '1' : '0';
  return chars;
}
void BitCoding::writeBinaryString(const String& string, uint8_t valueArray[], uint8_t size) {
  uint8_t limit = minMax<uint8_t>(size * 8, string.length() | 0b11111111, false);
  uint8_t stepLimit = 8;
  uint8_t byteIndex = 0;
  for (uint8_t i = 0; i < limit; ++i) {
    if (i == stepLimit) {
      ++valueArray;
      byteIndex = 0;
      stepLimit += 8;
    }
    writeBit(*valueArray, string[i] == '1', byteIndex);
    ++byteIndex;
  }
}

uint8_t BitCoding::getMaskFirst(uint8_t length) {
  return ~(0b11111111 << length);
}
uint8_t BitCoding::getMaskLast(uint8_t length) {
  return ~(0b11111111 >> length);
}
uint8_t BitCoding::getMask(uint8_t startIndex, uint8_t length) {
  return ~(0b11111111 << length) << startIndex;
}
uint8_t BitCoding::readBits(const uint8_t& valueByte, uint8_t bitIndex, uint8_t length) {
  uint8_t val = valueByte;
  return (val >> bitIndex) & getMaskFirst(length);
}
bool BitCoding::readBit(const uint8_t& valueByte, uint8_t bitIndex) {
  return readBits(valueByte, bitIndex, 1);
}
void BitCoding::writeBits(uint8_t& valueByte, uint8_t value, uint8_t bitIndex, uint8_t length) {
  uint8_t mask = getMask(bitIndex, length);
  valueByte = (valueByte & (~mask)) | ((value << bitIndex) & mask);
}
void BitCoding::writeBit(uint8_t& valueByte, bool value, uint8_t bitIndex) {
  writeBits(valueByte, value, bitIndex, 1);
}
uint8_t BitCoding::sizeInBytes(uint16_t bits) {
  return bits / 8 + ((bits % 8) > 0);
}
uint8_t BitCoding::sizeInBytesArray(uint8_t elementBitsLength, uint8_t arraySize) {
  return sizeInBytes(uint16_t(arraySize) * elementBitsLength);
}

uint8_t BitCoding::readBitsFromArray(const uint8_t array[], uint16_t firstBitIndex, uint8_t bitsLength) {
  uint8_t firstByteIndex = firstBitIndex / 8;
  firstBitIndex %= 8;  //in byte
  int8_t lastBitInNextElement = firstBitIndex + bitsLength - 9;
  uint8_t retValue = (array[firstByteIndex] >> firstBitIndex) & getMaskFirst(bitsLength);  //first part
  if (lastBitInNextElement >= 0) {                                                         //first part is cut by byte
    retValue = ((array[firstByteIndex + 1] & getMaskFirst(lastBitInNextElement + 1)) << (8 - firstBitIndex)) | retValue;
  }
  return retValue;
}
uint8_t BitCoding::writeBitsToArray(uint8_t array[], uint8_t value, uint16_t firstBitIndex, uint8_t bitsLength) {
  uint8_t firstByteIndex = firstBitIndex / 8;
  firstBitIndex %= 8;  //in byte
  int8_t bitSizeInNextByte = firstBitIndex + bitsLength - 8;
  writeBits(array[firstByteIndex], value, firstBitIndex, bitsLength);
  if (bitSizeInNextByte > 0) {  //first part is cut by byte
    writeBits(array[firstByteIndex + 1], value >> (bitsLength - bitSizeInNextByte), 0, bitSizeInNextByte);
  }
}
uint8_t BitCoding::readSizedElement(const uint8_t array[], uint8_t elementBitsLength, uint8_t index) {
  return readBitsFromArray(array, uint16_t(elementBitsLength) * index, elementBitsLength);
}
void BitCoding::writeSizedElement(uint8_t array[], uint8_t value, uint8_t elementBitsSize, uint8_t index) {
  writeBitsToArray(array, value, uint16_t(elementBitsSize) * index, elementBitsSize);
}

uint8_t BitCoding::sizeInBytesPermutation(uint8_t numberOfElements) {
  uint16_t bits = 0;
  BitSize limit(uint8_t(1));
  for (uint8_t elementCount = 1; elementCount < numberOfElements; elementCount++) {
    if (limit < elementCount) {
      limit++;
    }
    bits += limit.bitLength;
  }
  return sizeInBytes(bits);
}
int16_t BitCoding::indexOfSized(const uint8_t source[], uint8_t arraySize, uint8_t searchValue, uint8_t elementBitSize) {
  for (uint8_t i = 0; i < arraySize; i++)
    if (readSizedElement(source, elementBitSize, i) == searchValue) return i;
  return -1;
}
void BitCoding::printByte(const uint8_t& value) {
  for (uint8_t j = 7; j < 255; j--)
    pout(readBit(value, j));
}
void BitCoding::printArray(const uint8_t array[], uint8_t end, uint8_t beg = 0) {
  for (uint8_t i = beg; i <= end; i++) {
    printByte(array[i]);
    pnl();
  }
}
void BitCoding::printSizedArray(const uint8_t array[], uint8_t size, uint8_t elementBitSize = 8) {
  for (uint8_t i = 0; i < size;) {
    pout(readSizedElement(array, elementBitSize, i));
    if ((++i) < size) pout(", ");
  }
}

bool BitCoding::compareBitsFromArrays(const uint8_t arrayA[], uint16_t bitIndexA, const uint8_t arrayB[], uint16_t bitIndexB, uint16_t length) {
  ArrayCursor first(arrayA, bitIndexA);
  ArrayCursor last(arrayB, bitIndexB);
  uint8_t bitsToRead;
  while (length) {
    bitsToRead = limits<uint16_t>(length, 8);
    if (first.readBits(bitsToRead) != last.readBits(bitsToRead)) return false;
    length -= bitsToRead;
  }
  return true;
}



void BitCoding::compressArray(const uint8_t uncompressed[], uint8_t compressed[], uint8_t elementLength, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    writeSizedElement(compressed, uncompressed[i], elementLength, i);
  }
}
void BitCoding::decompressArray(const uint8_t compressed[], uint8_t uncompressed[], uint8_t elementLength, uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    uncompressed[i] = readSizedElement(compressed, elementLength, i);
  }
}
void BitCoding::mergeBitSort(uint8_t array[], uint8_t bitLength, const uint8_t size, bool ascending = true) {
  uint8_t temp[size];
  decompressArray(array, temp, bitLength, size);
  valuesMergeSort(temp, size, ascending);
  compressArray(temp, array, bitLength, size);
}

// /*
void BitCoding::decodePermutation(uint8_t size, const uint8_t source[], uint8_t result[], uint8_t resultElementBits = 8) {
  for (uint8_t i = 0; i < size; i++) {
    writeSizedElement(result, 0, resultElementBits, i);
  }
  uint16_t bitCursor = 0;
  uint8_t placingIndex = size - 1;
  uint8_t lowestUnoccupied = 0;


  for (; placingIndex > 0; placingIndex--) {
    uint8_t bitSize = sizeInBits(placingIndex);
    uint8_t relativePlacement = readBitsFromArray(source, bitCursor, bitSize);
#ifdef PermutationDebug
    pout(placingIndex);
    pout("\t");
    pout(relativePlacement);
    pout("\t");
    pout(bitSize);
    pout("\t");
    pout(bitCursor % 8);
    pout("b\t");
    pout(bitCursor / 8);
    poutN("B\t");
#endif
    for (uint8_t i = 0; i <= relativePlacement; i++) {  //for each element until relative Unoccupied
      uint8_t thisElement = readSizedElement(result, resultElementBits, i);
      if (thisElement != 0)
        relativePlacement++;
    }
    writeSizedElement(result, placingIndex, resultElementBits, relativePlacement);
    bitCursor += bitSize;
  }
}

void BitCoding::encodePermutation(uint8_t size, const uint8_t source[], uint8_t result[], uint8_t sourceElementBits = 8) {
  int16_t cursor = 0;
  for (uint8_t index = size - 1; index > 0; index--) {
    uint8_t bitStep = sizeInBits(index);
    uint8_t place = indexOfSized(source, size, index, sourceElementBits);
    uint8_t delta = 0;
    for (uint8_t i = 0; i < place; i++) {
      if (readSizedElement(source, sourceElementBits, i) > index) {
        delta++;
      }
    }

#ifdef PermutationDebug
    pout(index);
    pout("\t");
    pout(place);
    pout("\t");
#endif
    place -= delta;
#ifdef PermutationDebug
    pout(place);
    pout("\t");
    pout(bitStep);
    pout("\t");
    pout(cursor % 8);
    pout("b\t");
    pout(cursor / 8);
    poutN("B\t");
#endif
    writeBitsToArray(result, place, cursor, bitStep);
    cursor += bitStep;
  }
}

BitCoding::ArrayCursor::ArrayCursor(uint8_t* arr, uint16_t bitindex = 0) {
  array = arr;
  bitIndex = bitindex;
}
void BitCoding::ArrayCursor::setBitIndex(uint16_t bitindex) {
  bitIndex = bitindex;
}
uint8_t BitCoding::ArrayCursor::readBits(uint8_t length, bool right = true) {
  return readBitsFromArray(array, right ? bitIndex : (bitIndex - length + 1), length);
}


BitCoding::BitSize::BitSize(uint8_t bits) {
  bitLength = bits;
  maxNumber = BitCoding::getMaskFirst(bits);
}
bool BitCoding::BitSize::operator<(const uint8_t& other) const {
  return maxNumber < other;
}
void BitCoding::BitSize::operator++() {
  *this = BitSize(bitLength + 1);
}
void BitCoding::BitSize::operator--() {
  *this = BitSize(bitLength - 1);
}
uint8_t BitCoding::BitSize::getBitLength(int8_t delta) {
  return bitLength + delta;
}