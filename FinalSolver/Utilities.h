#pragma once
#include <stdint.h>
#include "IO.h"

//frequently used funcs

// #define SortDebug
void spn();

bool& f(bool& other);
String boolStr(bool value);
bool fromString(const String& boolString);

int8_t Mod(int8_t period, int8_t argument);
int8_t Mod3(int8_t argument);
int8_t Mod4(int8_t argument);
int8_t Mod8(int8_t argument);

char digitOf(uint8_t i);
uint8_t valueOf(char c);

int8_t cycleDistanceVector(int8_t position, int8_t target, uint8_t cycleLength);
template<class T> T mirrorValueOnRange(T from, T value, T until);

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
template<class T> void copyArray(T source[], T target[], uint16_t size);
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
template<class T> const T& getConstRef(const void* ptr);

//move semantics
template<class T>
struct _Remove_reference {  // remove reference
  typedef T _noRef;
};
template<class T>
struct _Remove_reference<T&> {  // remove reference
  typedef T _noRef;
};
template<class T>
struct _Remove_reference<T&&> {  // remove rvalue reference
  typedef T _noRef;
};
template<class T>
inline typename _Remove_reference<T>::_noRef&&
move(T&& _Arg) noexcept {  //forwarding reference
  return static_cast<typename _Remove_reference<T>::_noRef&&>(_Arg);
}
template<class T> inline T&& forward(typename _Remove_reference<T>::_noRef& _Arg) noexcept {
  return static_cast<T&&>(_Arg);
}
template< class T >
constexpr const T& as_const(T& t) noexcept {
  return t;
}
///move semantics

//collections
template<class T> class KPtr {  //unique pointer
protected:
  T* ptr = NULL;
public:
  KPtr()
    : ptr(NULL) {}
  KPtr(T* ptr) {
    this->ptr = ptr;
  }
  KPtr(KPtr&& other) {
    swap(this->prt, other.ptr);
    if (this->ptr == ptr)
      other.prt = NULL;
  }
  ~KPtr() {
    delete ptr;
  }
  KPtr& operator=(KPtr&& other) {
    swap(this->ptr, other.ptr);
    return *this;
  }
  T* operator->() const {
    return ptr;
  }
  T* release() {
    T* temp = ptr;
    ptr = NULL;
    return temp;
  }
  T& operator*() const {
    return *ptr;
  }
};
//Collections
class ICollection {  //collection contains number of items
public:
  virtual uint16_t getSize() const = 0;  //collection always have number of items
  virtual ~ICollection() {}
};
template<class T> class IndexedCollection : public ICollection {  //indexed collection allow access to elements by index
public:
  virtual T& operator[](uint16_t index) = 0;
  virtual const T& operator[](uint16_t index) const = 0;
};
template<class T> class IIterator {  //iterator can reach end and be dereferenced
public:
  virtual bool notEnd() const = 0;

  virtual T& operator*() = 0;
  virtual const T& operator*() const = 0;
};
template<class T> class ForwardIterator : public IIterator<T> {  //foreach element of collection
protected:
  uint16_t counter = 0;
public:
  uint16_t getCounter() const {
    return counter;
  }
  void skip(uint16_t items) {
    while (items) {
      this->operator++();
      --items;
    }
  }
  virtual ForwardIterator& operator++() = 0;
};
template<class T> class BiderectionalIterator : public ForwardIterator<T>, public IndexedCollection<T> {
protected:
  bool backwards = false;

  virtual void evaluate() {}
public:
  virtual bool notEnd() const override;
  void setIndex(uint16_t index);
  BiderectionalIterator& reverse();

  BiderectionalIterator& operator+=(int16_t delta);
  virtual ForwardIterator<T>& operator++() override;
  BiderectionalIterator& operator--();
  BiderectionalIterator& begin(bool backwards = false);

  virtual T& operator[](uint16_t index) override;
  virtual const T& operator[](uint16_t index) const override;
};
template<class F> struct Func;  //single Arg function template declaration
template<class R, class... Args> struct Func<R(Args...)> {
  using t = R (*)(Args...);
};

template<class T> class FunctionIterator : public BiderectionalIterator<T> {
protected:
  T result;
  uint16_t limit = 0;
  int16_t shift = 0;
  uint16_t getIndex() const {
    return Mod(limit, this->counter + shift);
  }
  void setResult(T&& result) {
    this->result = move(result);
  }
  virtual void evaluate() = 0;  //result=move(F(???))
public:
  FunctionIterator(uint8_t limit, int8_t shift)
    : limit(limit), shift(shift) {}
  virtual uint16_t getSize() const override;

  virtual T& operator*() override;
  virtual const T& operator*() const override;
};
template<class T> class SimpleFunctionIterator : public FunctionIterator<T> {
protected:
  T(*func)
  (uint8_t) = NULL;
  virtual void evaluate() override {
    setResult((*func)(this->getIndex()));
  }
public:
  SimpleFunctionIterator(SimpleFunctionIterator&& other) = default;
  SimpleFunctionIterator(T (*func)(uint8_t), uint8_t limit, int8_t shift)
    : FunctionIterator<T>(limit, shift), func(func) {}
};
template<class T> class InternalFunctionIterator : public FunctionIterator<T> {
protected:
  virtual T internalValueProvider(uint16_t index) = 0;
  virtual void evaluate() override {
    setResult(internalValueProvider(this->getIndex()));
  }
public:
  InternalFunctionIterator(InternalFunctionIterator&& other) = default;
  InternalFunctionIterator(uint8_t limit, int8_t shift = 0)
    : FunctionIterator<T>(limit, shift) {}
};
template<class T> class RandomAccessIterator : public BiderectionalIterator<T> {  //fast access by index
protected:
  T* pointer = NULL;
public:
  virtual T& operator*() override;
  virtual const T& operator*() const override;
};
template<class T> struct ArrayHandle : public ICollection {  //Control dynamic array<T>
  T* array = NULL;
  T* end = NULL;

  ArrayHandle() {}
  ArrayHandle(T* array, T* end)
    : array(array), end(end) {}
  ArrayHandle(uint16_t size);
  ArrayHandle(T array[], uint16_t size);
  ArrayHandle(const ArrayHandle& other);
  ArrayHandle(ArrayHandle&& other);
  ArrayHandle& operator=(const ArrayHandle& other);
  ArrayHandle& operator=(ArrayHandle&& other);  ///5

  virtual uint16_t getSize() const override;

  void destroy() {
    delete[] array;
    array = NULL;
    end = NULL;
  }
};
template<class T> class Array : IndexedCollection<T> {  //simple array
  ArrayHandle<T> handle;
public:
  Array() {}
  Array(T array[], uint16_t size)
    : handle(array, size) {}
  Array(uint16_t size)
    : handle(uint16_t(size)) {}
  ~Array() {
    handle.destroy();
  }  //5
  Array(const Array& other)
    : handle(as_const(other.handle)) {}
  Array(Array&& other)
    : handle(move(other.handle)) {}
  Array& operator=(const Array& other) = default;
  Array& operator=(Array&& other) = default;  ///5


  virtual uint16_t getSize() const override {
    return handle.getSize();
  }
  T& last(uint16_t skip = 0) {
    return *(this->handle.end - 1 - skip);
  }

  const T& last(uint16_t skip = 0) const {
    return *(this->handle.end - 1 - skip);
  }

  virtual T& operator[](uint16_t index) {
    return this->handle.array[index];
  }
  virtual const T& operator[](uint16_t index) const {
    return this->handle.array[index];
  }
  class ArrayIterator : public RandomAccessIterator<T> {
  protected:
    ArrayHandle<T> handle;
    uint16_t size = 0;

    ArrayIterator(T* array, T* end, bool reverse)
      : handle(array, end) {
      size = handle.getSize();
      if (reverse) this->reverse();
      setIndex(0);
    }
    virtual void evaluate() override {
      this->pointer = handle.array + this->counter;
    }
  public:
    ArrayIterator(ArrayIterator&& other) = default;
    virtual uint16_t getSize() const override {
      return size;
    }
    friend class Array<T>;
  };
  ArrayIterator iterator(uint16_t skip = 0, uint16_t keep = 0, bool reverse = false);
};
///ArrayIterator
template<class T> class CollectionRandomAccessIterator : public RandomAccessIterator<T> {
protected:
  Array<T*> pointers;
  virtual void evaluate() override {
    this->pointer = pointers[this->counter];
  }
public:
  CollectionRandomAccessIterator(Array<T*>&& pointers)
    : pointers(move(pointers)) {
    begin();
  }
  virtual uint16_t getSize() const override {
    return pointers.getSize();
  }
};
//Stack
template<class T>
class Stack : public IndexedCollection<T> {
protected:
  class StackNode {
  public:
    T value;
    StackNode* prev = NULL;
    template<class U> StackNode(StackNode* previous, U&& value) {  //universal reference
      this->value = forward<U>(value);
      prev = previous;
    }
    StackNode(StackNode&& other) {
      swap(value, other.value);
    }
    T pop() {
      T temp = move(value);
      delete this;
      return temp;
    }
  };
  StackNode* head = NULL;  //head<- push
  StackNode* tail = NULL;  //tail<- pushback
  uint16_t size = 0;

  StackNode* nodeByIndex(uint16_t index) const;
  StackNode* unChainedNode(uint16_t index);
  template<class U> void insertNode(uint16_t index, U&& value);
  StackNode*& pointerToNodeByIndex(uint16_t index);

  Stack(StackNode* head, StackNode* tail, uint16_t size)
    : head(head), tail(tail), size(size) {}
public:
  Stack() {}
  ~Stack();  //5
  Stack(const Stack& other);
  Stack(Stack&& other);
  Stack& operator=(const Stack& other);
  Stack& operator=(Stack&& other);  ///5

  virtual uint16_t getSize() const override;
  void clear();

  const T& peek() const;
  const T& peekBack() const;

  T& item(uint8_t index = 0);
  T& itemBack(uint8_t index = 0);

  virtual T& operator[](uint16_t index) override;
  virtual const T& operator[](uint16_t index) const override;

  T pop();  //normal pop
  T popBack();
  T popFromFront(uint16_t skip);
  T popFromBack(uint16_t skip);

  template<class U> void push(U&& value);
  template<class U> void pushBack(U&& value);
  template<class U> void pushFromFront(U&& value, uint16_t skip);
  template<class U> void pushFromBack(U&& value, uint16_t skip);

  void swapItems(uint16_t a, uint16_t b, bool fromTop);  //swap by index
  void reverse();

  Stack<T> cutLast(uint16_t items);
  Stack<T> cut(uint16_t from, uint16_t until);
  Stack<T>& append(Stack<T>&& other);
  Stack<T>& insert(Stack<T>&& other, uint16_t at);

  class StackIteratorForward : public ForwardIterator<T> {
  protected:
    StackNode* currentNode;
  public:
    StackIteratorForward(StackIteratorForward&& other)
      : currentNode(other.currentNode) {}
    StackIteratorForward(StackNode* start);

    virtual bool notEnd() const override;

    virtual T& operator*() override;
    virtual const T& operator*() const override;
    virtual ForwardIterator<T>& operator++() override;

    friend class Stack<T>;
  };  ///StackIteratorForward
  class StackIteratorRandom : public CollectionRandomAccessIterator<T> {
  protected:
    StackIteratorRandom(Array<T*>&& pointers)
      : CollectionRandomAccessIterator<T>(move(pointers)) {}
    friend class Stack<T>;
  };
  StackIteratorForward iteratorForward() const;
  StackIteratorRandom iteratorRandom(bool backwards = false) const;
protected:
  Array<T*> arrayForRandomAccess() const;
};  ///Stack


template<class T>
class HalfDevision {  //for effective halve division method
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


//=================================================================================================================================
//===================implementation===================================================================================================
//=================================================================================================================================
//refs
template<class T> T& getRef(void* ptr) {
  return *((T*)ptr);
}
template<class T> const T& getConstRef(const void* ptr) {
  return *((T*)ptr);
}
//BidirectionalIterator
template<class T> void BiderectionalIterator<T>::setIndex(uint16_t index) {
  this->counter = backwards ? mirrorValueOnRange<uint16_t>(0, index, this->getSize()) : index;
  evaluate();
}
template<class T> bool BiderectionalIterator<T>::notEnd() const {
  return this->counter < this->getSize();
}
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::operator+=(int16_t delta) {
  if (backwards) delta = -delta;
  this->counter += delta;
  evaluate();
  return *this;
}
template<class T> ForwardIterator<T>& BiderectionalIterator<T>::operator++() {
  return operator+=(1);
};
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::begin(bool backwards) {
  this->backwards = backwards;
  setIndex(0);
  return *this;
}
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::operator--() {
  return operator+=(-1);
};
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::reverse() {
  backwards = !backwards;
  return *this;
}
template<class T> T& BiderectionalIterator<T>::operator[](uint16_t index) {
  this->setIndex(index);
  return this->operator*();
}
template<class T> const T& BiderectionalIterator<T>::operator[](uint16_t index) const {
  this->setIndex(index);
  return this->operator*();
}
///BidirectionalIterator
//FuncIterator
template<class T> uint16_t FunctionIterator<T>::getSize() const {
  return limit;
}
template<class T> T& FunctionIterator<T>::operator*() {
  return result;
}
template<class T> const T& FunctionIterator<T>::operator*() const {
  return result;
}
///FuncIterator
//RandomAccessIterator
template<class T> T& RandomAccessIterator<T>::operator*() {
  return *pointer;
}
template<class T> const T& RandomAccessIterator<T>::operator*() const {
  return *pointer;
}
///RandomAccessIterator
//ArrayHandle<T>
template<class T> ArrayHandle<T>::ArrayHandle(T array[], uint16_t size) {  //from existing dynamic array
  this->array = array;
  this->end = array + size;
}
template<class T> ArrayHandle<T>::ArrayHandle(uint16_t size)
  : ArrayHandle(new T[size], size) {}
template<class T> ArrayHandle<T>::ArrayHandle(const ArrayHandle& other) {
  *this = other;
}
template<class T> ArrayHandle<T>::ArrayHandle(ArrayHandle&& other) {
  *this = move(other);
}
template<class T> ArrayHandle<T>& ArrayHandle<T>::operator=(const ArrayHandle& other) {  //copy assignment
  if (&other != this) {
    delete[] array;
    const uint16_t size = other.getSize();
    this->array = new T[size];
    copyArray(other.array, this->array, size);
  }
  return *this;
}
template<class T> ArrayHandle<T>& ArrayHandle<T>::operator=(ArrayHandle&& other) {  //move assignment
  swap(this->array, other.array);
  swap(this->end, other.end);
  return *this;
}
template<class T> uint16_t ArrayHandle<T>::getSize() const {
  return (this->array < this->end) ? this->end - this->array : 0;
}
///ArrayHandle<T>
//Array<T>
template<class T> typename Array<T>::ArrayIterator Array<T>::iterator(uint16_t skip, uint16_t keep, bool reverse) {
  return ArrayIterator(this->handle.array + skip, this->handle.end - keep, reverse);
}
///Array<T>
//Stack<T>
template<class T> typename Stack<T>::StackNode* Stack<T>::nodeByIndex(uint16_t index) const {
  if (index >= size) return NULL;
  if (!index) return tail;
  index = size - 1 - index;  //how much skip
  StackNode* temp = head;
  while (index) {  //while skip
    temp = temp->prev;
    --index;
  }
  return temp;
}
template<class T> typename Stack<T>::StackNode* Stack<T>::unChainedNode(uint16_t index) {
  StackNode* after = nodeByIndex(index + 1);
  StackNode* selected = after->prev;
  after->prev = selected->prev;
  --size;
  return selected;
}
template<class T> template<class U> void Stack<T>::insertNode(uint16_t index, U&& value) {
  StackNode* after = nodeByIndex(index);
  after->prev = new StackNode(after->prev, forward<U>(value));
  ++size;
}
template<class T> typename Stack<T>::StackNode*& Stack<T>::pointerToNodeByIndex(uint16_t index) {
  index %= size;
  if (index == (size - 1)) return head;
  return nodeByIndex(index + 1)->prev;
}
template<class T> Stack<T>::~Stack() {
  clear();
}
template<class T> Stack<T>::Stack(const Stack<T>& other) {
  *this = other;
}
template<class T> Stack<T>::Stack(Stack<T>&& other) {
  *this = move(other);
}
template<class T> Stack<T>& Stack<T>::operator=(const Stack<T>& other) {
  if (this != &other) {
    if (size) clear();
    for (auto stackIt = other.iteratorForward(); stackIt.notEnd(); ++stackIt)
      pushBack(*stackIt);
  }
}
template<class T> Stack<T>& Stack<T>::operator=(Stack<T>&& other) {
  swap(this->tail, other.tail);
  swap(this->head, other.head);
  swap(this->size, other.size);
}
template<class T> uint16_t Stack<T>::getSize() const {
  return size;
}
template<class T> void Stack<T>::clear() {
  while (size) pop();
}
template<class T> const T& Stack<T>::peek() const {
  if (size) return head->value;
  //poutN(F("Stack is empty can't peek"));
}
template<class T> const T& Stack<T>::peekBack() const {
  if (size) return tail->value;
  //poutN(F("Stack is empty can't peek"));
}
template<class T> T& Stack<T>::item(uint8_t index) {
  return nodeByIndex(this->size - index - 1)->value;
}
template<class T> T& Stack<T>::itemBack(uint8_t index) {
  return nodeByIndex(index)->value;
}
template<class T> T& Stack<T>::operator[](uint16_t index) {
  return nodeByIndex(index)->value;
}
template<class T> const T& Stack<T>::operator[](uint16_t index) const {
  return nodeByIndex(index)->value;
}
template<class T> T Stack<T>::pop() {
  if (size) {
    StackNode* temp = head;
    head = head->prev;
    --size;
    if (!size) tail = NULL;
    return temp->pop();
  }
  poutN(F("Stack is empty can't pop"));
}
template<class T> T Stack<T>::popBack() {
  if (size) {
    StackNode* temp = tail;
    tail = nodeByIndex(1);
    --size;
    if (size)
      tail->prev = NULL;
    else
      head = NULL;
    return temp->pop();
  }
  poutN(F("Stack is empty can't pop"));
}
template<class T> T Stack<T>::popFromFront(uint16_t skip) {
  skip %= size;
  const uint16_t maxIndex = size - 1;
  if (!skip) return pop();
  if (skip == maxIndex) return popBack();
  if (size) {
    return unChainedNode(maxIndex - skip)->pop();
  }
  //poutN(F("Stack is empty can't pop"));
}
template<class T> T Stack<T>::popFromBack(uint16_t skip) {
  skip %= size;
  if (!skip) return popBack();
  if (skip == size - 1) return pop();
  if (size) {
    return unChainedNode(skip)->pop();
  }
  //poutN(F("Stack is empty can't pop"));
}
template<class T> template<class U> void Stack<T>::push(U&& value) {
  head = new StackNode(head, forward<U>(value));
  if (!size) tail = head;
  ++size;
}
template<class T> template<class U> void Stack<T>::pushBack(U&& value) {
  if (!size) return push(forward<U>(value));
  StackNode* temp = new StackNode(NULL, forward<U>(value));
  tail->prev = temp;
  tail = temp;
  ++size;
}
template<class T> template<class U> void Stack<T>::pushFromFront(U&& value, uint16_t skip) {
  skip %= (size + 1);
  if (!skip) return push(forward<U>(value));
  if (skip == size) return pushBack(forward<U>(value));
  if (size) return insertNode(size - skip, forward<U>(value));
}
template<class T> template<class U> void Stack<T>::pushFromBack(U&& value, uint16_t skip) {
  skip %= (size + 1);
  if (!skip) return pushBack(forward<U>(value));
  if (skip == size) return push(forward<U>(value));
  if (size) return insertNode(skip, forward<U>(value));
}
template<class T> void Stack<T>::swapItems(uint16_t a, uint16_t b, bool fromTop) {
  if (a == b) return;
  if (size < 2) return;
  a %= size;
  b %= size;
  if (fromTop) {
    a = mirrorValueOnRange<uint16_t>(0, a, size);
    b = mirrorValueOnRange<uint16_t>(0, b, size);
  }
  //a, b -different indexes
  auto aP = pointerToNodeByIndex(a);
  auto bP = pointerToNodeByIndex(b);
  if (!a) tail = bP;         //if a is the first point tail to b
  else if (!b) tail = aP;    //if b is the first point tail to a
  swap(aP, bP);              //swap forward pointers to nodes
  swap(aP->prev, bP->prev);  //swap prev pointers
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

template<class T> Stack<T> Stack<T>::cutLast(uint16_t items) {
  items %= size + 1;
  if (!items) return Stack();
  if (items == size) return Stack(move(*this));
  StackNode* tempHead = head;                       //last node pf span always head
  StackNode* tempTail = nodeByIndex(size - items);  //first node of span
  head = tempTail->prev;                            //new head`
  size -= items;
  if (!size) tail = NULL;
  return Stack(tempHead, tempTail, items);
}
template<class T> Stack<T> Stack<T>::cut(uint16_t from, uint16_t until) {
  if (from >= size) return Stack();  //empty
  if (until >= size) return cutLast(size - from);
  auto tempHeadP = pointerToNodeByIndex(until - 1);  //pointer& to last node of span
  StackNode* tempHead = tempHeadP;                   //assign value of pointer to tempHead
  StackNode* tempTail = nodeByIndex(from);           //first node of span
  tempHeadP = tempTail->prev;                        //collapse this stack chain
  tempTail->prev = NULL;                             //disconnect cutten span
  if (!from) tail = tempHeadP;                       //if from first item, tail tempTail->prev
  uint16_t items = until - from;
  size -= items;
  return Stack(tempHead, tempTail, items);  //return Stack with cut chain
}
template<class T> Stack<T>& Stack<T>::append(Stack<T>&& other) {
  if (!other.size) goto ret;
  if (!size) {
    *this = move(other);
    goto ret;
  }
  other.tail->prev = head;  //conect head to other tail
  head = other.head;        //take head;
  size += other.size;       //add size
  other.head = NULL;
  other.tail = NULL;
  other.size = 0;
ret:
  return *this;
}
template<class T> Stack<T>& Stack<T>::insert(Stack<T>&& other, uint16_t at) {
  if (!other.size) goto ret;
  if (!size) *this = move(other);
  if (at >= size) append(move(other));
  StackNode* afterNode = nodeByIndex(at);
  other.tail->prev = afterNode.prev;  //connect other chain's tail
  afterNode.prev = other.head;        //connect other chain's head
  if (!at) tail = other.tail;         //reconnect tail if needed
  size += other.size;                 //add size
  other.tail = NULL;                  //empty
  other.head = NULL;
  other.size = 0;
ret:
  return *this;
}
//Stack<T>::StackIteratorForward
template<class T> Stack<T>::StackIteratorForward::StackIteratorForward(StackNode* start) {
  currentNode = start;
}
template<class T> bool Stack<T>::StackIteratorForward::notEnd() const {
  return currentNode != NULL;
}
template<class T> T& Stack<T>::StackIteratorForward::operator*() {
  return currentNode->value;
}
template<class T> const T& Stack<T>::StackIteratorForward::operator*() const {
  return currentNode->value;
}
template<class T> ForwardIterator<T>& Stack<T>::StackIteratorForward::operator++() {
  if (notEnd()) {
    currentNode = currentNode->prev;
    ++(this->counter);
  }
  return *this;
}
template<class T> typename Stack<T>::StackIteratorForward Stack<T>::iteratorForward() const {
  return StackIteratorForward(this->head);
}
template<class T> typename Stack<T>::StackIteratorRandom Stack<T>::iteratorRandom(bool backwards) const {
  auto it = StackIteratorRandom(move(arrayForRandomAccess()));
  if (backwards) it.begin(true);
  return it;
}
template<class T> Array<T*> Stack<T>::arrayForRandomAccess() const {
  Array<T*> ptrs = Array<T*>(size);
  {
    auto pt = ptrs.iterator();
    for (auto it = iteratorForward(); it.notEnd(); ++it) {
      (*pt) = &(*it);
      ++pt;
    }
  }
  return ptrs;
}
///Stack<T>
///Collections
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
  T temp(move(a));
  a = move(b);
  b = move(temp);
}
template<class T> bool sort(T& a, T& b, bool ascending) {
  bool swapped = ((a > b) == ascending);
  if (swapped) swap(a, b);
  return swapped;
}
template<class T> void copyArray(T source[], T target[], uint16_t size) {
  T* limit = source + size;
  while (source < limit) {
    *target = *source;
    ++target;
    ++source;
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
template<class T> T mirrorValueOnRange(T from, T value, T until) {
  return until - value + from - 1;
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