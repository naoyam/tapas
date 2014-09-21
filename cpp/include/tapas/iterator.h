#ifndef TAPAS_ITERATOR_H_
#define TAPAS_ITERATOR_H_

#include "tapas/cell.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=tapas::NONE
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR

namespace tapas {
template <int DIM, class BT, class BT_ATTR, class CellType>
class BodyIterator {
  const CellType &c_;
  index_t idx_;
 public:
  BodyIterator(const CellType &c)
      : c_(c), idx_(0) {}
#if 0  
  typedef BT::type value_type;
#else  
  typedef BodyIterator value_type;
#endif  
  typedef BT_ATTR attr_type;  
  index_t size() const {
    return c_.nb();
  }
#if 0
  BT::type &&operator*() const {
    return c_.body(idx_);
  }
#else
  const BodyIterator &operator*() const {
    return *this;
  }
  BodyIterator &operator*() {
    return *this;
  }
#endif    
  typename BT::type *operator->() const {
    return &(c_.body(idx_));
  }
  BT_ATTR &attr() const {
    return c_.body_attr(idx_);
  }
  CellType &cell() {
    return c_;
  }
  const CellType &cell() const {
    return c_;
  }
  typename BT::type &operator++() {
    return c_.body(++idx_);
  }
  bool operator==(const BodyIterator &x) const {
    return c_ == x.c_ && idx_ == x.idx_;
  }
  template <class T>
  bool operator==(const T &x) const { return false; }
};

#if 0
template <CELL_TEMPLATE_PARAMS>
class SubCellIterator {
  const Cell<CELL_TEMPLATE_ARGS> &c_;
  int idx_;
 public:
  typedef Cell<CELL_TEMPLATE_ARGS> value_type;
  typedef ATTR attr_type;  
  SubCellIterator(const Cell<CELL_TEMPLATE_ARGS> &c): c_(c), idx_(0) {}
  unsigned size() const {
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
}; // class SubCellIterator
#else
template <int DIM, class CELL>
class SubCellIterator {
  const CELL &c_;
  int idx_;
 public:
  typedef CELL value_type;
  typedef typename CELL::attr_type attr_type;  
  SubCellIterator(const CELL &c): c_(c), idx_(0) {}
  unsigned size() const {
    if (c_.IsLeaf()) {
      return 0;
    } else {
      return 1 << DIM;
    }
  }
  CELL &operator*() const {
    return c_.subcell(idx_);
  }
  CELL &operator++() {
    return c_.subcell(++idx_);
  }
  bool operator==(const SubCellIterator &x) const {
    return c_ == x.c_;
  }
  template <class T>
  bool operator==(const T &x) const { return false; }
}; // class SubCellIterator
#endif

template <class T1, class T2>
class ProductIterator {
  index_t idx1_;
  index_t idx2_;
  T1 &t1_;
  T2 &t2_;
 public:
  ProductIterator(T1 &t1, T2 &t2):
      idx1_(0), idx2_(0), t1_(t1), t2_(t2) {
    if (t1_ == t2_) {
      idx2_ = 1;
    }
  }
  unsigned size() const {
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
  typename T1::attr_type &attr_first() const {
    return t1_.attr();
  }
  typename T2::attr_type &attr_second() const {
    return t2_.attr();
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
}; // class ProductIterator

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


} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif // TAPAS_ITERATOR_H_ 
