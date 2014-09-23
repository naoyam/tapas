#ifndef TAPAS_CELL_H_
#define TAPAS_CELL_H_

#include "tapas/common.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"

#define CELL_TEMPLATE_PARAMS \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR=tapas::NONE
#define CELL_TEMPLATE_PARAMS_NO_DEF \
  int DIM, class FP, class BT, class BT_ATTR, class ATTR
#define CELL_TEMPLATE_ARGS \
  DIM, FP, BT, BT_ATTR, ATTR

namespace tapas {

template <int DIM, class BT, class BT_ATTR, class CellType>
class BodyIterator;

#if 0
template <CELL_TEMPLATE_PARAMS_NO_DEF>
class SubCellIterator;
#else
template <int DIM, class CELL>
class SubCellIterator;
#endif

template <CELL_TEMPLATE_PARAMS>
class Cell {
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
  const Cell &operator++(int) const {
    return *this;
  }
  void rewind(int idx) const {
    TAPAS_ASSERT(idx == 0);
    return;
  }

  //SubCellIterator<CELL_TEMPLATE_ARGS> subcells() const;
  //BodyIterator<CELL_TEMPLATE_ARGS> bodies() const;

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
  virtual void make_pure_virtual() const = 0;
}; // class Cell


} // namespace tapas

#undef CELL_TEMPLATE_PARAMS
#undef CELL_TEMPLATE_PARAMS_NO_DEF
#undef CELL_TEMPLATE_ARGS

#endif /* TAPAS_CELL_H_ */
