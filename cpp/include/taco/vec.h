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
  template <class... Args>
  explicit Vec(Args... args): x_{args...} {}

  Vec(const Vec<DIM, FP> &v) {
    for (int i = 0; i < DIM; ++i) {
      x_[i] = v[i];
    }
  }

  FP &operator[](int i) {
    return x_[i];
  }
  const FP &operator[](int i) const {
    return x_[i];
  }

  Vec operator-() const {
    Vec v(*this);
    for (int i = 0; i < DIM; ++i) {
      v.x_[i] = x_[i];
    }
    return v;
  }
    
};

}

#endif /* TACO_VEC_H_ */
