#ifndef TAPAS_BITS_H
#define TAPAS_BITS_H

#include <cmath>

namespace tapas {

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


} // namespace tapas

#endif // TAPAS_BITS_H
