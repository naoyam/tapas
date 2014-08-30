#ifndef TACO_BITS_H
#define TACO_BITS_H

namespace taco {

template <int N>
struct Log2 {
  static const int x = Log2<(N>>1)>::x + 1;
};

template <>
struct Log2<0> {
  static const int x = 0;
};


} // namespace taco

#endif // TACO_BITS_H
