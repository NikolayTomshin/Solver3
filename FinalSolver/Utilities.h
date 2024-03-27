#pragma once
#include <stdint.h>
#include "IO.h"

//frequently used funcs

// #define SortDebug
void spn();

bool& f(bool& other);
const String& boolStr(bool value);
bool fromString(const String& boolString);

int8_t Mod(int8_t period, int8_t argument);
int8_t Mod3(int8_t argument);
int8_t Mod4(int8_t argument);
int8_t Mod8(int8_t argument);

char digitOf(uint8_t i);
uint8_t valueOf(char c);

int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength);

uint16_t endOfTheLine(const String& string, const uint8_t& lineSize, const uint16_t& lineStart, const String& separator);
uint16_t indexOfSkipping(const String& string, uint16_t startingIndex, const String& separator, bool directionForward = true);

template<class T> T minMax(T a, T b, bool max);
template<class T> T assignSign(bool positive, T argument);
template<class T> T flipSign(bool flip, const T& argument);
template<class T> T limits(T value, T limit, bool upperLimit = true);
template<class T> T doubleLimits(T value, T lLimit, T uLimit);
template<class T> T absLimits(T value, T limit, bool upperLimit = true);

template<class T> T arSum(T arr[], uint8_t size);
template<class T> T minMax(const T array[], uint8_t size, bool max);
template<class T> void copyArray(T source[], T target[], uint8_t size);
template<class T> void printArray(T arr[], uint8_t size);
template<class T> int16_t indexIn(T array[], uint8_t size, const T& sample, uint8_t skip = 0);
template<class T> int16_t indexInLast(T array[], uint8_t size, const T& sample, uint8_t skip = 0);
template<class T> int16_t indexOfcondition(T array[], uint8_t size, T sample, bool (*condition)(const T& sample, const T& element), uint8_t skip = 0);

template<class T> void valuesMergeSort(T array[], const uint8_t size, bool ascending = true);
template<class T, class V> void sortBy(T property[], V objects[], const uint8_t size, bool ascending);

template<class T> bool inMargin(const T& value, const T& target, const T& epsilon);
uint8_t arcQuarter(int8_t x, int8_t y);

template<class T> void swap(T& a, T& b);
template<class T> bool sort(T& a, T& b, bool ascending);

template<class T> T& getRef(void* ptr);


template<class T> class IIterator {
protected:
  T* ptr;
public:
  virtual IIterator& operator+=(int8_t other) = 0;
  virtual IIterator& operator-=(int8_t other) = 0;
  virtual bool isEnd() const = 0;
  virtual bool isLoop() = 0;

  T& operator++();
  T& operator--();
  virtual T& operator*();
  virtual const T& operator*() const;
};

template<class T> class HalfDevision {  //for effective halve division method
  T step;
  T value;
  T margin;
public:
  bool isExpired();
  HalfDevision(T margin_, T value_);
  HalfDevision(T margin_, T value_, T step_);
  void setValue(T value_);
  void setStep(T step_);
  T operator*();
  T operator--();
  T operator++();
private:
  void decStep();
};

class ICollection {
public:
  virtual uint8_t getSize() const = 0;  
};

template<class T> class StackIterator;  //forward declaration
template<class T> class Stack : public ICollection {
protected:
  class StackNode {
  public:
    T value;
    StackNode* prev = NULL;
    StackNode(StackNode* previous, const T& value_) {
      value = value_;
      prev = previous;
    }
    T pop() {
      T temp = value;
      delete this;
      return temp;
    }
  };
  StackNode* head = NULL;
  StackNode* tail = NULL;
  uint8_t size = 0;
public:
  virtual ~Stack() {}
  virtual Stack<T>* Clone() const = 0;
  virtual void clear();
  virtual T& peek() const;
  virtual T& peek(uint8_t deeper) const;
  virtual T pop();
  virtual void push(T value);
  uint8_t getSize() const override;
  void reverse();
  friend class StackIterator<T>;
};

template<class T> class ValueStack : public Stack<T> {
public:
  ValueStack() {}
  Stack<T>* Clone() const override;
};

template<class T> class PointerStack : public Stack<T*> {
protected:
  bool autoDelete = true;
public:
  void setAutodelete(bool value);
  PointerStack(bool autoDel = true);
  ~PointerStack();
  Stack<T>* Clone() const override;
  void clear() override;
};

template<class T> class CollectionIterator : public IIterator<T> {  //iterator over collection
protected:
  ICollection* collection = NULL;
  bool looped = false;
  uint8_t index = 0;
public:
  uint8_t getSize() const;
  T& operator[](uint8_t other);  //set and return
  CollectionIterator& operator+=(int8_t other) override;
  CollectionIterator& operator-=(int8_t other) override;
  bool isEnd() const override;
  bool isLoop() override;
protected:
  void checkLoop(int8_t add);
  virtual void updatePtr() = 0;
};
template<class T> class UnorderedCollectionIterator : public CollectionIterator<T> {  //collection elements don't allow fast
protected:
  T** ptrs = NULL;  //for every element
  void updatePtr() override;
public:
  T& operator[](uint8_t other);  //set and return
  ~UnorderedCollectionIterator();
  virtual void updatePtrs() = 0;
};

template<class T> class StackIterator : public UnorderedCollectionIterator<T> {
public:
  StackIterator(const Stack<T>& ref);
  void updatePtrs() override;
};

template<class T> class ArrayIterator;
template<class T> class Array : public ICollection {
protected:
  uint8_t size = 0;
  T* values = NULL;
public:
  uint8_t getSize() const override;
  ~Array();
  Array() {}
  Array(const uint8_t size);
  Array(const uint8_t size, T* values);
  T& operator[](uint8_t index);
  const T& operator[](uint8_t index) const;
  T& last();
  friend class ArrayIterator<T>;
};
template<class T> class PointerArray : public Array<T*> {
public:
  PointerArray()
    : Array<T*>() {}
  PointerArray(const uint8_t size)
    : Array<T*>(size) {
    for (uint8_t i = 0; i < size; i++) this->values[i] = NULL;
  }
  T* take(uint8_t i);
  ~PointerArray();
};
template<class T> class ArrayIterator : public CollectionIterator<T> {
public:
  ArrayIterator(const Array<T>& array);
protected:
  void updatePtr() override;
};
//=================================================================================================================================
//===================realization===================================================================================================
//=================================================================================================================================
//stack
template<class T> T& getRef(void* ptr) {
  return *((T*)ptr);
}
template<class T> ArrayIterator<T>::ArrayIterator(const Array<T>& array) {
  this->collection = &array;
}
template<class T> void ArrayIterator<T>::updatePtr() {
  this->ptr = &(((Array<T>*)(this->collection))->values[this->index]);
}
template<class T> T& Array<T>::last() {
  return operator[](size - 1);
}
template<class T> Array<T>::Array(const uint8_t size) {
  this->size = size;
  values = new T[size];
}
template<class T> Array<T>::Array(const uint8_t size, T* values) {
  this->size = size;
  this->values = values;
}
template<class T> Array<T>::~Array() {
  delete[] values;
}
template<class T> T& Array<T>::operator[](uint8_t index) {
  return values[index];
}
template<class T> const T& Array<T>::operator[](uint8_t index) const {
  return values[index];
}
template<class T> uint8_t Array<T>::getSize() const {
  return size;
}
template<class T> T* PointerArray<T>::take(uint8_t i) {
  T*& element = this->values[i];
  T* temp = element;
  element = NULL;
  return temp;
}
template<class T> PointerArray<T>::~PointerArray() {
  for (uint8_t i = 0; i < this->size; ++i) {
    T*& element = this->values[i];
    if (element != NULL) delete element;
  }
}
template<class T> void Stack<T>::clear() {
  while (size > 0) pop();
}
template<class T> T& Stack<T>::peek() const {
  if (size) return head->value;
  poutN("Stack is empty can't peek");
}
template<class T> T& Stack<T>::peek(uint8_t deeper) const {
  if (size) {
    if (!deeper)
      return head->value;
    StackNode* temp = head;
    for (uint8_t i = 0; i < deeper; ++i)
      temp = temp->prev;
    return temp->value;
  }
  poutN(F("Stack is empty can't peek"));
}
template<class T> T Stack<T>::pop() {
  if (size) {
    StackNode* temp = head;
    head = head->prev;
    size--;
    if (!size) tail = NULL;
    return temp->pop();
  }
  poutN(F("Stack is empty can't pop"));
}
template<class T> void Stack<T>::push(T value) {
  if (size < 255) {
    head = new StackNode(head, value);
    if (!size) tail = head;
    size++;
  } else poutN(F("Stack is full 255 can't push"));
}
template<class T> uint8_t Stack<T>::getSize() const {
  return size;
}
template<class T> void Stack<T>::reverse() {
  StackNode* leading = head;
  StackNode* following = NULL;
  head = tail;
  tail = leading;
  while (leading != NULL) {
    StackNode* exLeading = leading;
    leading = exLeading->prev;
    exLeading->prev = following;
    following = exLeading;
  }
  head = following;
}
//valueStack
template<class T> Stack<T>* ValueStack<T>::Clone() const {
  ValueStack<T>* ret = new ValueStack<T>;
  auto temp = this->head;
  for (uint8_t i = this->size; i > 0; i--) {
    ret->push(temp->value);
    temp = temp->prev;
  }
  ret->reverse();
  return ret;
}
//ptrStack
template<class T> void PointerStack<T>::setAutodelete(bool value) {
  autoDelete = value;
}
template<class T> PointerStack<T>::PointerStack(bool autoDel = true) {
  autoDelete = autoDel;
}
template<class T> PointerStack<T>::~PointerStack() {
  clear();
}
template<class T> Stack<T>* PointerStack<T>::Clone() const {
  PointerStack<T>* ret = new PointerStack<T>(autoDelete);
  auto temp = this->head;
  for (uint8_t i = this->size; i > 0; i--) {
    if (autoDelete) ret->push(new T(temp->value));
    else ret->push(temp->value);
    temp = temp->prev;
  }
  ret->reverse();
  return ret;
}
template<class T> void PointerStack<T>::clear() {
  while (this->size > 0) {
    T* temp = pop();
    if (autoDelete) delete temp;
  }
}
//Collection Iterator
template<class T> UnorderedCollectionIterator<T>::~UnorderedCollectionIterator() {
  delete[] ptrs;
}
template<class T> uint8_t CollectionIterator<T>::getSize() const {
  return collection->getSize();
}
template<class T> T& CollectionIterator<T>::operator[](uint8_t other) {
  uint8_t size = getSize();
  index = other < size ? other : other % size;
  updatePtr();
  return *(this->ptr);
}
template<class T> T& UnorderedCollectionIterator<T>::operator[](uint8_t other) {
  return CollectionIterator<T>::operator[](other);
}
template<class T> CollectionIterator<T>& CollectionIterator<T>::operator+=(int8_t other) {
  checkLoop(other);
  index = Mod(getSize(), other + index);
  updatePtr();
  return *this;
}
template<class T> CollectionIterator<T>& CollectionIterator<T>::operator-=(int8_t other) {
  checkLoop(other);
  index = Mod(getSize(), -other + index);
  updatePtr();
  return *this;
}
template<class T> bool CollectionIterator<T>::isEnd() const {
  return looped;
}
template<class T> bool CollectionIterator<T>::isLoop() {
  if (looped) {
    looped = false;
    return true;
  }
  return false;
}
template<class T> void CollectionIterator<T>::checkLoop(int8_t add) {
  looped = ((add + index) >= getSize());
}
template<class T> void UnorderedCollectionIterator<T>::updatePtr() {
  this->ptr = ptrs[this->index];
}
//stack iterator
template<class T> StackIterator<T>::StackIterator(const Stack<T>& ref) {
  this->collection = &ref;
  updatePtrs();
  updatePtr();
}
template<class T> void StackIterator<T>::updatePtrs() {
  if (this->ptrs != NULL) delete[] this->ptrs;
  this->ptrs = new T*[getSize()];
  Stack<T>* tptr = ((Stack<T>*)(this->collection));
  auto temp = tptr->head;
  for (uint8_t i = getSize(); i > 0; i--) {
    this->ptrs[i - 1] = &temp->value;
    temp = temp->prev;
  }
}
//
template<class T> T minMax(T a, T b, bool max) {
  return (max == (a > b)) ? a : b;
}
template<class T> T assignSign(bool positive, T argument) {
  return (argument < 0) != positive ? argument : -argument;
}
template<class T> T flipSign(bool flip, const T& argument) {
  return flip ? -argument : argument;
}
template<class T> T limits(T value, T limit, bool upperLimit = true) {
  if ((value > limit) == upperLimit) {
    return limit;
  }
  return value;
}
template<class T> T doubleLimits(T value, T lLimit, T uLimit) {  //double limit lower and upper
  return limits<T>(limits<T>(value, lLimit, false), uLimit, true);
}
template<class T> T absLimits(T value, T limit, bool upperLimit = true) {
  if ((abs(value) > limit) == upperLimit) {
    return assignSign<T>(limit, value > 0);
  }
  return value;
}


template<class T> T arSum(T arr[], uint8_t size) {
  T sum = 0;
  for (uint8_t i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum;
}
template<class T> bool inMargin(const T& value, const T& target, const T& epsilon) {
  if (value > target) return (value - epsilon) < target;
  return (value + epsilon) >= target;
}
template<class T> void swap(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}
template<class T> bool sort(T& a, T& b, bool ascending) {
  bool swapped = ((a > b) == ascending);
  if (swapped) swap(a, b);
  return swapped;
}
template<class T> void copyArray(T source[], T target[], uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {
    target[i] = source[i];
  }
}
template<class T> void valuesMergeSort(T array[], const uint8_t size, bool ascending = true) {
#ifdef SortDebug
  pout(F("Sorting size "));
  poutN(size);
  printArray(array, size);
  pnl();
#endif
  if (size > 2) {
    uint8_t half = size / 2;
#ifdef SortDebug
    pout(F("\tHalv="));
    poutN(half);
#endif
    valuesMergeSort(array, half, ascending);         //sort first half
    valuesMergeSort(&array[half], half, ascending);  //sort second half without odd last
    T aCopy[size];                                   //copy for merging
    copyArray(array, aCopy, size);

#ifdef SortDebug
    pout(F("Sorted halves: "));
    printArray(array, half * 2);
    pnl();
    if (size % 2) {
      pout(F("\t+ odd "));
      printArray(&array[size - 1], 1);
      pnl();
    }
#endif
    T* v1 = aCopy;
    T* v2 = v1 + half;  //sizeof(T)
    T* l1 = v2;
    T* l2 = l1 + half;  //sizeof(T)
    T* a = array;
    T* al = a + 2 * half;
    for (; a < al; a++) {  //merge
#ifdef SortDebug
      pout(F("Comparing "));
      pout(*v1);
      pout(F(" and "));
      poutN(*v2);
#endif
      if ((*v1 < *v2) == ascending) {
        *a = *v1;
#ifdef SortDebug
        pout(F("\tAdded "));
        poutN(*a);
#endif
        v1++;
        if (v1 >= l1) {
#ifdef SortDebug
          poutN(F("\tFirst halve ended "));
#endif
          a++;
          for (; a < al; a++) {
            *a = *v2;
            v2++;
#ifdef SortDebug
            pout(F("\tAdded "));
            poutN(*a);
#endif
          }
        }
      } else {
        *a = *v2;
#ifdef SortDebug
        pout(F("\tAdded "));
        poutN(*a);
#endif
        v2++;
        if (v2 >= l2) {
#ifdef SortDebug
          poutN(F("\tSecond halve ended "));
#endif
          a++;
          for (; a < al; a++) {
            *a = *v1;
            v1++;
#ifdef SortDebug
            pout(F("\tAdding "));
            poutN(*a);
#endif
          }
        }
      }
    }
#ifdef SortDebug
    pout(F("After merging "));
    printArray(array, half * 2);
    pnl();
#endif
    if (size % 2) {  //deal with odd
      const T& last = array[size - 1];
      uint8_t i;
      for (i = 0; i < size; i++) {
        if ((last <= array[i]) == ascending) {  //if time to insert
          copyArray(&array[i], &aCopy[i + 1], size - i - 1);
          array[i] = last;  //do,break
          i++;
          break;
        }
      }
      for (; i < size; i++) {  //continue with aCopy behind
        array[i] = aCopy[i];
      }
    }
    return;
  }  //s>2
  if (size == 2) {
    sort(array[0], array[1], ascending);
#ifdef SortDebug
    pout(F("\tSorted:\n\t"));
    printArray(array, size);
    pnl();
#endif
  }
}
template<class T> int16_t indexIn(T array[], uint8_t size, const T& sample, uint8_t skip = 0) {
  for (int16_t i = 0; i < size; i++) {
    if (sample == array[i]) {
      if (skip) {
        skip--;
        continue;
      }
      return i;
    }
  }
  return -1;
}
template<class T> int16_t indexInLast(T array[], uint8_t size, const T& sample, uint8_t skip = 0) {
  int16_t i = size - 1;
  for (; i > -1; i--) {
    if (sample == array[i]) {
      if (skip) {
        skip--;
        continue;
      }
      break;
    }
  }
  return i;
}
template<class T> int16_t indexOfcondition(T array[], uint8_t size, T sample, bool (*condition)(const T& sample, const T& element), uint8_t skip = 0) {
  for (uint8_t i = 0; i < size; i++) {
    if (condition(sample, array[i])) {
      if (skip) {
        skip--;
        continue;
      }
      return i;
    }
  }
  return -1;
}
template<class T> T minMax(const T array[], uint8_t size, bool max) {
  T minmax = *array;
  for (uint8_t i = 1; i < size; i++) {
    T& ref = array[i];
    if ((ref > minmax) == max) minmax = ref;
  }
  return minmax;
}
template<class T, class V> void sortBy(T property[], V objects[], const uint8_t size, bool ascending) {
  uint8_t repeatMap[size / 8 + bool(size % 8)] = { 0 };
  T sortedProperties[size];
  copyArray(property, sortedProperties, size);
  valuesMergeSort(sortedProperties, size);

  V objectsCopy[size];
  copyArray(objects, objectsCopy, size);
  for (uint8_t i = 0; i < size; i++) {
    uint8_t indexInSorted;
    uint8_t skip = 0;
    uint8_t ti;
    uint8_t tm;
    for (;; skip++) {
      indexInSorted = indexIn(property, size, sortedProperties[i], skip);
      ti = indexInSorted / 8;
      tm = 1 << (indexInSorted % 8);
      if (!(repeatMap[ti] & tm)) break;
    }
    // pout(indexInSorted);
    objects[i] = objectsCopy[indexInSorted];
    repeatMap[ti] |= tm;
  }
  copyArray(sortedProperties, property, size);
}

template<class T> T& IIterator<T>::operator++() {
  operator+=(1);
  return operator*();
}
template<class T> T& IIterator<T>::operator--() {
  operator-=(1);
  return operator*();
}
template<class T> T& IIterator<T>::operator*() {
  return *ptr;
}
template<class T> const T& IIterator<T>::operator*() const {
  return *ptr;
}
template<class T> bool HalfDevision<T>::isExpired() {
  return step <= (margin / 2);
}
template<class T> HalfDevision<T>::HalfDevision(T margin_, T value_) {  //from max
  margin = margin_;
  value = value_;
  step = value / 2;
}
template<class T> HalfDevision<T>::HalfDevision(T margin_, T value_, T step_) {
  margin = margin_;
  value = value_;
  step = step_;
}
template<class T> void HalfDevision<T>::setValue(T value_) {
  value = value_;
}
template<class T> void HalfDevision<T>::setStep(T step_) {
  step = step_;
}
template<class T> T HalfDevision<T>::operator*() {
  return value;
}
template<class T> T HalfDevision<T>::operator--() {
  value -= step;
  decStep();
  return value;
}
template<class T> T HalfDevision<T>::operator++() {
  value += step;
  decStep();
  return value;
}
template<class T> void HalfDevision<T>::decStep() {
  step /= 2;
}

template<class T> T& larger(T& a, T& b, bool invert = false) {
  if ((a > b) != invert) return a;
  return b;
}

template<class T> class Lerp1 {
protected:
  T a;
  T b;
public:
  Lerp1() {}
  Lerp1(const T& a, const T& b) {
    this->a = a;
    this->b = b;
  }
  T getVal(float t) const {
    t = doubleLimits<float>(t, 0, 1);
    return a * (1 - t) + t * b;
  }
  void setA(const T& a) {
    this->a = a;
  }
  void setB(const T& b) {
    this->b = b;
  }
};