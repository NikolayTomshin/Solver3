namespace bitCoding {

const uint8_t unts = 0b11111111;
uint8_t previousBase = 0, bitMask, maskLength;  //prev base of numbers, bitmask and it's length
uint8_t _index;

void printByte(uint8_t a) {
  for (int8_t i = 7; i > -1; i--) {
    Serial.print((a >> i) & 1);
  }
}

void printArray(uint8_t arr[], uint8_t a, uint8_t b, bool decimal) {
  for (a; a < b + 1; a++) {
    Serial.print(a);
    Serial.print(":");
    if (!decimal) {
      printByte(arr[a]);
    } else Serial.print(arr[a]);
    Serial.println();
  }
}

uint8_t getBitMask(uint8_t length) {  //get byte mask of length
  return (~(unts << length));         //thats what the mask is
}

void setBitMask(uint8_t l) {
  bitMask = getBitMask(l);  //0b001..1
}
uint8_t getBitLength(uint8_t v) {
  uint8_t l = 0;
  while (v > 0) {
    v >>= 1;
    l++;
  }
  return (l);
}
void setBitMaskBased(uint8_t base) {  //base >1
  if (base != previousBase) {
    // Serial.print("pB:");
    // Serial.println(base);
    previousBase = base;
    base--;
    maskLength = 0;
    while (base > 0) {  //for each value bit of size
      base >>= 1;
      maskLength++;
    }
    setBitMask(maskLength);
    // Serial.print("mL:");
    // Serial.println(maskLength);
    // Serial.print("bM:");
    // printByte(bitMask);
    // Serial.println();
  }
}

void storeBits(uint8_t byteIndex, uint8_t bitIndex, uint8_t value, uint8_t array[]) {
  array[byteIndex] = (array[byteIndex] & ~(bitMask << bitIndex)) | (value << bitIndex);
  bitIndex = bitIndex + maskLength;        //индекс начала следующего числа
  if (bitIndex > 8) {                      //если следующее число начинается после первого бита след числа т.е. наше число записано на двух байтах
    bitIndex = maskLength + 8 - bitIndex;  //теперь это необходимый сдвиг в сторону предыдущего байта
    byteIndex++;
    array[byteIndex] = (array[byteIndex] & ~(bitMask >> bitIndex)) | (value >> bitIndex);
  }
}

uint8_t extractBits(uint8_t byteIndex, uint8_t bitIndex, uint8_t array[]) {
  if (bitIndex + maskLength > 8) {  //если запрашиваемое число сидит на двух байтах
    return (((array[byteIndex + 1] << (8 - bitIndex)) & bitMask) | (array[byteIndex] >> bitIndex));
  }
  return ((array[byteIndex] >> bitIndex) & bitMask);
}

void writeBits(uint8_t bitIndex, uint8_t length, uint8_t value, uint8_t array[]) {
  setBitMask(length);
  storeBits(bitIndex / 8, bitIndex % 8, value, array);
}
uint8_t getBits(uint8_t bitIndex, uint8_t length, uint8_t array[]) {
  setBitMask(length);
  return (extractBits(bitIndex / 8, bitIndex % 8, array));
}
//write into byte array of encoded numbers with constant bit length  *32 bytes max array length
void writeBased(uint8_t base, uint8_t value, uint8_t index, uint8_t array[]) {
  setBitMaskBased(base);
  base = maskLength * index;  //now it's bit index
  index = base % 8;           //now it's locale bit index
  storeBits(base / 8, index, value, array);
}

uint8_t getBased(uint8_t base, uint8_t index, uint8_t array[]) {
  setBitMaskBased(base);
  base = maskLength * index;  //now it's bit index
  index = base % 8;           //now it's locale bit index
  return (extractBits(base / 8, index, array));
}

//source-full sized array of numbers from 0 to size-1 in any order
//result - array of numbers with upper limit of size-i that encodes permutation of indexes from size-1 to 0
void encodePermutation(uint8_t size, uint8_t source[], uint8_t result[]) {  //size>1;
  uint8_t positionBits = getBitLength(size - 1);                            //length in bits
  uint8_t limitNumber = getBitMask(positionBits - 1);                       //next number that takes 1 less bit to write
  uint8_t position;
  uint8_t cursor = 0;
  for (uint8_t i = size - 1; i > 0; i--) {  //выбор индексов для поиска по убывающей
    position = 0;
    for (uint8_t j = 0; j < size; j++) {  //поиск индексов
      uint8_t value = source[j];
      if (value < i) {  //Если индекс меньше цели значит он не находится тут и будет правее т.е. увеличиваем позицию
        position++;
      } else if (value == i) {  //если индекс находится тут значит выходим из цикла и используем последнюю позицию
        break;
      }  //иначе если индекс больше цели значит его на этом этапе размещения ещё не существует и позицию не прибавляем
    }
    // Serial.print("iV:");
    // Serial.print(i);
    // Serial.print("; pos:");
    // Serial.print(position);
    // Serial.print("; len:");
    // Serial.println(positionBits);
    writeBits(cursor, positionBits, position, result);
    cursor += positionBits;
    if (limitNumber >= position) {  //если позиция достигла нижнего предела, можно сделать меньше длину битов
      positionBits--;
      limitNumber >>= 1;
    }
  }
}
void decodePermutation(uint8_t size, uint8_t source[], uint8_t result[]) {
  uint8_t positionBits = getBitLength(size - 1);
  uint8_t limitNumber = getBitMask(positionBits - 1);
  uint8_t cursor = 0;
  uint8_t i,k;
  for (i = 0; i < size; i++) {  //mark all indexes as last index 0
    result[i] = 0;
  }
  for (i = size - 1; i > 0; i--) {  //начиная с последнего индекса размещаем по 1, 0 остаётся автоматом.
    if (i <= limitNumber) {                 //если позиция достигла нижнего предела, можно сделать меньше длину битов
      positionBits--;
      limitNumber >>= 1;
    }
    // Serial.print("bits:");
    // Serial.print(positionBits);
    // Serial.println(";");
    k = getBits(cursor, positionBits, source);  //сколько неустановленных значений нужно пропустить
    // Serial.print("i=");
    // Serial.print(i);
    // Serial.print("; k=");
    // Serial.print(k);
    for (uint8_t j = 0; j < size; j++) {
      if (result[j] == 0) {
        if (k == 0) {
          result[j] = i;
          // Serial.print("put in ");
          // Serial.println(j);
          break;
        }
        k--;
      }
    }
    cursor += positionBits;
  }
}
}