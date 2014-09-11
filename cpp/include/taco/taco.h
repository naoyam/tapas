#ifndef TACO_TACO_H_
#define TACO_TACO_H_

#include "taco/common.h"
#include "taco/vec.h"
#include "taco/basic_types.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class PT, int POS_OFFSET, class PT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class PT, int POS_OFFSET, class PT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, PT, POS_OFFSET, PT_ATTR, ATTR

namespace taco {

template <CELL_TEMPLATE_PARAMS_NO_DEF>
class SubCellIterator;

template <CELL_TEMPLATE_PARAMS>
class Cell {
  PT_ATTR *dummy_;
  PT PT_dummy_;
  ATTR attr_;
  Vec<DIM, FP> min_;
  Vec<DIM, FP> max_;
 public:
  bool IsRoot() const; // TODO
  bool IsLeaf() const;// TODO
  index_t np() const;// TODO
  int nsubcells() const;// TODO  
  Cell &subcell(int idx) const; // TODO
  bool operator==(const Cell &c) const;
  template <class T>
  bool operator==(const T &x) const { return false; }
  
  // Iterator interface
  typedef Cell value_type;
  int size() const { return 1; }
  Cell &operator*() {
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
  SubCellIterator<CELL_TEMPLATE_ARGS> subcells() const;
  ATTR &attr() {
    return attr_;
  }
  const ATTR &attr() const {
    return attr_;
  }
  FP width(int d) const {
    return max_[d] - min_[d];
  }
};


template <CELL_TEMPLATE_PARAMS>
Cell<CELL_TEMPLATE_ARGS> *PartitionBSP(const PT *p, index_t np,
                                       const Region<DIM, FP> &r,
                                       int max_np);

template <CELL_TEMPLATE_PARAMS>
class SubCellIterator {
  const Cell<CELL_TEMPLATE_ARGS> &c_;
  int idx_;
 public:
  typedef Cell<CELL_TEMPLATE_ARGS> value_type;
  SubCellIterator(const Cell<CELL_TEMPLATE_ARGS> &c): c_(c), idx_(0) {}
  int size() const {
    if (c_.IsLeaf()) {
      return 0;
    } else {
      return 1 << DIM;
    }
  }
  Cell<CELL_TEMPLATE_ARGS> &operator*() const {
    return c_.subcell(idx_);
  }
  Cell<CELL_TEMPLATE_ARGS> &operator++() {
    return c_.subcell(++idx_);
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
  T1 &t1_;
  T2 &t2_;
 public:
  ProductIterator(T1 &t1, T2 &t2):
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
  typename T1::value_type &first() const {
    return *t1_;
  }
  typename T2::value_type &second() const {
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
ProductIterator<T1, T2> Product(T1 t1, T2 t2) {
  return ProductIterator<T1, T2>(t1, t2);
}

template <CELL_TEMPLATE_PARAMS, class T2>
ProductIterator<Cell<CELL_TEMPLATE_ARGS>, T2> Product(
    Cell<CELL_TEMPLATE_ARGS> &c, T2 t2) {
  return ProductIterator<Cell<CELL_TEMPLATE_ARGS>, T2>(c, t2);
}

template <class T1, CELL_TEMPLATE_PARAMS>
ProductIterator<T1, Cell<CELL_TEMPLATE_ARGS> > Product(
    T1 t1, Cell<CELL_TEMPLATE_ARGS> &c) {
  return ProductIterator<T1, Cell<CELL_TEMPLATE_ARGS> >(t1, c);
}

template <CELL_TEMPLATE_PARAMS>
ProductIterator<Cell<CELL_TEMPLATE_ARGS>, Cell<CELL_TEMPLATE_ARGS> > Product(
    Cell<CELL_TEMPLATE_ARGS> &c1,
    Cell<CELL_TEMPLATE_ARGS> &c2) {
  return ProductIterator<Cell<CELL_TEMPLATE_ARGS>, Cell<CELL_TEMPLATE_ARGS> >(c1, c2);
}

template <CELL_TEMPLATE_PARAMS, class T1, class T2, class... Args>
PT_ATTR *Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Cell<CELL_TEMPLATE_ARGS>&, Args...), ProductIterator<T1, T2> prod, Args...args) {
  for (int i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
  return NULL;
}

template <CELL_TEMPLATE_PARAMS, class T, class... Args>
PT_ATTR *Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Args...), T iter, Args...args) {
  for (int i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    ++iter;
  }
  return NULL;
}

} // namespace taco

template <CELL_TEMPLATE_PARAMS_NO_DEF>
taco::SubCellIterator<CELL_TEMPLATE_ARGS> taco::Cell<CELL_TEMPLATE_ARGS>::
subcells() const {
  return SubCellIterator<CELL_TEMPLATE_ARGS>(*this);
}


#endif /* TACO_TACO_H_ */
