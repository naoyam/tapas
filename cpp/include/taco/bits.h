#ifndef TACO_BITS_H
#define TACO_BITS_H

#include <cmath>

namespace taco {

template <int N>
struct MinBitLen {
  static const int x = MinBitLen<(N>>1)>::x + 1;
};

template <>
struct MinBitLen<0> {
  static const int x = 0;
};

inline
int CalcMinBitLen(int x) {
  return std::floor(std::log2(x)) + 1;
}


} // namespace taco

#endif // TACO_BITS_H
