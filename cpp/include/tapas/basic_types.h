#ifndef TAPAS_BASIC_TYPES_H_
#define TAPAS_BASIC_TYPES_H_

#include <cstdint>

#include "tapas/common.h"
#include "tapas/vec.h"

namespace tapas {

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
  FP width(int d) const {
    return max_[d] - min_[d];
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

} // namespace tapas

#endif // TAPAS_BASIC_TYPES_H_
