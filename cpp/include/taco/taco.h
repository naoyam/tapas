#ifndef TACO_TACO_H_
#define TACO_TACO_H_

#include "taco/common.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

namespace taco {

template <int DIM, class FP, class PT>
class Cell {
 public:
  bool IsRoot() const; // TODO
  bool IsLeaf() const;// TODO
  index_t np() const;// TODO
  Cell &SubCell(int idx) const; // TODO
  bool operator==(const Cell &c) const;
  template <class T>
  bool operator==(const T &x) const { return false; }
  
  // Iterator interface
  typedef Cell value_type;
  int size() const { return 1; }
  const Cell &operator*() const {
    return *this;
  }
  const Cell &operator++() const {
    return *this;
  }
  PT *Particle(index_t idx) const {
    return NULL;
  }
};


template <int DIM, class FP, class PT, int OFFSET>
Cell<DIM, FP, PT> *PartitionBSP(const PT *p, index_t np,
                                const Region<DIM, FP> &r,
                                int max_np);

template <int DIM, class FP, class PT>
class SubCellIterator {
  const Cell<DIM, FP, PT> &c_;
  int idx_;
 public:
  typedef Cell<DIM, FP, PT> value_type;
  SubCellIterator(const Cell<DIM, FP, PT> &c): c_(c), idx_(0) {}
  int size() const {
    if (c_.IsLeaf()) {
      return 0;
    } else {
      return 1 << DIM;
    }
  }
  const Cell<DIM, FP, PT> &operator*() const {
    return c_.SubCell(idx_);
  }
  SubCellIterator &operator++() {
    ++idx_;
  }
  bool operator==(const SubCellIterator &x) const {
    return c_ == x.c_;
  }
  template <class T>
  bool operator==(const T &x) const { return false; }
};

template <int DIM, class FP, class PT>
SubCellIterator<DIM, FP, PT> SubCells(const Cell<DIM, FP, PT> &c) {
  return SubCellIterator<DIM, FP, PT>(c);
}


template <class T1, class T2>
class ProductIterator {
  int idx1_;
  int idx2_;
  const T1 &t1_;
  const T2 &t2_;
 public:
  ProductIterator(const T1 &t1, const T2 &t2): idx1_(0), idx2_(0), t1_(t1), t2_(t2) {
    if (t1_ == t2_) {
      idx2_ = 1;
    }
  }
  int size() const {
    if (t1_ == t2_) {
      return (t1_.size() + 1) * t1_.size() / 2;
    } else {
      return t1_.size() * t2_.size();
    }
  }
  const typename T1::value_type &first() const {
    return *t1_;
  }
  const typename T2::value_type &second() const {
    return *t2_;
  }
  ProductIterator<T1, T2> &operator++() {
    if (idx2_ + 1 == t2_.size()) {
      ++idx1_;
      if (t1_ == t2_) {
        idx2_ = idx1_ + 1;
      } else {
        idx2_ = 0;
      }
    } else {
      ++idx2_;
    }
    return *this;
  }
};

template <class T1, class T2>
ProductIterator<T1, T2> Product(const T1 &t1, const T2 &t2) {
  return ProductIterator<T1, T2>(t1, t2);
}

template <int DIM, class FP, class PT, class T1, class T2, class... Args>
void Map(void (*f)(const Cell<DIM, FP, PT>&, const Cell<DIM, FP, PT>&, Args...), ProductIterator<T1, T2> prod,
         Args...args) {
  for (int i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
}

template <int DIM, class FP, class PT>
void AccumulateForce(const Cell<DIM, FP, PT>&, index_t idx, Vec<DIM, FP> f);


} // namespace taco

#endif /* TACO_TACO_H_ */
