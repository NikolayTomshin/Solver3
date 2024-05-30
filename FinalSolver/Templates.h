#pragma once
#include <inttypes.h>

template<class I> struct _Int;
template<> struct _Int<uint8_t> {
  typedef int8_t _signed;
  typedef uint8_t _unsigned;
};
template<> struct _Int<uint16_t> {
  typedef int16_t _signed;
  typedef uint16_t _unsigned;
};
template<> struct _Int<uint32_t> {
  typedef int32_t _signed;
  typedef uint32_t _unsigned;
};
template<> struct _Int<int8_t> {
  typedef int8_t _signed;
  typedef int8_t _unsigned;
};
template<> struct _Int<int16_t> {
  typedef int16_t _signed;
  typedef uint16_t _unsigned;
};
template<> struct _Int<int32_t> {
  typedef int32_t _signed;
  typedef uint32_t _unsigned;
};

template<class T>
struct _Remove_const {
  typedef T _type;
};
template<class T> struct _Remove_const<const T> {
  typedef T _type;
};
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

template<class T>
T sum(T first) {
  return first;
}
template<class T, class... Args>
T sum(T first, Args... args) {
  return first + sum(args...);
}
/*
template<class T>
T min(T first) {
  return first;
}
template<class T, class... Args>
T min(T first, Args... args) {
  T temp = min(args...);
  return (first < temp) ? first : temp;
}
template<class T>
T max(T first) {
  return first;
}
template<class T, class... Args>
T max(T first, Args... args) {
  T temp = max(args...);
  return (first < temp) ? temp : first;
}
*/