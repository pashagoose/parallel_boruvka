#include "is_prime.h"


bool IsPrime(uint64_t val) {
  if (val < 2) {
    return false;
  }
  for (uint64_t x = 2; x * x < val; ++x) {
    if (val % x == 0) {
      return false;
    }
  }
  return true;
}

bool IsPrimeFast(uint64_t val) {
  if (val < 2) {
    return false;
  }
  if (val == 2) {
    return true;
  }
  if ((val & 1) == 0) {
    return false;
  }
  for (uint64_t x = 3; x * x < val; x += 2) {
    if (val % x == 0) {
      return false;
    }
  }
  return true;
}

