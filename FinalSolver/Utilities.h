#pragma once
#include "Templates.h"
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
template<class T> T uLimit(const T& value, const T& limit);
template<class T> T uLimitStrict(T value, const T& limit);
template<class T> bool uLimited(T& value, const T& limit);
template<class T> bool uLimitedStrict(T& value, const T& limit);
template<class T> T limits(T value, T limit, bool upperLimit = true);
template<class T> T doubleLimits(T value, T lLimit, T uLimit);
template<class T> bool doubleLimited(T& value, const T& lLimit, const T& uLimit);
template<class T> bool doubleLimitedStrict(T& value, const T& lLimit, const T& uLimit);
template<class T> bool doubleLimitedLStrict(T& value, const T& lLimit, const T& uLimit);
template<class T> bool doubleLimitedRStrict(T& value, const T& lLimit, const T& uLimit);
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

template<class T> bool inSpan(const T& a, const T& value, const T& b);
template<class T> bool inSpanStrict(const T& a, const T& value, const T& b);
template<class T> bool inSpanLStrict(const T& a, const T& value, const T& b);
template<class T> bool inSpanRStrict(const T& a, const T& value, const T& b);
template<class T> bool inMargin(const T& value, const T& target, const T& epsilon);
uint8_t arcQuarter(int8_t x, int8_t y);

template<class T> void swap(T& a, T& b);
template<class T> bool sort(T& a, T& b, bool ascending);
template<class T> bool sort(T& a, T& b, bool ascending, bool (*compareFunc)(const T& a, const T& b));

template<class T> T& getRef(void* ptr);
template<class T> const T& getConstRef(const void* ptr);
template<class T> constexpr nulPtr() noexcept;

template<class T> void delNull(T*& ptr);
template<class T> void takeNull(T*& ptr);


//"Smart" "pointers"
template<class T> class SPtr {
protected:
  T* ptr = NULL;
public:
  SPtr() {}
  SPtr(T* ptr)
    : ptr(ptr) {}
  SPtr(const SPtr& other) {
    ptr = new T(*other.ptr);
  }
  SPtr(SPtr&& other) {
    swap(ptr, other.ptr);
  }
  SPtr& operator=(const SPtr& other) {
    ptr = new T(*other.ptr);
    return *this;
  }
  SPtr& operator=(SPtr&& other) {
    swap(ptr, other.ptr);
    return *this;
  }
  ~SPtr() {
    delete ptr;
  }
  T& operator*() {
    return *ptr;
  }
  T* operator->() {
    return ptr;
  }
  T* take() {
    return takeNull(ptr);
  }
  bool delPtr() {
    if (ptr == NULL)
      return false;
    delete ptr;
    ptr = NULL;
    return true;
  }
  bool operator==(T* other) const {
    return ptr == other;
  }
  bool operator!=(T* other) const {
    return ptr != other;
  }
};
///"Smart" "pointers"
//Collections
template<class T> class IIterator {  //iterator can reach end and be dereferenced
public:
  virtual bool notEnd() const = 0;

  virtual T& operator*() = 0;
  virtual const T& operator*() const = 0;
};
template<class T> class IForwardIterator : public IIterator<T> {
};
template<class T> class ForwardIterator : public IForwardIterator<T> {  //foreach element of collection
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

//Collections interfaces
template<class T> class ICollection {  //collection contains number of items
public:
  virtual uint16_t getSize() const = 0;  //collection always have number of items
  virtual ~ICollection() {}
};
template<class T> class OrderedRead {
public:
  virtual const T& itemAt(uint16_t index) const = 0;
  virtual const T& itemLast(uint16_t index) const = 0;
};
template<class T> class OrderedAccess {
public:
  virtual T& itemAt(uint16_t index) = 0;
};
template<class T> class OrderedWrite : public OrderedAccess<T> {
public:
  virtual T& itemLast(uint16_t index) = 0;
};
template<class T> class OrderedCollection : public OrderedRead<T>, public OrderedWrite<T>, public ICollection<T> {};

template<class T> class RandomRead {
public:
  virtual const T& operator[](uint16_t index) const = 0;
};
template<class T> class RandomWrite {
public:
  virtual T& operator[](uint16_t index) = 0;
};
template<class T> class RandomAccess : public RandomRead<T>, public RandomWrite<T> {};

template<class T> class RandomCollectionRead : public OrderedRead<T>, public RandomRead<T>, public ICollection<T> {
public:
  virtual const T& itemAt(uint16_t index) const override final {
    return this->operator[](index);
  }
  virtual const T& itemLast(uint16_t index) const override final {
    return this->operator[](this->getSize() - index - 1);
  }
};
template<class T> class RandomCollection : public RandomCollectionRead<T>, public RandomWrite<T>, public OrderedWrite<T> {
  virtual T& itemAt(uint16_t index) override final {
    return this->operator[](index);
  }
  virtual T& itemLast(uint16_t index) override final {
    return this->operator[](this->getSize() - index - 1);
  }
};

///Collections interfaces
template<class T> class BiderectionalIterator : public ForwardIterator<T> {
protected:
  bool backwards = false;

  virtual void evaluate() {}
public:
  BiderectionalIterator& reverse();

  BiderectionalIterator& operator+=(int16_t delta);
  virtual ForwardIterator<T>& operator++() override;
  BiderectionalIterator& operator--();

  bool goToItemNotEnd(uint16_t index);
  //virtual const T& operator[](uint16_t index) = 0;
};
template<class F> struct Func;  //single Arg function template declaration
template<class R, class... Args> struct Func<R(Args...)> {
  using t = R (*)(Args...);
};

template<class T> class FixedSizeIterator : public BiderectionalIterator<T> {
protected:
  uint16_t limit = 0;
public:
  FixedSizeIterator() {}
  FixedSizeIterator(uint16_t limit)
    : limit(limit) {}
  virtual bool notEnd() const override final {
    return this->counter < limit;
  }
  void setIndex(uint16_t index);
  uint16_t getSize() const {
    return limit;
  }
  FixedSizeIterator& begin(bool backwards = false);
};
template<class T> class RandomAccessIterator : public FixedSizeIterator<T>, public RandomAccess<T> {  //fast access by index
protected:
  using FixedSizeIterator<T>::FixedSizeIterator;
public:
  virtual T& operator[](uint16_t index) = 0;
  virtual const T& operator[](uint16_t index) const = 0;
};
template<class T> class FunctionIterator : public FixedSizeIterator<T> {
protected:
  T result;

  int16_t shift = 0;
  uint16_t getIndex() const {
    return Mod(this->limit, this->counter + shift);
  }
  void setResult(T&& result) {
    this->result = move(result);
  }
  virtual void evaluate() = 0;  //result=move(F(???))
public:
  FunctionIterator(uint8_t limit, int8_t shift)
    : FixedSizeIterator<T>(limit), shift(shift) {}

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

template<class T> struct ArrayHandle {  //Control dynamic array<T>
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

  uint16_t getSize() const;

  void destroy() {
    delete[] array;
    array = NULL;
    end = NULL;
  }
};
template<class T> class Array /*: RandomCollection<T>*/ {  //simple array
protected:
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
  Array& operator=(const Array& other) {
    handle = other.handle;
    return *this;
  };
  Array& operator=(Array&& other) {  ///5
    handle = move(other.handle);
    return *this;
  };

  uint16_t getSize() const {
    return handle.getSize();
  }
  T& last(uint16_t skip = 0) {
    return *(this->handle.end - 1 - skip);
  }

  const T& last(uint16_t skip = 0) const {
    return *(this->handle.end - 1 - skip);
  }

  T& operator[](uint16_t index) {
    return this->handle.array[index];
  }
  const T& operator[](uint16_t index) const {
    return this->handle.array[index];
  }
  class ArrayIterator : public RandomAccessIterator<T> {
  protected:
    ArrayHandle<T> handle;
    T* pointer;
    ArrayIterator(T* array, T* end, bool reverse)
      : handle(array, end) {
      this->limit = handle.getSize();
      if (reverse) this->reverse();
      setIndex(0);
    }
    virtual void evaluate() override {
      this->pointer = handle.array + this->counter;
    }
  public:
    ArrayIterator(ArrayIterator&& other) = default;
    T& operator*() {
      return *pointer;
    }
    const T& operator*() const {
      return *pointer;
    }
    virtual T& operator[](uint16_t index) override {
      return this->handle.array[index];
    }
    virtual const T& operator[](uint16_t index) const override {
      return this->handle.array[index];
    }
    friend class Array<T>;
  };

  ArrayIterator iterator(uint16_t skip = 0, uint16_t keep = 0, bool reverse = false);
};

template<class T> class PointerArray : public Array<T*> {
protected:
  void nullify() {
    T** end = this->handle.end;
    for (T** it = this->handle.array; it < end; ++it)
      *it = NULL;
  }
public:
  void erase() {
    T** end = this->handle.end;
    for (T** it = this->handle.array; it < end; ++it)
      delNull(*it);
  }
  T* take(uint16_t index) {
    T*& selected = operator[](index);
    T* temp = selected;
    selected = NULL;
    return temp;
  }
  PointerArray() {}
  PointerArray(T* array[], uint16_t size)
    : Array<T*>(array, size) {
    nullify();
  }
  PointerArray(uint16_t size)
    : Array<T*>(size) {
    nullify();
  }
  ~PointerArray() {
    erase();
  }  //5
  PointerArray(const PointerArray& other)
    : Array<T*>(other) {}
  PointerArray(PointerArray&& other)
    : Array<T*>(move(other)) {}
  using Array<T*>::operator=;
};
///Array
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
  uint16_t getSize() const {
    return pointers.getSize();
  }
};
//Stack
template<class T> class StackIteratorRandomIO;
template<class T> class Stack /*: public OrderedCollection<T>*/ {
protected:
  struct StackNode {
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
  StackNode* nodeByIndexTop(uint16_t skip) const;

  StackNode* unChained(StackNode* after);
  StackNode* unChainedNode(uint16_t index);
  StackNode* unChainedNodeFront(uint16_t skip);

  template<class U> void insertNode(uint16_t index, U&& value);
  template<class U> void insertNodeFront(uint16_t skip, U&& value);

  StackNode*& pointerToNodeByIndex(uint16_t index);
  StackNode*& pointerToNodeByIndexTop(uint16_t skip);

  Stack(StackNode* head, StackNode* tail, uint16_t size)
    : head(head), tail(tail), size(size) {}
public:
  Stack() {}
  ~Stack();  //5
  Stack(const Stack& other);
  Stack(Stack&& other);
  Stack& operator=(const Stack& other);
  Stack& operator=(Stack&& other);  ///5

  uint16_t getSize() const;
  void clear();

  const T& peek() const;
  const T& peekBack() const;

  T& itemAt(uint16_t index = 0);
  T& itemLast(uint16_t index = 0);

  T pop();  //normal pop
  T popBack();
  T popFromFront(uint16_t skip);
  T popFromBack(uint16_t index);

  template<class U> void push(U&& value);
  template<class U> void pushBack(U&& value);
  template<class U> void pushFromFront(U&& value, uint16_t skip);
  template<class U> void pushFromBack(U&& value, uint16_t skip);

  void swapItems(uint16_t a, uint16_t b, bool fromTop);  //swap by index
  void reverse();

  Stack<T> cutFront(uint16_t items);  //from top
  Stack<T> cutBack(uint16_t items);   //from top
  Stack<T> cutFront(uint16_t from, uint16_t until);
  Stack<T> cutBack(uint16_t from, uint16_t until);
  Stack<T>& appendFront(Stack<T>&& other);
  Stack<T>& appendBack(Stack<T>&& other);
  Stack<T>& insertFront(Stack<T>&& other, uint16_t skip);
  Stack<T>& insertBack(Stack<T>&& other, uint16_t skip);

  class NodeIterator {  //Very light weight iterator
  protected:
    StackNode* currentNode;
    StackNode* operator->() {
      return currentNode;
    }
  public:
    operator StackNode*() const {
      return currentNode;
    }
    NodeIterator(StackNode& initialNode)
      : currentNode(initialNode) {}
    T& operator*() {
      return currentNode->value;
    }
    bool notEnd() const {
      return currentNode != NULL;
    }
    NodeIterator& operator++() {
      if (notEnd())
        currentNode = currentNode->prev;
      return *this;
    }
    void skip(uint16_t number) {
      while (number) {
        if (currentNode == NULL)
          return;
        currentNode = currentNode->prev;
        --number;
      }
    }
    friend class StackIteratorForwardIO;
    friend class StackIteratorRandom;
    friend class StackIteratorRandomIO<T>;
  };
  NodeIterator iteratorForwardLite(uint16_t skip = 0) {
    NodeIterator it(head);
    it.skip(skip);
    return it;
  }
  class StackIteratorForward : public ForwardIterator<T> {
  protected:
    StackNode* currentNode = NULL;
    Stack* myStack = NULL;

    virtual void reverseIf() {
      if (myStack != NULL) myStack->reverse();
    }
    StackIteratorForward() {}
  public:
    StackIteratorForward(StackIteratorForward&& other)
      : currentNode(other.currentNode), myStack(other.myStack) {}
    StackIteratorForward(StackNode* start, Stack* myStack = NULL)
      : currentNode(start), myStack(myStack) {
      reverseIf();  //first reverse
    }
    ~StackIteratorForward() {
      reverseIf();  //second reverse
    }
    virtual bool notEnd() const override;

    virtual T& operator*() override;
    virtual const T& operator*() const override;
    virtual ForwardIterator<T>& operator++() override;

    void skip(uint16_t items) {
      uLimited(items, myStack->size - this->counter);
      if (items)
        ForwardIterator<T>::skip(items);
    }
    friend class Stack<T>;
  };  ///StackIteratorForward

  class StackIteratorForwardIO : public StackIteratorForward {
  protected:
    bool reverse = false;
    StackNode** pointerToCurrentNode = NULL;

    virtual void reverseIf() override {
      if (reverse) this->myStack->reverse();
    }
    T manualPop(StackNode*& popping, StackNode* overwrite) {
      --(this->myStack->size);  //manually decr stack size
      StackNode* temp = popping;
      popping = overwrite;
      return temp->pop();  //manually pop
    }
    template<class U> void manualPush(U&& value, StackNode*& pointerToNew) {
      ++this->myStack->size;
      pointerToNew = new StackNode(pointerToNew->prev, forward(value));
    }
    void placeAfter(StackNode*& pointerToThis);
    StackNode* nodeBefore() const;
  public:
    StackIteratorForwardIO(StackIteratorForwardIO&& other)
      : StackIteratorForward(move(other)), pointerToCurrentNode(other.pointerToCurrentNode), reverse(other.reverse) {}
    StackIteratorForwardIO(Stack& myStack, bool reverse = false)
      : StackIteratorForward(myStack.head, &myStack), reverse(reverse), pointerToCurrentNode(&(myStack.pointerToNodeByIndexTop(0))) {}

    void beginFrom(uint16_t index);
    void skip(uint16_t items);

    uint16_t globalByOffset(int16_t offset) {
      return this->counter + offset;
    }
    bool limitedOffsetPop(int16_t& offset) {
      return doubleLimited<int16_t>(offset, -this->counter, this->myStack->size - this->counter);
    }
    bool limitedOffsetPush(int16_t& offset) {
      return doubleLimitedRStrict<int16_t>(offset, -this->counter, this->myStack->size - this->counter);
    }
    bool isValidOffsetPop(int16_t offset) {
      return inSpanRStrict<int16_t>(-this->counter, offset, this->myStack->size - this->counter);
    }
    bool isValidOffsetPush(int16_t offset) {
      return inSpan<int16_t>(-this->counter, offset, this->myStack->size - this->counter);
    }
    virtual ForwardIterator<T>& operator++() override;

    T pop();                      //pop current
    T popOffset(int16_t offset);  //pop relatevely

    T popFront();  //normal pops
    T popBack();
    T popFromFront(uint16_t skip);
    T popFromBack(uint16_t index);

    template<class U> void push(U&& value);                        //push after current
    template<class U> void pushOffset(U&& value, int16_t offset);  //push relatevely

    template<class U> void pushFront(U&& value);  //normal pushes
    template<class U> void pushBack(U&& value);
    template<class U> void pushFromFront(U&& value, uint16_t skip);
    template<class U> void pushFromBack(U&& value, uint16_t skip);
    /*
    Stack<T> cutLast(uint16_t items);  //

    Stack<T> cutNext(uint16_t items, bool inclusive);
    Stack<T> cutPrevios(uint16_t items, bool inclusive);
    Stack<T> cutLocal(int16_t from, int16_t until);

    Stack<T> cut(uint16_t from, uint16_t until);
    StackIteratorForwardIO& append(Stack<T>&& other);
    StackIteratorForwardIO& insertAt(Stack<T>&& other, uint16_t at);
    StackIteratorForwardIO& insertAtLocal(Stack<T>&& other, uint16_t at);

    void swapItems(uint16_t a, uint16_t b, bool fromTop);
    void swapItemsLocal(int16_t a, int16_t b);
    */
    friend class Stack<T>;
  };  ///StackIteratorForwardIO

  class StackIteratorRandom : public CollectionRandomAccessIterator<T> {
  protected:
    StackIteratorRandom(Array<T*>&& pointers)
      : CollectionRandomAccessIterator<T>(move(pointers)) {}
    friend class Stack<T>;
  };

  StackIteratorForward iteratorForward(bool reverse = false, uint16_t skip = 0);
  StackIteratorForwardIO iteratorForwardIO(bool reverse = false, uint16_t skip = 0);
  StackIteratorRandom iteratorRandom(bool backwards = false) const;
  StackIteratorRandomIO<T> iteratorRandomIO(bool backwards = false) const;
protected:
  Array<T*> arrayForRandomAccess() const;

  friend class StackIteratorForwardIO;
  friend class StackIteratorRandom;
  friend class StackIteratorRandomIO<T>;
};  ///Stack
template<class T> class PointerStack : public Stack<SPtr<T>> {
public:
  using Stack<SPtr<T>>::Stack;
};

template<class T> struct _Constructors : public T {  //struct for inheriting and using all constructors (not sure it's worth it)
  using T::T;
};
/*
Representation is an ordered collection of read only elements
template <class T> Rep : ICollection {
  virtual T operator[](uint16_t index) const;
};
where T may be (const T&) (derive from IndexedCollectionRead<T>) or value T
Representation is to already existing or internal data.
*/
//Reps
template<class T> struct Representation {  //representation has type of subscript return
public:
  typedef T ReturnType;
  typedef typename _Remove_reference<T>::_noRef _Type;
  class RIterator {
  public:
    virtual ReturnType operator*() = 0;
    virtual RIterator& operator++() = 0;
    virtual bool notEnd() = 0;
    void skip(uint16_t items) {
      while (items) {
        this->operator++();
        --items;
      }
    }
  };
  class DumbRiterator : public RIterator {
  protected:
    uint16_t index = 0;
    uint16_t limit;
    Representation& rep;
  public:
    uint16_t getCounter() const {
      return index;
    }
    DumbRiterator(Representation& rep)
      : rep(rep), limit(rep.getSize()) {}
    virtual ReturnType operator*() override {
      return rep.readAt(index);
    }
    virtual RIterator& operator++() override {
      if (notEnd())
        ++index;
    }
    virtual bool notEnd() override {
      index < limit;
    }
  };
  SPtr<Representation> getProxy() const {
    return SPtr<Representation>(new Reference(*this));
  }
  virtual ReturnType readAt(uint16_t index) = 0;
  virtual uint16_t getSize() = 0;
  virtual SPtr<RIterator> iteratorForwardV() = 0;
protected:
  class Reference : public Representation {
  protected:
    Representation& ref;
    using typename Representation::ReturnType;
    using typename Representation::_Type;
    using typename Representation::RIterator;
  public:
    Reference(Representation& ref)
      : ref(ref) {}
    virtual ReturnType readAt(uint16_t index) override {
      return ref.readAt(index);
    }
    virtual uint16_t getSize() override {
      return ref.getSize();
    }
    virtual SPtr<RIterator> iteratorForwardV() override {
      return ref.iteratorForwardV();
    }
  };
};
//VariableRepresentation : Rep<T>, OrderedAccess<T>
template<class TRepresentation> struct VariableRepresentation : public TRepresentation, public OrderedAccess<typename TRepresentation::_Type> {
  using typename TRepresentation::ReturnType;
  using typename TRepresentation::_Type;
};
//Aliases for val and ref reps
template<class T> using ValRep = Representation<T>;
template<class T> using RefRep = Representation<T&>;

template<class TRepresentation> class SpanRepresentation : public TRepresentation {
  using typename TRepresentation::ReturnType;
  using typename TRepresentation::_Type;
protected:
  using typename TRepresentation::RIterator;
  class SpanIterator : public RIterator {
    uint16_t counter = 0;
    uint16_t& until;
    SPtr<RIterator> itP;  //sub iterator pointer
  public:
    SpanIterator(SpanRepresentation& sRep, TRepresentation& rep)
      : until(sRep.until), itP(move(rep.iteratorForwardV())) {
      skip(sRep.from);
    }
    virtual ReturnType operator*() override {
      return *(*itP);
    }
    virtual RIterator& operator++() override {
      ++(*itP);
      ++counter;
      return *this;
    }
    virtual bool notEnd() override {
      return counter < until;
    }
  };

  TRepresentation* rep;
  uint16_t from;
  uint16_t until;
public:
  SpanRepresentation(const volatile TRepresentation& rep, uint16_t from, uint16_t until = 0xFFFF)
    : rep(&rep) {
    uLimited(until, rep.getSize());  //max until = size
    this->until = until;
    this->from = uLimitStrict(from, until);  //from is less then until
  }
  TRepresentation* sourcePtr() const;
  uint16_t getFrom() const;
  uint16_t getUntil() const;

  void setFrom(uint16_t from);
  void setUntil(uint16_t until);

  void updateSpan();
  void reset(uint16_t from, uint16_t until);

  virtual ReturnType readAt(uint16_t index) override;
  virtual uint16_t getSize() override;
  virtual SPtr<RIterator> iteratorForwardV() override {
    return SPtr<RIterator>(new SpanIterator(*this, *rep));
  }
};  ///SpanRepresentation

template<class TVariableRepresentation> class VariableSpanRepresentation : public SpanRepresentation<TVariableRepresentation> {
  using typename TVariableRepresentation::ReturnType;
  using typename TVariableRepresentation::_Type;
public:
  virtual _Type& itemAt(uint16_t index) override {
    return this->rep->itemAt(this->from + index);
  }
};  ///VariableSpanRepresentation

template<class TRepresentation> class RepresentationComposition : public TRepresentation {
protected:
  using typename TRepresentation::ReturnType;
  using typename TRepresentation::_Type;
  using RepPtr = SPtr<TRepresentation>;  //smart pointer type
  class Part {                           //in stack
  protected:
    RepPtr rep;                 //abstract representation
    uint16_t limitGlobalIndex;  //last global index
  public:
    Part(TRepresentation* rep)
      : rep(rep) {}
    Part(const RepPtr& rep)
      : rep(rep) {}
    Part(RepPtr&& rep)
      : rep(move(rep)) {}
    bool isIndexInside(uint16_t index) const {
      return index < limitGlobalIndex;
    }
  };
  using typename Representation<ReturnType>::RIterator;
  class CompositonIterator : public RIterator {
  protected:
    using NodeIterator = decltype(nulPtr<Stack<Part>>()->iteratorForwardLite());
    NodeIterator stackIterator;
    SPtr<RIterator> subIteratorPtr;
  public:
    CompositonIterator(NodeIterator& stackIterator)
      : stackIterator(stackIterator) {
      if (stackIterator.notEnd())
        subIteratorPtr = move((*stackIterator).rep->iteratorForwardV());
    }
    virtual ReturnType operator*() override {
      return *(*subIteratorPtr);
    }
    virtual RIterator& operator++() override {
      subIteratorPtr->operator++();  //iterate in part
checkSubIterator:
      if (subIteratorPtr->notEnd())  //if subIt not ended do nothing
        goto end;
      subIteratorPtr.delPtr();  //else delete subIt
      ++stackIterator;          //iterate to next part
      if (stackIterator.notEnd()) {
        subIteratorPtr = move((*stackIterator).rep->iteratorForwardV());  //set subIt to new iterator
        goto checkSubIterator;                                            //check new subIt
      }
end:  //stackIterator might be ended at this point
      return *this;
    }
    virtual bool notEnd() override {
      stackIterator.notEnd();
    }
  };
  Stack<Part> stack;  //Stack of Parts
  uint16_t size = 0;  //Sum of T elements
  uint16_t dirtyElement = 0xFFFF;

  void removeEmpty();
  void evaluateMetaData(uint16_t fromPartIndex = 0);  //recalculate limitGlobalIndex from part in stack
public:
  void cleanElements(bool removeEmptyElements = false);

  RepresentationComposition() {}  //empty
  ~RepresentationComposition();   //5
  RepresentationComposition(const RepresentationComposition& other)
    : size(other.size), dirtyElement(other.dirtyElement), stack(other.stack) {}  //copy
  RepresentationComposition(RepresentationComposition&& other)
    : size(other.size), dirtyElement(other.dirtyElement), stack(move(other.stack)) {}
  RepresentationComposition(PointerStack<TRepresentation>&& other);
  RepresentationComposition& operator=(const RepresentationComposition& other) {
    size = other.size;
    stack = other.stack;
    return *this;
  }
  RepresentationComposition& operator=(RepresentationComposition&& other) {
    size = other.size;
    stack = move(other.stack);
    return *this;
  }  ///5

  virtual ReturnType readAt(uint16_t index) override;
  virtual uint16_t getSize() override;
  virtual SPtr<RIterator> iteratorForwardV() override;

  //Stack overloads
  void clear() {
    stack.clear();
  }

  const TRepresentation& partAt(uint8_t index = 0) {
    return *(stack.itemAt(index).rep);
  }
  const TRepresentation& partLast(uint8_t index = 0) {
    return *(stack.itemLast(index).rep);
  }

  RepPtr pop() {       //pop first
    dirtyElement = 0;  //everything dirty
    RepPtr temp = move(stack.pop().rep);
    size -= temp->getSize();  //decr size
    return temp;
  }                   //normal pop
  RepPtr popBack() {  //pop last, nothing new dirty
    RepPtr temp = move(stack.popBack().rep);
    size -= temp->getSize();
    return temp;
  }
  RepPtr popFromFront(uint16_t skip) {
    uLimited(dirtyElement, skip);  //dirty limited to skip
    RepPtr temp = move(stack.popFromFront(skip).rep);
    size -= temp->getSize();
    return temp;
  }
  RepPtr popFromBack(uint16_t index) {
    return popFromFront(stack.size - 1 - index);
  }
  void push(RepPtr&& sptr) {  //push first
    dirtyElement = 0;         //everything dirty
    size += sptr->getSize();
    stack.push(move(sptr));
  }
  void pushBack(RepPtr&& sptr) {
    dirtyElement = stack.size;
    size += sptr->getSize();
    stack.pushBack(move(sptr));
  }
  void pushFromFront(RepPtr&& sptr, uint16_t skip) {
    uLimited(dirtyElement, skip);
    size += sptr->getSize();
    stack.pushFromFront(move(sptr), skip);
  }
  void pushFromBack(RepPtr&& sptr, uint16_t skip) {
    pushFromFront(move(sptr), stack->size - 1 - skip);
  }
  void swapParts(uint16_t a, uint16_t b, bool fromTop) {  //swap by index
    uLimited(dirtyElement, min(a, b));
    stack.swapItems(a, b, fromTop);
  }
  void reverse() {
    dirtyElement = 0;
    stack.reverse();
  }
  RepresentationComposition<TRepresentation>& appendPartsFront(RepresentationComposition<TRepresentation>&& other) {
    uLimited(dirtyElement, stack.size);
    stack.appendFront(move(other.stack));
  }
  RepresentationComposition<TRepresentation>& appendPartsBack(RepresentationComposition<TRepresentation>&& other) {
    uLimited(dirtyElement, stack.size);
    stack.appendBack(move(other.stack));
  }
  RepresentationComposition<TRepresentation>& insertPartsAt(RepresentationComposition<TRepresentation>&& other, uint16_t at) {
    uLimited(dirtyElement, at);
    stack.insertFront(move(other.stack));
  }
};  ///RepresentationComposition




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
template<class T> constexpr nulPtr() noexcept {
  return (T*)NULL;
}
template<class T> void delNull(T*& ptr) {
  delete ptr;
  ptr = NULL;
}
template<class T> void takeNull(T*& ptr) {
  T* temp = ptr;
  ptr = NULL;
  return temp;
}
//BidirectionalIterator
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::operator+=(int16_t delta) {
  if (backwards) delta = -delta;
  this->counter += delta;
  evaluate();
  return *this;
}
template<class T> ForwardIterator<T>& BiderectionalIterator<T>::operator++() {
  return operator+=(1);
};
template<class T> bool BiderectionalIterator<T>::goToItemNotEnd(uint16_t index) {
  if (index > this->counter)
    this->operator+=(index - this->counter);     //c < index; ->
  else this->operator+=(this->counter - index);  //index < c; <-
  return this->notEnd();                         //say if item in bounds
}
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::operator--() {
  return operator+=(-1);
};
template<class T> BiderectionalIterator<T>& BiderectionalIterator<T>::reverse() {
  backwards = !backwards;
  return *this;
}
///BidirectionalIterator
//FuncIterator
template<class T> void FixedSizeIterator<T>::setIndex(uint16_t index) {
  this->counter = this->backwards ? mirrorValueOnRange<uint16_t>(0, index, getSize()) : index;
  evaluate();
}
template<class T> T& FunctionIterator<T>::operator*() {
  return result;
}
template<class T> const T& FunctionIterator<T>::operator*() const {
  return result;
}
///FuncIterator
//FixedSizeIterator
template<class T> FixedSizeIterator<T>& FixedSizeIterator<T>::begin(bool backwards) {
  this->backwards = backwards;
  setIndex(0);
  return *this;
}
///FixedSizeIterator
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
  if (!index) return tail;
  if (index >= size) return NULL;
  return nodeByIndexTop(size - 1 - index);  //how much skip
}
template<class T> typename Stack<T>::StackNode* Stack<T>::nodeByIndexTop(uint16_t skip) const {
  StackNode* temp = head;
  while (skip) {  //while skip
    temp = temp->prev;
    --skip;
  }
  return temp;
}
template<class T> typename Stack<T>::StackNode* Stack<T>::unChained(StackNode* after) {
  StackNode* selected = after->prev;
  after->prev = selected->prev;
  --size;
  return selected;
}
template<class T> typename Stack<T>::StackNode* Stack<T>::unChainedNode(uint16_t index) {
  return unChained(nodeByIndex(index + 1));
}
template<class T> typename Stack<T>::StackNode* Stack<T>::unChainedNodeFront(uint16_t skip) {
  return unChained(nodeByIndexTop(skip - 1));
}
template<class T> template<class U> void Stack<T>::insertNode(uint16_t index, U&& value) {
  StackNode* after = nodeByIndex(index);
  after->prev = new StackNode(after->prev, forward<U>(value));
  ++size;
}
template<class T> template<class U> void Stack<T>::insertNodeFront(uint16_t skip, U&& value) {
  StackNode* after = nodeByIndexTop(skip);
  after->prev = new StackNode(after->prev, forward<U>(value));
  ++size;
}
template<class T> typename Stack<T>::StackNode*& Stack<T>::pointerToNodeByIndex(uint16_t index) {
  uLimitedStrict(index, size);
  if (index == (size - 1)) return head;
  return nodeByIndex(index + 1)->prev;
}
template<class T> typename Stack<T>::StackNode*& Stack<T>::pointerToNodeByIndexTop(uint16_t skip) {
  uLimitedStrict(skip, size);
  if (!skip) return head;
  return nodeByIndexTop(skip - 1)->prev;
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
template<class T> T& Stack<T>::itemAt(uint16_t index) {
  return nodeByIndex(this->size - index - 1)->value;
}
template<class T> T& Stack<T>::itemLast(uint16_t index) {
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
  return T();
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
  return T();
}
template<class T> T Stack<T>::popFromFront(uint16_t skip) {
  uLimitedStrict(skip, size);
  if (!skip) return pop();
  if (skip == (size - 1)) return popBack();
  if (!size) return T();
  return unChainedNodeFront(skip)->pop();
}
template<class T> T Stack<T>::popFromBack(uint16_t index) {
  uLimitedStrict(index, size);
  if (!index) return popBack();
  if (index == (size - 1)) return pop();
  if (!size) return T();
  return unChainedNode(index)->pop();
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
  if (!skip) return push(forward<U>(value));
  uLimited(skip, size);
  if (skip == size) return pushBack(forward<U>(value));
  if (size) return insertNode(size - skip, forward<U>(value));
  push(forward<U>(value));
}
template<class T> template<class U> void Stack<T>::pushFromBack(U&& value, uint16_t skip) {
  if (!skip) return pushBack(forward<U>(value));
  uLimited(skip, size);
  if (skip == size) return push(forward<U>(value));
  if (size) return insertNode(skip, forward<U>(value));
  pushBack(forward<U>(value));
}
template<class T> void Stack<T>::swapItems(uint16_t a, uint16_t b, bool fromTop) {
  if (a == b) return;
  if (size < 2) return;
  uLimitedStrict(a, size);
  uLimitedStrict(b, size);
  if (fromTop) {
    a = mirrorValueOnRange<uint16_t>(0, a, size);
    b = mirrorValueOnRange<uint16_t>(0, b, size);
  }
  StackNode*& aP = pointerToNodeByIndex(a);
  StackNode*& bP = pointerToNodeByIndex(b);
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
template<class T> Stack<T> Stack<T>::cutFront(uint16_t items) {
  uLimited(items, size);  //items<=size
  if (!items) return Stack();
  if (items == size) return Stack(move(*this));     //else 0 < items < size
  StackNode* tempHead = head;                       //first node of span is head
  StackNode* tempTail = nodeByIndexTop(items - 1);  //last node of span is it-1 element
  head = tempTail->prev;                            //move this head to after tTail
  tempTail->prev = NULL;                            //disconnect span from this stack chain
  size -= items;                                    //dec size
  return Stack(tempHead, tempTail, items);
}
template<class T> Stack<T> Stack<T>::cutBack(uint16_t items) {
  uLimited(items, size);  //items<=size
  if (!items) return Stack();
  if (items == size) return Stack(move(*this));  //else 0 < items < size
  StackNode* tempTail = tail;                    //tail of cut is same as current tail
  tail = nodeByIndexTop(size - 1 - items);       //set tail to last node of this
  StackNode* tempHead = tail->prev;              //cut starts from prev of that
  tail->prev = NULL;                             //disconnect chain
  size -= items;                                 //dec size
  return Stack(tempHead, tempTail, items);
}
template<class T> Stack<T> Stack<T>::cutFront(uint16_t from, uint16_t until) {
  uLimited(until, size);
  uLimited(from, until);
  if (until == from) return Stack();                         //empty; else some items
  if (!from) return cutFront(until - from);                  //cut from front. else from > 0
  if (until == size) return cutBack(until - from);           //cut from back, else until < size ; 0 < from <= lastIndex < indexuntil < size
  StackNode*& firstPointer = pointerToNodeByIndexTop(from);  //pointer to first cutting node
  StackNode* tempHead = firstPointer;                        //assign value of pointer to tempHead
  uint16_t items = (until - from);
  until -= from + 1;                   //use until as delta between first and last element of span
  StackNode* tempTail = firstPointer;  //set  pointer to iterate from
  while (until) {
    tempTail = tempTail->prev;
    --until;
  }                               //tempTail=last element of span <done>
  firstPointer = tempTail->prev;  //shortcut this chain
  tempTail->prev = NULL;          //disconnect span from chain
  size -= items;
  return Stack(tempHead, tempTail, items);  //return Stack with cut chain
}
template<class T> Stack<T> Stack<T>::cutBack(uint16_t from, uint16_t until) {
  return cutFront(size - until, size - from);
}
template<class T> Stack<T>& Stack<T>::appendFront(Stack<T>&& other) {  //insert before everything from front
  if (!other.size) return *this;
  if (!size) return (*this) = move(other);
  other.tail->prev = head;  //conect head to other tail
  head = other.head;        //take head;
  size += other.size;       //add size
  other.head = NULL;
  other.tail = NULL;
  other.size = 0;  //other empty
  return *this;
}
template<class T> Stack<T>& Stack<T>::appendBack(Stack<T>&& other) {  //insert before everything from front
  if (!other.size) return *this;
  if (!size) return (*this) = move(other);
  tail->prev = other.head;  //connect head to the tail
  tail = other.tail;        //move tail to last element
  size += other.size;       //add size
  other.head = NULL;
  other.tail = NULL;
  other.size = 0;  //other empty
  return *this;
}
template<class T> Stack<T>& Stack<T>::insertFront(Stack<T>&& other, uint16_t at) {
  if (!other.size) return *this;                   //other empty
  if (!size) return (*this) = move(other);         //this empty
  if (at >= size) return appendBack(move(other));  //after all
  if (!at) return appendFront(move(other));        //before all
  StackNode* afterNode = nodeByIndexTop(at - 1);   //0 < at < size; after this node
  other.tail->prev = afterNode->prev;              //connect other chain's tail
  afterNode->prev = other.head;                    //connect other chain's head
  size += other.size;                              //add size
  other.head = NULL;
  other.tail = NULL;
  other.size = 0;  //other empty
  return *this;
}
template<class T> Stack<T>& Stack<T>::insertBack(Stack<T>&& other, uint16_t at) {
  if (!other.size) return *this;                    //other empty
  if (!size) return (*this) = move(other);          //this empty
  if (at >= size) return appendFront(move(other));  //before all
  if (!at) return appendBack(move(other));          //after all
  StackNode* afterNode = nodeByIndex(at);           //0 < at < size; after this node
  other.tail->prev = afterNode->prev;               //connect other chain's tail
  afterNode->prev = other.head;                     //connect other chain's head
  size += other.size;                               //add size
  other.tail = NULL;
  other.head = NULL;
  other.size = 0;  //other empty
  return *this;
}
//Stack<T>::StackIteratorForward

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
template<class T> typename Stack<T>::StackIteratorForward Stack<T>::iteratorForward(bool reverse, uint16_t skip) {
  auto it = StackIteratorForward(this->head, reverse ? this : NULL);
  it.skip(skip);
  return it;
}
template<class T> typename Stack<T>::StackIteratorForwardIO Stack<T>::iteratorForwardIO(bool reverse, uint16_t skip) {
  auto it = StackIteratorForwardIO(*this, reverse);
  it.skip(skip);
  return it;
}
//StackIteratorForwardIO
template<class T> void Stack<T>::StackIteratorForwardIO::placeAfter(StackNode*& pointerToThis) {
  pointerToCurrentNode = &pointerToThis;
  this->currentNode = *pointerToCurrentNode;
}
template<class T> typename Stack<T>::StackNode* Stack<T>::StackIteratorForwardIO::nodeBefore() const {
  return (StackNode*)((void*)(pointerToCurrentNode)-offsetof(StackNode, prev));
}
template<class T> void Stack<T>::StackIteratorForwardIO::beginFrom(uint16_t index) {
  this->counter = index;
  placeAfter(this->myStack->pointerToNodeByIndex(index));
}
template<class T> void Stack<T>::StackIteratorForwardIO::skip(uint16_t items) {
  uLimitedStrict(items, this->myStack->size - this->counter);
  if (!items) return;
  StackIteratorForward::skip(items - 1);
  pointerToCurrentNode = &this->currentNode->prev;
  this->currentNode = this->currentNode->prev;
}
template<class T> ForwardIterator<T>& Stack<T>::StackIteratorForwardIO::operator++() {
  if (notEnd()) {
    pointerToCurrentNode = &(this->currentNode->prev);
    this->currentNode = *pointerToCurrentNode;
    ++(this->counter);
  }
  return *this;
}
template<class T> T Stack<T>::StackIteratorForwardIO::pop() {
  return popFromFront(this->counter);
}
template<class T> T Stack<T>::StackIteratorForwardIO::popOffset(int16_t offset) {
  limitedOffsetPop(offset);
  return popFromFront(this->counter + offset);
}
template<class T> T Stack<T>::StackIteratorForwardIO::popFront() {  //stack pop(Front(first node))
  return popFromFront(0);
}
template<class T> T Stack<T>::StackIteratorForwardIO::popBack() {  //stack popBack, pop last node
  return popFromFront(this->myStack->size - 1);
}
template<class T> T Stack<T>::StackIteratorForwardIO::popFromFront(uint16_t skip) {  //main access
  uLimitedStrict(skip, this->myStack->size);                                         //force skip to valid value
  if (this->myStack->size < 2) {                                                     //pop very last item in stack
    this->currentNode = NULL;
    pointerToCurrentNode = &this->myStack->head;
    return this->myStack->pop();
  }                                                                             //size>1
  if (skip <= this->counter)                                                    //pop this or before(can't descent from current)
    switch (this->counter - skip) {                                             //difference between current and pop
      case 0:                                                                   //POP CURRENT: skip=counter
        if (this->counter == (this->myStack->size - 1)) {                       //POP LAST ITEM: maxIndex>0 => skip>0
          beginFrom(skip - 1);                                                  //place iterator at previous node
          this->myStack->tail = this->currentNode;                              //put tail to previous
          return manualPop(this->currentNode->prev, NULL);                      //pop last, write null
        }                                                                       // else counter<maxIndex there is node after
        return manualPop(*pointerToCurrentNode, this->currentNode->prev);       //pop current, replace with next
      case 1:                                                                   //POP PREVIOS:
        pointerToCurrentNode = &this->myStack->pointerToNodeByIndex(skip - 1);  //pointerToCurrentNode=pointer to previous
        --this->counter;
        return manualPop(*pointerToCurrentNode, this->currentNode);  //pop previous, peplace with this
      default:                                                       //POP FAR BEHIND
        --this->counter;                                             //collapse back
        return this->myStack->popFromFront(skip);
    }
  //counter < skip < size
  skip -= this->counter + 1;                                    //how many pointers to skip
  StackNode** pointerToPoppingNode = &this->currentNode->prev;  //pointer to pointer to node after current
  while (skip) {                                                //iterate for skip jumping over nodes
    pointerToPoppingNode = &(*pointerToPoppingNode->prev);
    --skip;
  }
  return manualPop(*pointerToPoppingNode, (*pointerToPoppingNode)->prev);  //pop last and rewrite with prev
}
template<class T> T Stack<T>::StackIteratorForwardIO::popFromBack(uint16_t index) {
  return popFromFront(this->myStack->size - 1 - index);
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::push(U&& value) {
  if (this->myStack->size) {  //push into existing chain
    manualPush(forward(value), *pointerToCurrentNode);
    if (this->currentNode == NULL) this->stack->tail = *pointerToCurrentNode;
    this->currentNode = *pointerToCurrentNode;
  }
  this->myStack->push(forward(value));  //push first item
  placeAfter(this->myStack->head);      //place iterator
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::pushOffset(U&& value, int16_t offset) {
  limitedOffsetPush(offset);
  pushFromFront(forward(value), this->counter + offset);
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::pushFront(U&& value) {
  pushFromFront(forward(value), 0);
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::pushBack(U&& value) {
  pushFromFront(forward(value), this->myStack->size);
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::pushFromFront(U&& value, uint16_t skip) {
  uLimited(skip, this->myStack->size);  // skip<=size, counter<=size
  if (skip == this->counter) return push(forward(value));
  if (skip < this->counter) {           //counter>skip, counter>0 skip>=0
    if ((this->counter - skip) == 1) {  //push behind
      manualPush(forward(value), *pointerToCurrentNode);
      pointerToCurrentNode = (*pointerToCurrentNode)->prev;
      ++this->counter;
      return;
    }  //push far behind
    ++this->counter;
    this->myStack->pushFromFront(forward(value), skip);
    return;
  }  //counter < skip
  StackNode** pointerToNewNode = &this->currentNode->prev;
  bool afterAllItems = (skip == this->myStack->size);
  skip -= this->counter + 1;
  while (skip) {
    pointerToNewNode = &pointerToNewNode->prev;
    --skip;
  }
  manualPush(forward(value), *pointerToNewNode);
  if (afterAllItems) this->myStack->tail = *pointerToNewNode;
}
template<class T> template<class U> void Stack<T>::StackIteratorForwardIO::pushFromBack(U&& value, uint16_t skip) {
  pushFromFront(forward(value), this->myStack->size - skip);
}
/*
template<class T> Stack<T> Stack<T>::StackIteratorForwardIO::cutLast(uint16_t items);

template<class T> Stack<T> Stack<T>::StackIteratorForwardIO::cutNext(uint16_t items, bool inclusive);
template<class T> Stack<T> Stack<T>::StackIteratorForwardIO::cutPrevios(uint16_t items, bool inclusive);
template<class T> Stack<T> Stack<T>::StackIteratorForwardIO::cutLocal(int16_t from, int16_t until);

template<class T> Stack<T> Stack<T>::StackIteratorForwardIO::cut(uint16_t from, uint16_t until);
template<class T> typename Stack<T>::StackIteratorForwardIO& Stack<T>::StackIteratorForwardIO::append(Stack<T>&& other);
template<class T> typename Stack<T>::StackIteratorForwardIO& Stack<T>::StackIteratorForwardIO::insertAt(Stack<T>&& other, uint16_t at);
template<class T> typename Stack<T>::StackIteratorForwardIO& Stack<T>::StackIteratorForwardIO::insertAtLocal(Stack<T>&& other, uint16_t at);

template<class T> void Stack<T>::StackIteratorForwardIO::swapItems(uint16_t a, uint16_t b, bool fromTop) {
}
template<class T> void Stack<T>::StackIteratorForwardIO::swapItemsLocal(int16_t a, int16_t b) {
}
*/
///StackIteratorForwardIO

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
//Reps
template<class TRepresentation> TRepresentation* SpanRepresentation<TRepresentation>::sourcePtr() const {
  return rep;
}
template<class TRepresentation> uint16_t SpanRepresentation<TRepresentation>::getFrom() const {
  return from;
}
template<class TRepresentation> uint16_t SpanRepresentation<TRepresentation>::getUntil() const {
  return until;
}
template<class TRepresentation> void SpanRepresentation<TRepresentation>::setFrom(uint16_t from) {
  reset(from, until);
}
template<class TRepresentation> void SpanRepresentation<TRepresentation>::setUntil(uint16_t until) {
  reset(from, until);
}
template<class TRepresentation> void SpanRepresentation<TRepresentation>::updateSpan() {
  *this = SpanRep(*rep, from, until);
}
template<class TRepresentation> void SpanRepresentation<TRepresentation>::reset(uint16_t from, uint16_t until) {
  *this = SpanRep(*rep, from, until);
}
template<class TRepresentation> typename TRepresentation::ReturnType SpanRepresentation<TRepresentation>::readAt(uint16_t index) {
  return rep->readAt(from + index);
}
template<class TRepresentation> uint16_t SpanRepresentation<TRepresentation>::getSize() {
  return until - from;
}

//RepresentationComposition
template<class TRepresentation> uint16_t RepresentationComposition<TRepresentation>::getSize() {
  if (dirtyElement != 0xFFFF) cleanElements();
  return this->size;
}
template<class TRepresentation> typename TRepresentation::ReturnType RepresentationComposition<TRepresentation>::readAt(uint16_t index) {
  uLimitedStrict(index, size);
  for (auto it = stack.iteratorForward(); it.notEnd(); ++it) {
    auto& part = *it;
    if (index < part.limitGlobalIndex)
      return *(part.rep)[index - part.limitGlobalIndex - part.rep->GetSize()];
  }
  return TRepresentation::ReturnType();
}
template<class TRepresentation> SPtr<typename RepresentationComposition<TRepresentation>::RIterator> RepresentationComposition<TRepresentation>::iteratorForwardV() {
  return SPtr<RIterator>(new CompositonIterator(stack.iteratorForwardLite()));
}
template<class TRepresentation> void RepresentationComposition<TRepresentation>::evaluateMetaData(uint16_t fromPartIndex = 0) {  //recalculate indexes
  uint16_t stackSize = stack.getSize();
  if (!stackSize) {  //if 0
    this->size = 0;  //size=0
    goto done;       //done
  }
  if (fromPartIndex >= stackSize)  //if fromIndex>=stackSize also done;
    goto done;
  {  //In this case fromPartIndex is existing element
    auto it = stack.iteratorForward();
    if (fromPartIndex) {  //skip before initial iteration
      it.skip(fromPartIndex - 1);
      this->size = *it.limitGlobalIndex;  //size= valid lGI from previous element
      ++it;                               //go to next first dirty element
      auto& el = *it;
      size += el.rep->getSize();   //add size
      el.limitGlobalIndex = size;  //limit index
    } else
      this->size = 0;            //if no skip
    for (; it.notEnd(); ++it) {  //for each rest
      auto& el = *it;
      size += el.rep->getSize();   //add size
      el.limitGlobalIndex = size;  //last index is before size
    }
  }
done:
  dirtyElement = 0xFFFF;  //mark dirty element as max value
}
template<class TRepresentation> void RepresentationComposition<TRepresentation>::removeEmpty() {
  for (auto it = stack.iteratorForwardIO(); it.notEnd(); ++it)
    if (!(*it).rep->getSize()) it.pop();
}
template<class TRepresentation> void RepresentationComposition<TRepresentation>::cleanElements(bool removeEmptyElements) {
  if (removeEmptyElements) removeEmpty();
  evaluateMetaData(dirtyElement);
}
///RepresentationComposition
///Reps
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
template<class T> T uLimit(const T& value, const T& limit) {
  if (value > limit) return limit;
  return value;
}
template<class T> T uLimitStrict(T value, const T& limit) {
  if (value >= limit) return limit - 1;
  return value;
}
template<class T> bool uLimited(T& value, const T& limit) {
  if (value <= limit) return false;
  value = limit;
  return true;
}
template<class T> bool uLimitedStrict(T& value, const T& limit) {
  if (value < limit) return false;
  value = limit - 1;
  return true;
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
template<class T> bool doubleLimited(T& value, const T& lLimit, const T& uLimit) {
  if (value > uLimit) {
    value = uLimit;
    return true;
  }
  if (value < lLimit) {
    value = uLimit;
    return true;
  }
  return false;
}
template<class T> bool doubleLimitedStrict(T& value, const T& lLimit, const T& uLimit) {
  if (value >= uLimit) {
    value = uLimit - 1;
    return true;
  }
  if (value <= lLimit) {
    value = uLimit + 1;
    return true;
  }
  return false;
}
template<class T> bool doubleLimitedLStrict(T& value, const T& lLimit, const T& uLimit) {
  if (value > uLimit) {
    value = uLimit;
    return true;
  }
  if (value <= lLimit) {
    value = uLimit + 1;
    return true;
  }
  return false;
}
template<class T> bool doubleLimitedRStrict(T& value, const T& lLimit, const T& uLimit) {
  if (value >= uLimit) {
    value = uLimit - 1;
    return true;
  }
  if (value < lLimit) {
    value = uLimit;
    return true;
  }
  return false;
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
template<class T> bool inSpan(const T& a, const T& value, const T& b) {
  if (a <= value)
    return (value <= b);
  return false;
}
template<class T> bool inSpanStrict(const T& a, const T& value, const T& b) {
  if (a < value)
    return (value < b);
  return false;
}
template<class T> bool inSpanLStrict(const T& a, const T& value, const T& b) {
  if (a < value)
    return (value <= b);
  return false;
}
template<class T> bool inSpanRStrict(const T& a, const T& value, const T& b) {
  if (a <= value)
    return (value < b);
  return false;
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
template<class T> bool sort(T& a, T& b, bool ascending, bool (*compareFunc)(const T& a, const T& b)) {
  bool swapped = ((*compareFunc)(a, b) == ascending);
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

template<class T> T& larger(T& a, T& b, const bool invert = false) {
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