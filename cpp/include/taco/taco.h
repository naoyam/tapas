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
class ParticleIterator;

template <CELL_TEMPLATE_PARAMS_NO_DEF>
class SubCellIterator;

template <CELL_TEMPLATE_PARAMS>
class Cell {
  friend class ParticleIterator<CELL_TEMPLATE_ARGS>;
  PT_ATTR *dummy_;
  PT *PT_dummy_;
  ATTR attr_;
  Vec<DIM, FP> min_;
  Vec<DIM, FP> max_;
 public:
  bool IsRoot() const; // TODO
  bool IsLeaf() const;// TODO
  index_t np() const;// TODO
  int nsubcells() const;// TODO  
  Cell &subcell(int idx) const; // TODO
  Cell &parent() const; // TODO  
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
  PT &particle(index_t idx) const {
    return PT_dummy_[0];
  }
  SubCellIterator<CELL_TEMPLATE_ARGS> subcells() const;
  ParticleIterator<CELL_TEMPLATE_ARGS> particles() const;
  ATTR &attr() {
    return attr_;
  }
  const ATTR &attr() const {
    return attr_;
  }
  FP width(int d) const {
    return max_[d] - min_[d];
  }
  PT_ATTR *particle_attrs() const;
 protected:
  PT_ATTR &attr(index_t idx) const {
    return dummy_[0];
  }
};


template <CELL_TEMPLATE_PARAMS>
Cell<CELL_TEMPLATE_ARGS> *PartitionBSP(const PT *p, index_t np,
                                       const Region<DIM, FP> &r,
                                       int max_np);

template <CELL_TEMPLATE_PARAMS>
class ParticleIterator {
  Cell<CELL_TEMPLATE_ARGS> &c_;
  index_t idx_;
 public:
  ParticleIterator(Cell<CELL_TEMPLATE_ARGS> &c)
      : c_(c), idx_(0) {}
#if 0  
  typedef PT value_type;
#else  
  typedef ParticleIterator value_type;
#endif  
  typedef PT_ATTR attr_type;  
  index_t size() const {
    return c_.np();
  }
#if 0
  PT &&operator*() const {
    return c_.particle(idx_);
  }
#else
  const ParticleIterator &operator*() const {
    return *this;
  }
  ParticleIterator &operator*() {
    return *this;
  }
#endif    
  PT *operator->() const {
    return &(c_.particle(idx_));
  }
  PT_ATTR &attr() const {
    return c_.attr(idx_);
  }
  Cell<CELL_TEMPLATE_ARGS> &cell() {
    return c_;
  }
  const Cell<CELL_TEMPLATE_ARGS> &cell() const {
    return c_;
  }
  PT &operator++() {
    return c_.particle(++idx_);
  }
  bool operator==(const ParticleIterator &x) const {
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
};

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
void Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Cell<CELL_TEMPLATE_ARGS>&, Args...), ProductIterator<T1, T2> prod, Args...args) {
  for (unsigned i = 0; i < prod.size(); ++i) {
    f(prod.first(), prod.second(), args...);
    ++prod;
  }
}

template <CELL_TEMPLATE_PARAMS, class T, class... Args>
void Map(void (*f)(Cell<CELL_TEMPLATE_ARGS>&, Args...), T iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(*iter, args...);
    ++iter;
  }
}
#if 0
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, const PT &p2, PT_ATTR &a2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), iter.second(), iter.attr_second(), args...);
    ++iter;
  }
}

template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, const PT &p2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), iter.second(), args...);
    ++iter;
  }
}
#else
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(ParticleIterator<CELL_TEMPLATE_ARGS> &p1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (index_t i = 0; i < iter.size(); ++i) {
    f(iter, args...);
    ++iter;
  }
}
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(ParticleIterator<CELL_TEMPLATE_ARGS> &p1,
                   ParticleIterator<CELL_TEMPLATE_ARGS> &p2, Args...),
         ProductIterator<ParticleIterator<CELL_TEMPLATE_ARGS>, ParticleIterator<CELL_TEMPLATE_ARGS> >
         iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.second(), args...);
    ++iter;
  }
}
#endif
template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, PT_ATTR &a1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), iter.attr_first(), args...);
    ++iter;
  }
}

template <CELL_TEMPLATE_PARAMS, class... Args>
void Map(void (*f)(const PT &p1, Args...),
         ParticleIterator<CELL_TEMPLATE_ARGS> iter, Args...args) {
  for (unsigned i = 0; i < iter.size(); ++i) {
    f(iter.first(), args...);
    ++iter;
  }
}



} // namespace taco

template <CELL_TEMPLATE_PARAMS_NO_DEF>
taco::SubCellIterator<CELL_TEMPLATE_ARGS> taco::Cell<CELL_TEMPLATE_ARGS>::
subcells() const {
  return SubCellIterator<CELL_TEMPLATE_ARGS>(*this);
}


#endif /* TACO_TACO_H_ */
