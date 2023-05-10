#pragma once
#include "Piece.h"
#include "Arduino.h"
#include <stdint.h>

template<class T>
struct StackElement {  //simple stack element
  T value;
  StackElement<T>* previous;
  StackElement(T val, StackElement<T>* prev);
};

template<class T>
struct StackHandler {
protected:
  StackElement<T>* temp = NULL;
public:
  StackElement<T>* first = NULL;
  StackElement<T>* last = NULL;
  uint8_t length = 0;
  StackHandler() {}
  ~StackHandler();
  void add(T value);
  void remove();
  void clear();
  void reverse();
  T* valueFromLast(uint8_t i = 0);
};

template<class T>
StackElement<T>::StackElement(T val, StackElement<T>* prev) {
  value = val;
  previous = prev;
}

template<class T>
StackHandler<T>::~StackHandler() {
  clear();
}

template<class T>
void StackHandler<T>::add(T value) {
  switch (length) {
    case 0:
      first = new StackElement<T>(value, NULL);
      last = first;
      break;
    default:
      temp = last;
      last = new StackElement<T>(value, temp);
  }
  length++;
}
template<class T>
void StackHandler<T>::remove() {
  if (length > 1) {
    temp = last->previous;
    delete last;
    last = temp;
  } else if (length == 1) {
    delete last;
    first = NULL;
  }
    length--;
}
template<class T>
void StackHandler<T>::clear() {
  while (length > 0) {
    remove();
  }
}
template<class T>
void StackHandler<T>::reverse() {
  temp = NULL;                   //remember previous
  StackElement<T>* carriage;     //current editing element
  StackElement<T>* next = last;  //start from last(to first)
  while (carriage != first) {
    carriage = next;            //shift
    next = carriage->previous;  //aim next
    carriage->previous = temp;  //connect to reverse previous
    temp = carriage;            //remember previous
  }
  temp = last;  //swap last and first
  last = first;
  first = temp;
}
template<class T>
T* StackHandler<T>::valueFromLast(uint8_t i = 0) {
  temp = last;
  for (uint8_t l = 1; l < i; l++) {
    temp = temp->previous;
  }
  return (&temp->value);
}