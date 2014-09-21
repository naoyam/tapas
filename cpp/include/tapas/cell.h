#ifndef TAPAS_CELL_H_
#define TAPAS_CELL_H_

#include "tapas/common.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=float
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR

namespace tapas {

template <CELL_TEMPLATE_PARAMS_NO_DEF>
class BodyIterator;

template <CELL_TEMPLATE_PARAMS_NO_DEF>
class SubCellIterator;

template <CELL_TEMPLATE_PARAMS>
class Cell {
  friend class BodyIterator<CELL_TEMPLATE_ARGS>;
 protected:
#if 0  
  BT_ATTR *dummy_;
  BT::type *BT_dummy_;
#endif  
  ATTR attr_; // can be omitted when ATTR=NONE
  Region<DIM, FP> region_;
  index_t bid_;
  index_t nb_;
 public:
  Cell(const Region<DIM, FP> &region, index_t bid, index_t nb):
      region_(region), bid_(bid), nb_(nb) {}
  index_t bid() const { return bid_; }
  index_t nb() const { return nb_; }
  const Region<DIM, FP> &region() const {
    return region_;
  }
  FP width(int d) const {
    return region_.width(d);
  }
  
  bool operator==(const Cell &c) const;
  template <class T>
  bool operator==(const T &x) const { return false; }
  
  // Iterator interface
  typedef Cell value_type;
  typedef ATTR attr_type;  
  unsigned size() const { return 1; }
  Cell &operator*() {
    return *this;
  }
  const Cell &operator++() const {
    return *this;
  }

  SubCellIterator<CELL_TEMPLATE_ARGS> subcells() const;
  BodyIterator<CELL_TEMPLATE_ARGS> bodies() const;

  // Cell attributes
  ATTR &attr() {
    return attr_;
  }
  const ATTR &attr() const {
    return attr_;
  }

  // Following methods are to be implemented in sub-classes 
  bool IsRoot() const;
  bool IsLeaf() const;
  int nsubcells() const;
  Cell &subcell(int idx) const; 
  Cell &parent() const;
  typename BT::type &body(index_t idx) const;
  BT_ATTR *body_attrs() const;
  
 protected:
  BT_ATTR &attr(index_t idx) const;
  
}; // class Cell

template <CELL_TEMPLATE_PARAMS>
class BodyIterator {
  Cell<CELL_TEMPLATE_ARGS> &c_;
  index_t idx_;
 public:
  BodyIterator(Cell<CELL_TEMPLATE_ARGS> &c)
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
    return c_.attr(idx_);
  }
  Cell<CELL_TEMPLATE_ARGS> &cell() {
    return c_;
  }
  const Cell<CELL_TEMPLATE_ARGS> &cell() const {
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

template <CELL_TEMPLATE_PARAMS_NO_DEF>
SubCellIterator<CELL_TEMPLATE_ARGS> Cell<CELL_TEMPLATE_ARGS>::
subcells() const {
  return SubCellIterator<CELL_TEMPLATE_ARGS>(*this);
}

template <CELL_TEMPLATE_PARAMS_NO_DEF>
BodyIterator<CELL_TEMPLATE_ARGS> Cell<CELL_TEMPLATE_ARGS>::
bodies() const {
  return BodyIterator<CELL_TEMPLATE_ARGS>(*this);
}

} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif /* TAPAS_CELL_H_ */
