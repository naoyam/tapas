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

template <class CellType>
class BodyIterator {
  const CellType &c_;
  index_t idx_;
 public:
  BodyIterator(const CellType &c)
      : c_(c), idx_(0) {}
  typedef BodyIterator value_type;
  typedef typename CellType::body_attr_type attr_type;  
  index_t size() const {
    return c_.nb();
  }
  const BodyIterator &operator*() const {
    return *this;
  }
  BodyIterator &operator*() {
    return *this;
  }
  typename CellType::BODY_INFO::type *operator->() const {
    return &(c_.body(idx_));
  }
  void rewind(int idx) {
    idx_ = idx;
  }
  typename CellType::BODY_ATTR &attr() const {
    return c_.body_attr(idx_);
  }
#if 0  
  CellType &cell() {
    return c_;
  }
#endif  
  const CellType &cell() const {
    return c_;
  }
  typename CellType::BODY_INFO::type &operator++() {
    return c_.body(++idx_);
  }
  typename CellType::BODY_INFO::type &operator++(int) {
    return c_.body(idx_++);
  }
  bool operator==(const BodyIterator &x) const {
    return c_ == x.c_ && idx_ == x.idx_;
  }
  bool operator!=(const BodyIterator &x) const {
    return !operator==(x);
  }
  template <class T>
  bool operator==(const T &) const { return false; }
  bool AllowMutualInteraction(const BodyIterator &x) const {
    return c_ == x.c_;
  }
};

template <class CELL>
class CellIterator {
  CELL &c_;
 public:
  CellIterator(CELL &c): c_(c) {}
  typedef CELL value_type;
  typedef typename CELL::attr_type attr_type;
  CELL &operator*() {
    return c_;
  }
  const CELL &operator*() const {
    return c_;
  }
  CELL &operator++() {
    return c_;
  }
  CELL &operator++(int) {
    return c_;
  }
  attr_type &attr() {
    return c_.attr();
  }
  const attr_type &attr() const {
    return c_.attr();
  }
  bool operator==(const CellIterator &x) const {
    return c_ == x.c_;
  }
  template <class T>
  bool operator==(const T &) const { return false; }
  void rewind(int idx) {
  }
  int size() const {
    return 1;
  }
}; // class CellIterator

template <class CELL>
class SubCellIterator {
  const CELL &c_;
  int idx_;
 public:
  typedef CELL value_type;
  typedef typename CELL::attr_type attr_type;  
  SubCellIterator(const CELL &c): c_(c), idx_(0) {}
  int size() const {
    if (c_.IsLeaf()) {
      return 0;
    } else {
      return 1 << CELL::dim;
    }
  }
  value_type &operator*() const {
    return c_.subcell(idx_);
  }
  value_type &operator++() {
    return c_.subcell(++idx_);
  }
  value_type &operator++(int) {
    return c_.subcell(idx_++);
  }
  void rewind(int idx) {
    idx_ = idx;
  }
  bool operator==(const SubCellIterator &x) const {
    return c_ == x.c_;
  }
  template <class T>
  bool operator==(const T &x) const { return false; }
  bool AllowMutualInteraction(const SubCellIterator &x) const {
    return c_ == x.c_;
  }
}; // class SubCellIterator


template <class T1, class T2=void>
class ProductIterator {
  index_t idx1_;
  index_t idx2_;
  T1 t1_;
  T2 t2_;
 public:
  ProductIterator(const T1 &t1, const T2 &t2):
      idx1_(0), idx2_(0), t1_(t1), t2_(t2) {
  }
  index_t size() const {
    return t1_.size() * t2_.size();
  }
  const typename T1::value_type &first() const {
    return *t1_;
  }
  typename T1::value_type &first() {
    return *t1_;
  }
  const typename T2::value_type &second() const {
    return *t2_;
  }
  typename T2::value_type &second() {
    return *t2_;
  }
  typename T1::attr_type &attr_first() const {
    return t1_.attr();
  }
  typename T2::attr_type &attr_second() const {
    return t2_.attr();
  }
  
  void operator++(int) {
    if (idx2_ + 1 == t2_.size()) {
      idx1_++;
      t1_++;
      idx2_ = 0;
      t2_.rewind(idx2_);      
    } else {
      idx2_++;
      t2_++;
    }
    //return *this;
  }
}; // class ProductIterator

template <class ITER>
class ProductIterator<ITER, void> {
  index_t idx1_;
  index_t idx2_;
  ITER t1_;
  ITER t2_;
 public:
  ProductIterator(const ITER &t1, const ITER &t2):
      idx1_(0), idx2_(0), t1_(t1), t2_(t2) {
    if (t1_.AllowMutualInteraction(t2_)) {
      TAPAS_LOG_ERROR() << "mutual interaction\n";
#if 0 // No self interaction     
      idx2_ = 1;
      t2_.rewind(idx2_);      
#endif
    }
  }
  index_t size() const {
    if (t1_.AllowMutualInteraction(t2_)) {
#if 0  // No self interaction
      return (t1_.size() - 1) * t1_.size() / 2;
#else
      return (t1_.size() + 1) * t1_.size() / 2;      
#endif
    } else {
      return t1_.size() * t2_.size();
    }
  }
  const typename ITER::value_type &first() const {
    return *t1_;
  }
  typename ITER::value_type &first() {
    return *t1_;
  }
  const typename ITER::value_type &second() const {
    return *t2_;
  }
  typename ITER::value_type &second() {
    return *t2_;
  }
  typename ITER::attr_type &attr_first() const {
    return t1_.attr();
  }
  typename ITER::attr_type &attr_second() const {
    return t2_.attr();
  }
  
  void operator++(int) {
    if (idx2_ + 1 == t2_.size()) {
      idx1_++;
      t1_++;
      if (t1_.AllowMutualInteraction(t2_)) {
#if 0 // No self interaction        
        idx2_ = idx1_ + 1;
#else
        idx2_ = idx1_;
#endif        
      } else {
        idx2_ = 0;
      }
      t2_.rewind(idx2_);      
    } else {
      idx2_++;
      t2_++;
    }
  }
}; // class ProductIterator

template <class T1, class T2>
ProductIterator<T1, T2> Product(T1 t1, T2 t2) {
  TAPAS_LOG_ERROR() << "Product(X, X)" << std::endl;  
  return ProductIterator<T1, T2>(t1, t2);
}


template <class CELL>
ProductIterator<SubCellIterator<CELL >> Product(
    SubCellIterator<CELL> c1, SubCellIterator<CELL> c2) {
  TAPAS_LOG_ERROR() << "Product(SubCellIterator, SubCellIterator)" << std::endl;
  return ProductIterator<SubCellIterator<CELL> >(c1, c2);
}

template <class CELL>
ProductIterator<BodyIterator<CELL> > Product(
    BodyIterator<CELL> c1, BodyIterator<CELL> c2) {
  TAPAS_LOG_ERROR() << "Product(BodyIterator, BodyIterator)" << std::endl;
  return ProductIterator<BodyIterator<CELL> >(c1, c2);
}


} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif // TAPAS_ITERATOR_H_ 
