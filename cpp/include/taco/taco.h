#ifndef TACO_TACO_H_
#define TACO_TACO_H_

#include "taco/common.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

namespace taco {

template <int DIM, class FP, class PT, class PT_ATTR>
class SubCellIterator;

template <int DIM, class FP, class PT, class PT_ATTR>
class Cell {
  PT_ATTR *dummy_;
  PT PT_dummy_;
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
  const PT &particle(index_t idx) const {
    return PT_dummy_;
  }
#if 0  
  const PT_ATTR &ReadAttr(index_t idx) const {
    return dummy_[0];
  }
#endif  
  PT_ATTR &attr(index_t idx) const {
    return dummy_[0];
  }
  SubCellIterator<DIM, FP, PT, PT_ATTR> subcells() const;
};


template <int DIM, class FP, class PT, int OFFSET, class PT_ATTR>
Cell<DIM, FP, PT, PT_ATTR> *PartitionBSP(const PT *p, index_t np,
                                      const Region<DIM, FP> &r,
                                      int max_np);

template <int DIM, class FP, class PT, class PT_ATTR>
class SubCellIterator {
  const Cell<DIM, FP, PT, PT_ATTR> &c_;
  int idx_;
 public:
  typedef Cell<DIM, FP, PT, PT_ATTR> value_type;
  SubCellIterator(const Cell<DIM, FP, PT, PT_ATTR> &c): c_(c), idx_(0) {}
  int size() const {
    if (c_.IsLeaf()) {
      return 0;
    } else {
      return 1 << DIM;
    }
  }
  const Cell<DIM, FP, PT, PT_ATTR> &operator*() const {
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



template <class T1, class T2>
class ProductIterator {
  int idx1_;
  int idx2_;
  const T1 &t1_;
  const T2 &t2_;
 public:
  ProductIterator(const T1 &t1, const T2 &t2):
      idx1_(0), idx2_(0), t1_(t1), t2_(t2) {
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

template <int DIM, class FP, class PT, class PT_ATTR, class T1, class T2, class... Args>
PT_ATTR *Map(void (*f)(const Cell<DIM, FP, PT, PT_ATTR>&, const Cell<DIM, FP, PT, PT_ATTR>&, Args...), ProductIterator<T1, T2> prod, Args...args) {
  for (int i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
  return NULL;
}

} // namespace taco

template <int DIM, class FP, class PT, class PT_ATTR>
taco::SubCellIterator<DIM, FP, PT, PT_ATTR> taco::Cell<DIM, FP, PT, PT_ATTR>::
subcells() const {
  return SubCellIterator<DIM, FP, PT, PT_ATTR>(*this);
}


#endif /* TACO_TACO_H_ */
