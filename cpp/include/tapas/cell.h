#ifndef TAPAS_CELL_H_
#define TAPAS_CELL_H_

#include "tapas/common.h"
#include "tapas/vec.h"
#include "tapas/basic_types.h"

namespace tapas {

//template <int DIM, class BT, class BT_ATTR, class CellType>
template <class CellType> class BodyIterator;
template <class CellType> class SubCellIterator;

template<class TSP> // TSP=TapasStaticParams
class Cell {
  public:
    static const int Dim = TSP::Dim;
    typedef typename TSP::FP FP;
    typedef typename TSP::BT BT;
    typedef typename TSP::BT_ATTR BT_ATTR;
    typedef typename TSP::ATTR ATTR;
  protected:
#if 0  
    BT_ATTR *dummy_;
    BT::type *BT_dummy_;
#endif  
    ATTR attr_; // can be omitted when ATTR=NONE
    Region<TSP> region_;
    index_t bid_;
    index_t nb_;
  public:
  Cell(const Region<TSP> &region, index_t bid, index_t nb):
      region_(region), bid_(bid), nb_(nb) {}
  index_t bid() const { return bid_; }
  index_t nb() const { return nb_; }
  const Region<TSP> &region() const {
    return region_;
  }
  FP width(int d) const {
    return region_.width(d);
  }
  Vec<Dim, FP> width() const {
    return region_.width();
  }
  FP center(int d) const {
    return region_.min(d) + width(d) / 2;
  }
  Vec<Dim, FP> center() const {
    return region_.min() + width() / 2;
  }
  
  bool operator==(const Cell &c) const;
  template <class T>
  bool operator==(const T &) const { return false; }
  
  // Iterator interface (to be obsoleted)
#if 0  
  typedef Cell value_type;
#endif  
  typedef ATTR attr_type;
#if 0  
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
#endif
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

#endif /* TAPAS_CELL_H_ */
