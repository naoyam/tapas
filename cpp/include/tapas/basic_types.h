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
  FP min(int d) const {
    return min_[d];
  }
  FP &min(int d) {
    return min_[d];
  }
  Vec<DIM, FP> &max() {
    return max_;
  }
  FP max(int d) const {
    return max_[d];
  }
  FP &max(int d) {
    return max_[d];
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
  
  Region PartitionBSP(int pos) const {
    Region sr = *this;
    for (int i = 0; i < DIM; ++i) {
      FP center = sr.min(i) + sr.width(i) / 2;
      if ((pos & 1) == 0) {
        sr.max(i) = center;
      } else {
        sr.min(i) = center;
      }
      pos >>= 1;
    }
    return sr;
  }
#if 0  
  Region PartitionBSP(const Vec<DIM, int> &pos) const {
    int p = 0;
    for (int i = 0; i < DIM; ++i) {
      p <<= 1;
      if (pos[i]) p |= 1;
    }
    return PartitionBSP(p);
  }
#endif
  std::ostream &Print(std::ostream &os) const {
    os << "{" << min_ << ", " << max_ << "}";
    return os;
  }
};

template <int DIM, class FP>
std::ostream &operator<<(std::ostream &os, const Region<DIM, FP> &r) {
  return r.Print(os);
}

template <int DIM, class FP, int OFFSET>
struct ParticlePosOffset {
  static FP *pos(const void *base, int dim) {
    return (FP *)(((intptr_t)base) + OFFSET + dim * sizeof(FP));
  }
  static Vec<DIM, FP> vec(const void *base) {
    FP *p = (FP *)(((intptr_t)base) + OFFSET);
    return Vec<DIM, FP>(p);
  }

};

} // namespace tapas

#endif // TAPAS_BASIC_TYPES_H_
