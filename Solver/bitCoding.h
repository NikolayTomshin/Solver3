#pragma once
#include <HardwareSerial.h>
#include <stdint.h>

namespace bitCoding {

void printByte(uint8_t a);

void printArray(uint8_t arr[], uint8_t a, uint8_t b, bool decimal);

uint8_t getBitMask(uint8_t length);

void setBitMask(uint8_t l);

uint8_t getBitLength(uint8_t v);

void setBitMaskBased(uint8_t base);

void storeBits(uint8_t byteIndex, uint8_t bitIndex, uint8_t value, uint8_t array[]);

uint8_t extractBits(uint8_t byteIndex, uint8_t bitIndex, uint8_t array[]);

void writeBits(uint8_t bitIndex, uint8_t length, uint8_t value, uint8_t array[]);

uint8_t getBits(uint8_t bitIndex, uint8_t length, uint8_t array[]);

uint8_t requiredBytes(uint8_t bits);

uint8_t basedArrayLengthInBits(uint8_t base, uint8_t size);

uint8_t basedArrayLengthInBytes(uint8_t base, uint8_t size);

uint8_t permutationEncodingBits(uint8_t size);

uint8_t permutationEncodingBytes(uint8_t size);

void writeBased(uint8_t base, uint8_t value, uint8_t index, uint8_t array[]);

uint8_t getBased(uint8_t base, uint8_t index, uint8_t array[]);

void encodePermutation(uint8_t size, uint8_t source[], uint8_t result[]);

void decodePermutation(uint8_t size, uint8_t source[], uint8_t result[]);

void encodeBased(uint8_t base, uint8_t size, uint8_t source[], uint8_t result[]);

void decodeBased(uint8_t base, uint8_t size, uint8_t source[], uint8_t result[]);

}