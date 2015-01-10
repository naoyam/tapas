#ifndef TAPAS_BASIC_TYPES_H_
#define TAPAS_BASIC_TYPES_H_

#include <cstdint>

#include "tapas/common.h"
#include "tapas/vec.h"

namespace tapas {

template <class FP, class T>
FP REAL(const T &x) {
  return (FP)x;
}   

template <class TSP> // Tapas Static Params (define in common.h)
class Region {
    static const int Dim = TSP::Dim;
    typedef typename TSP::FP FP;
 private:
  Vec<Dim, FP> min_;
  Vec<Dim, FP> max_;
 public:
  Region(const Vec<Dim, FP> &min, const Vec<Dim, FP> &max):
      min_(min), max_(max) {}
  Region() {}

  Vec<Dim, FP> &min() {
    return min_;
  }
  FP min(int d) const {
    return min_[d];
  }
  FP &min(int d) {
    return min_[d];
  }
  Vec<Dim, FP> &max() {
    return max_;
  }
  FP max(int d) const {
    return max_[d];
  }
  FP &max(int d) {
    return max_[d];
  }
  const Vec<Dim, FP> &min() const {
    return min_;
  }
  const Vec<Dim, FP> &max() const {
    return max_;
  }
  FP width(int d) const {
    return max_[d] - min_[d];
  }
  const Vec<Dim, FP> width() const {
    return max_ - min_;
  }
  
  Region PartitionBSP(int pos) const {
    Region sr = *this;
    for (int i = 0; i < Dim; ++i) {
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
  Region PartitionBSP(const Vec<Dim, int> &pos) const {
    int p = 0;
    for (int i = 0; i < Dim; ++i) {
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

template <class TSP>
std::ostream &operator<<(std::ostream &os, const Region<TSP> &r) {
    return r.Print(os);
}

template <int Dim, class FP, int OFFSET>
struct ParticlePosOffset {
  static FP *pos(const void *base, int dim) {
    return (FP *)(((intptr_t)base) + OFFSET + dim * sizeof(FP));
  }
  static Vec<Dim, FP> vec(const void *base) {
    FP *p = (FP *)(((intptr_t)base) + OFFSET);
    return Vec<Dim, FP>(p);
  }

};

} // namespace tapas

#endif // TAPAS_BASIC_TYPES_H_
