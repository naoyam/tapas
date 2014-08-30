#ifndef TACO_VEC_H_
#define TACO_VEC_H_

#include <cstdarg>

#include "taco/common.h"

namespace taco {

template <int DIM, class FP>
class Vec {
  FP x_[DIM];
 public:
  Vec() {}
  explicit Vec(const FP* v) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] = v[i];
    }
  }
#if 0  
  template <class... Args>  
  explicit Vec(Args... args): x_{args...} {}  
  Vec(const Vec<DIM, FP> &v) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] = v[i];
    }
  }
#endif
  Vec(const FP &x1) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] = x1;
    }
  }
  Vec(const FP &x1, const FP &x2):
      x_{x1, x2} {}
  Vec(const FP &x1, const FP &x2,
      const FP &x3): x_{x1, x2, x3} {}
  Vec(const FP &x1, const FP &x2,
      const FP &x3, const FP &x4):
      x_{x1, x2, x3, x4} {}


  FP &operator[](int i) {
    return x_[i];
  }
  const FP &operator[](int i) const {
    return x_[i];
  }

  Vec operator-() const {
    Vec v(*this);
    for (int i = 0; i < DIM; ++i) {
      v.x_[i] = -x_[i];
    }
    return v;
  }

  Vec operator-=(const Vec &v) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] -= v[i];
    }
    return *this;
  }

  Vec operator/=(const Vec &v) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] /= v[i];
    }
    return *this;
  }

  bool operator>(const FP &v) const {
    for (int i = 0; i < DIM; ++i) {
      if (!(x_[i] > v)) return false;
    }
    return true;
  }
  
  bool operator>=(const FP &v) const {
    for (int i = 0; i < DIM; ++i) {
      if (!(x_[i] >= v)) return false;
    }
    return true;
  }
  
  bool operator<(const FP &v) const {
    for (int i = 0; i < DIM; ++i) {
      if (!(x_[i] < v)) return false;
    }
    return true;
  }

  bool operator<=(const FP &v) const {
    for (int i = 0; i < DIM; ++i) {
      if (!(x_[i] <= v)) return false;
    }
    return true;
  }
      
  ostream &print(ostream &os) const {
    StringJoin sj(", ");
    for (int i = 0; i < DIM; ++i) {
      sj << x_[i];
    }
    os << sj;
    return os;
  }
};


template <int DIM, class FP>
std::ostream &operator<<(std::ostream &os, const Vec<DIM, FP> &v) {
  return v.print(os);
}

}

#endif /* TACO_VEC_H_ */
