#pragma once

int8_t Mod(int8_t i, int8_t a) {
  return ((a - (a / i - 1) * i) % i);
}
int8_t Mod3(int8_t a) {
  return (Mod(3, a));
}
int8_t Sign(int8_t i, int8_t a) {//even positive, odd negative
  return (((i % 2) == 0) ? a : -a);
}
int8_t BSign(bool b, int8_t a) { //false positive, true negative
  return Sign(b, a);
}
int8_t BMult(bool b, int8_t a) {
  return (b ? a : 0);
}



