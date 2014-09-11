#ifndef TACO_BASIC_TYPES_H_
#define TACO_BASIC_TYPES_H_

#include <cstdint>

#include "taco/common.h"
#include "taco/vec.h"

namespace taco {

template <int DIM, class FP>
class Region {
 private:
  Vec<DIM, FP> min_;
  Vec<DIM, FP> max_;
 public:
  Region(const Vec<DIM, FP> &min, const Vec<DIM, FP> &max):
      min_(min), max_(max) {}
  Region() {}

  Vec<DIM, FP> &min() {
    return min_;
  }
  Vec<DIM, FP> &max() {
    return max_;
  }
  const Vec<DIM, FP> &min() const {
    return min_;
  }
  const Vec<DIM, FP> &max() const {
    return max_;
  }
};

template <int OFFSET, class FP>
struct ParticlePosOffset {
  static FP *pos(void *base, int dim) {
    return (FP *)(((intptr_t)base) + OFFSET + dim * sizeof(FP));
  }
  template <int DIM>
  static Vec<DIM, FP> vec(const void *base) {
    FP *p = (FP *)(((intptr_t)base) + OFFSET);
    return Vec<DIM, FP>(p);
  }

};

} // namespace taco

#endif // TACO_BASIC_TYPES_H_
